/*
 * getfat.cpp -- FAT32 lab tool (Windows / Win32, interactive menu)
 *
 * Works on a REAL physical disk (\\\\\\\\.\\\\PhysicalDriveN), selected by the user,
 * or on a raw image file given as the first command-line argument (safe test
 * mode, no admin needed).
 *
 * Features (all output in Chinese, UTF-8 console):
 *   1. List / select a physical disk, show capacity.
 *   2. Parse and annotate the MBR (primary partition table) in Chinese:
 *        boot flag, CHS start/end, partition type, LBA start, sector count,
 *        byte range, size.  Also flags GPT-protective / no-active-partition.
 *   3. Compute the tail free region: the space between the end of the last
 *        partition and the end of the disk (alignment leftover, belongs to no
 *        partition, invisible to the OS).  Show start/end/size.
 *   4. Self-made hidden file system over the tail region: 10 entries x 19
 *        bytes (8.3 short name: name 8 + ext 3 + u32 start + u32 inclusive
 *        end offsets) ended by a 0x55AA marker, followed by the file bytes.
 *        A manager lists / adds files (each source name must fit 8.3) from
 *        any mounted disk / exports them back out into a chosen folder /
 *        deletes them / defrags (repacks head-to-tail); stored files persist
 *        on disk across runs.
 *   5. FAT32 demo: rewrite the "first cluster" field of EVERY 8.3 directory
 *        entry (files AND sub-directories, whole tree, "." and ".." kept)
 *        inside the FAT32 data area so they all point at one chosen file's
 *        start cluster.  The chosen file may be given as a path inside the
 *        volume or as a raw cluster number.  Includes a dry-run preview.
 *   6. Shrink the last FAT32 partition by editing its MBR partition-table
 *        entry (sector count), so the tail free region grows.  To stay safe it
 *        parses the partition BPB and scans FAT0 for the highest allocated
 *        cluster, then refuses any shrink that would cut into used clusters.
 *
 * Writes are destructive and irreversible.  The program always requires the
 * user to type the disk number again to confirm before touching the disk, and
 * refuses to write to disk 0 unless explicitly overridden.  Admin is required
 * for physical disks; when not elevated the program relaunches itself via UAC.
 *
 * Build (this box, from a WSL prompt):
 *   cd /mnt/c/Users/chenj/Desktop/fat32实验
 *   x86_64-w64-mingw32-g++ -O2 -Wall -o getfat.exe getfat.cpp
 *
 * Usage:
 *   getfat                 interactive: pick a disk, then a menu
 *   getfat N               interactive on PhysicalDriveN
 *   getfat image.img       interactive on a raw disk image (safe, no admin)
 */
#include <windows.h>
#include <winioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ------------------------------------------------------------------ */
/* little helpers                                                      */
/* ------------------------------------------------------------------ */

static unsigned rd16(const unsigned char*p){return (unsigned)p[0]|((unsigned)p[1]<<8);}
static unsigned long rd32(const unsigned char*p)
{return (unsigned long)p[0]|((unsigned long)p[1]<<8)
      |((unsigned long)p[2]<<16)|((unsigned long)p[3]<<24);}
static void wr16(unsigned char*p,unsigned v){p[0]=(unsigned char)(v&255);p[1]=(unsigned char)((v>>8)&255);}

static int my_toupper(int c){return (c>='a'&&c<='z')?c-32:c;}

/* human readable size: "1.50 GiB" style */
static void human(ULONGLONG b,char*d,size_t cap)
{
    static const char*U[]={"B","KiB","MiB","GiB","TiB"};
    double v=(double)b; int u=0;
    while(v>=1024.0&&u<4){v/=1024.0;u++;}
    if(u==0)snprintf(d,cap,"%llu B",(unsigned long long)b);
    else    snprintf(d,cap,"%.2f %s",v,U[u]);
}

/* aligned buffer helpers (raw disk I/O wants sector-aligned buffers)   */
typedef struct { void*buf; void*base; size_t cap; } ABuf;
static void abuf_free(ABuf*a){if(a&&a->base){free(a->base);a->base=NULL;a->buf=NULL;a->cap=0;}}
static int abuf_ensure(ABuf*a,size_t n,size_t align)
{
    if(a->cap>=n&&a->buf)return 1;
    abuf_free(a);
    a->base=malloc(n+align);
    if(!a->base)return 0;
    a->cap=n+align;
    a->buf=(void*)((((unsigned long long)(a->base))+(unsigned long long)(align-1))
                   &~((unsigned long long)(align-1)));
    return 1;
}

/* ------------------------------------------------------------------ */
/* block device abstraction                                            */
/* ------------------------------------------------------------------ */

typedef struct {
    HANDLE    h;
    int       is_image;      /* 1 = raw file image, 0 = physical disk */
    DWORD     sector;        /* logical sector size in bytes          */
    ULONGLONG total;         /* device size in bytes                  */
    ULONGLONG part;          /* current partition start (absolute LBA)*/
    int       selpart;       /* selected FAT32 partition index (-1)    */
    char      title[96];
} Dev;

/* open a physical disk or a raw image.  returns 1 on success. */
static int dev_open(Dev*d,int disknum,const char*image,int for_write,int quiet)
{
    char   path[96];
    HANDLE h;
    DWORD  access = for_write? (GENERIC_READ|GENERIC_WRITE) : GENERIC_READ;

    memset(d,0,sizeof *d);
    d->sector=512; d->selpart=-1;

    if(image){
        h=CreateFileA(image,access,FILE_SHARE_READ|FILE_SHARE_WRITE,
                      NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
        if(h==INVALID_HANDLE_VALUE){
            if(!quiet)printf("  无法打开镜像文件 %s (err %lu)\n",image,(unsigned long)GetLastError());
            return 0;
        }
        d->h=h; d->is_image=1;
        LARGE_INTEGER sz;
        if(GetFileSizeEx(h,&sz))d->total=(ULONGLONG)sz.QuadPart;
        snprintf(d->title,sizeof d->title,"%s",image);
    } else {
        snprintf(path,sizeof path,"\\\\.\\PhysicalDrive%d",disknum);
        h=CreateFileA(path,access,FILE_SHARE_READ|FILE_SHARE_WRITE,
                      NULL,OPEN_EXISTING,0,NULL);
        if(h==INVALID_HANDLE_VALUE){
            if(!quiet){
                printf("  无法打开 %s (err %lu)",path,(unsigned long)GetLastError());
                if(GetLastError()==ERROR_ACCESS_DENIED)printf("  -- 需要管理员权限");
                printf("\n");
            }
            return 0;
        }
        d->h=h; d->is_image=0;
        GET_LENGTH_INFORMATION li; DWORD n=0;
        if(DeviceIoControl(h,IOCTL_DISK_GET_LENGTH_INFO,NULL,0,&li,sizeof li,&n,NULL))
            d->total=(ULONGLONG)li.Length.QuadPart;
        DISK_GEOMETRY_EX geo; n=0;
        if(DeviceIoControl(h,IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,NULL,0,&geo,sizeof geo,&n,NULL))
            d->sector=geo.Geometry.BytesPerSector;
        if(d->sector==0)d->sector=512;
        snprintf(d->title,sizeof d->title,"PhysicalDrive%d",disknum);
    }
    return 1;
}

static void dev_close(Dev*d){if(d->h&&d->h!=INVALID_HANDLE_VALUE)CloseHandle(d->h);d->h=NULL;}

/* read `nsec` whole sectors starting at absolute LBA `lba`. returns 1 on ok. */
static int rd_sec(Dev*d,ULONGLONG lba,unsigned nsec,void*buf)
{
    ULONGLONG start=lba*(ULONGLONG)d->sector;
    ULONGLONG need=(ULONGLONG)nsec*d->sector;
    LARGE_INTEGER pos; DWORD done=0;
    if(d->total&&start+need>d->total)return 0;
    pos.QuadPart=(LONGLONG)start;
    if(!SetFilePointerEx(d->h,pos,NULL,FILE_BEGIN))return 0;
    return (ReadFile(d->h,buf,(DWORD)need,&done,NULL)!=0)&&done==(DWORD)need;
}

/* write `nsec` whole sectors at absolute LBA `lba`. returns 1 on ok. */
static int wr_sec(Dev*d,ULONGLONG lba,unsigned nsec,const void*buf)
{
    ULONGLONG start=lba*(ULONGLONG)d->sector;
    ULONGLONG need=(ULONGLONG)nsec*d->sector;
    LARGE_INTEGER pos; DWORD done=0;
    if(d->total&&start+need>d->total)return 0;
    pos.QuadPart=(LONGLONG)start;
    if(!SetFilePointerEx(d->h,pos,NULL,FILE_BEGIN))return 0;
    return (WriteFile(d->h,buf,(DWORD)need,&done,NULL)!=0)&&done==(DWORD)need;
}

/* ------------------------------------------------------------------ */
/* read one whole file (path may be UTF-8) into a heap buffer          */
/* ------------------------------------------------------------------ */

static unsigned char*read_whole_file(const char*path,unsigned long*len,unsigned long*err)
{
    HANDLE h;
    wchar_t w[2048];
    unsigned char*buf=NULL;
    LARGE_INTEGER sz;
    unsigned long n=MultiByteToWideChar(CP_UTF8,0,path,-1,w,(int)(sizeof w/sizeof w[0]));
    if(n==0){*err=1;return NULL;}
    h=CreateFileW(w,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
                  NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(h==INVALID_HANDLE_VALUE){*err=GetLastError();return NULL;}
    if(!GetFileSizeEx(h,&sz)||sz.QuadPart<0||sz.QuadPart>0x100000000LL){
        CloseHandle(h);*err=GetLastError()?GetLastError():2;return NULL;
    }
    *len=(unsigned long)sz.QuadPart;
    if(*len==0){CloseHandle(h);*err=0;return (unsigned char*)calloc(1,1);}
    buf=(unsigned char*)malloc(*len);
    if(!buf){CloseHandle(h);*err=3;return NULL;}
    {
        DWORD total=0,done=0;
        while(total<*len){
            if(!ReadFile(h,buf+total,*len-total,&done,NULL)||done==0){
                CloseHandle(h);free(buf);*err=4;return NULL;
            }
            total+=done;
        }
    }
    CloseHandle(h);*err=0;
    return buf;
}

/* write one whole file (path may be UTF-8) from a heap buffer. returns 1 on ok. */
static int write_whole_file(const char*path,const unsigned char*buf,unsigned long len,unsigned long*err)
{
    HANDLE h;
    wchar_t w[2048];
    DWORD total=0;
    unsigned long n=MultiByteToWideChar(CP_UTF8,0,path,-1,w,(int)(sizeof w/sizeof w[0]));
    if(n==0){if(err)*err=1;return 0;}
    h=CreateFileW(w,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
                  NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(h==INVALID_HANDLE_VALUE){if(err)*err=GetLastError();return 0;}
    while(total<len){
        DWORD chunk=(len-total>(1u<<20))?(1u<<20):(DWORD)(len-total);
        DWORD done=0;
        if(!WriteFile(h,buf+total,chunk,&done,NULL)||done==0){
            CloseHandle(h);if(err)*err=5;return 0;
        }
        total+=done;
    }
    CloseHandle(h);if(err)*err=0;
    return 1;
}

/* 1 if `path` names an existing directory (path may be UTF-8). */
static int is_directory(const char*path)
{
    wchar_t w[2048]; DWORD a;
    if(!MultiByteToWideChar(CP_UTF8,0,path,-1,w,(int)(sizeof w/sizeof w[0])))return 0;
    a=GetFileAttributesW(w);
    if(a==INVALID_FILE_ATTRIBUTES)return 0;
    return (a&FILE_ATTRIBUTE_DIRECTORY)!=0;
}

/* 1 if `path` names an existing file or directory (path may be UTF-8). */
static int file_exists(const char*path)
{
    wchar_t w[2048];
    if(!MultiByteToWideChar(CP_UTF8,0,path,-1,w,(int)(sizeof w/sizeof w[0])))return 0;
    return GetFileAttributesW(w)!=INVALID_FILE_ATTRIBUTES;
}

/* ------------------------------------------------------------------ */
/* MBR partition type -> Chinese name                                  */
/* ------------------------------------------------------------------ */

static void mbr_type_name(unsigned t,char*out,size_t cap)
{
    switch(t){
    case 0x01:snprintf(out,cap,"FAT12");break;
    case 0x04:snprintf(out,cap,"FAT16 <32M");break;
    case 0x05:snprintf(out,cap,"扩展分区(CHS)");break;
    case 0x06:snprintf(out,cap,"FAT16B");break;
    case 0x07:snprintf(out,cap,"NTFS/exFAT");break;
    case 0x0B:snprintf(out,cap,"FAT32(CHS)");break;
    case 0x0C:snprintf(out,cap,"FAT32(LBA)");break;
    case 0x0E:snprintf(out,cap,"FAT16(LBA)");break;
    case 0x0F:snprintf(out,cap,"扩展分区(LBA)");break;
    case 0x11:snprintf(out,cap,"隐藏 FAT12");break;
    case 0x14:snprintf(out,cap,"隐藏 FAT16 <32M");break;
    case 0x16:snprintf(out,cap,"隐藏 FAT16");break;
    case 0x1B:snprintf(out,cap,"隐藏 FAT32");break;
    case 0x1C:snprintf(out,cap,"隐藏 FAT32(LBA)");break;
    case 0x82:snprintf(out,cap,"Linux swap");break;
    case 0x83:snprintf(out,cap,"Linux");break;
    case 0xEE:snprintf(out,cap,"GPT 保护性 MBR");break;
    case 0xEF:snprintf(out,cap,"EFI 系统分区");break;
    default:  snprintf(out,cap,"0x%02X",t&0xFF);break;
    }
}

/* one MBR primary partition table entry, decoded */
typedef struct {
    int       used;      /* type!=0 and count!=0 */
    unsigned  boot;      /* 0x80 = active/bootable, 0x00 = not */
    unsigned  type;
    unsigned  chs0[3];   /* head/sector/cylinder at start */
    unsigned  chs1[3];
    unsigned long start_lba;   /* little-endian at +8 */
    unsigned long count;       /* little-endian at +12 */
} MbrEnt;

/* decode all four entries from an MBR sector image */
static void mbr_decode(const unsigned char*m,size_t cap,MbrEnt*e,int n)
{
    int i;
    for(i=0;i<n;i++){
        const unsigned char*p=m+0x1BE + 16*i;
        memset(&e[i],0,sizeof e[i]);
        if((size_t)((p+16)-(m))>cap)continue;
        e[i].type=p[4];
        e[i].count=rd32(p+12);
        if(e[i].type==0||e[i].count==0)continue;   /* empty slot */
        e[i].used=1;
        e[i].boot=p[0];
        e[i].chs0[0]=p[1]; e[i].chs0[1]=p[2]&0x3F; e[i].chs0[2]=p[3]|((p[2]&0xC0)<<2);
        e[i].chs1[0]=p[5]; e[i].chs1[1]=p[6]&0x3F; e[i].chs1[2]=p[7]|((p[6]&0xC0)<<2);
        e[i].start_lba=rd32(p+8);
    }
}

/* ------------------------------------------------------------------ */
/* FAT32 geometry / helpers                                            */
/* ------------------------------------------------------------------ */

typedef struct {
    Dev*      dev;
    ULONGLONG part_lba;     /* absolute LBA of the FAT32 partition start  */
    ULONGLONG part_secs;    /* partition length in sectors                */
    unsigned  bps;          /* bytes per sector  (BPB +11)                */
    unsigned  spc;          /* sectors per cluster (BPB +13)              */
    ULONGLONG rsvd;         /* reserved sectors  (BPB +14, u16)           */
    unsigned  nfat;         /* number of FATs   (BPB +16)                 */
    ULONGLONG fatsz;        /* sectors per FAT  (FAT32, BPB +36, u32)     */
    ULONGLONG rootclu;      /* first cluster of root dir (FAT32,+44,u32)  */
    ULONGLONG fat0_lba;     /* absolute LBA of FAT 0                      */
    ULONGLONG datalba;      /* absolute LBA of first data sector          */
    ULONGLONG maxclu;       /* last valid cluster number (approx)         */
} Fat;

/* absolute LBA of a data cluster */
static ULONGLONG clu_lba(const Fat*f,ULONGLONG c)
{return f->datalba+(c-2)*f->spc;}

/* read the FAT entry for cluster c; returns next cluster or 0 on error.
 * FAT is an array of u32 at sector fat0_lba.  A 32-bit entry may straddle a
 * sector boundary, so handle that case explicitly. */
static ULONGLONG fat_next(const Fat*f,ULONGLONG c)
{
    ABuf a={0};
    ULONGLONG byte=c*4;
    ULONGLONG s=f->fat0_lba+byte/(ULONGLONG)f->bps;
    unsigned off=(unsigned)(byte%(ULONGLONG)f->bps);
    unsigned long v;

    if(!abuf_ensure(&a,f->bps,f->bps))return 0;
    if(!rd_sec(f->dev,s,1,a.buf)){abuf_free(&a);return 0;}
    if(off+4<=f->bps){
        v=rd32((unsigned char*)a.buf+off);
        abuf_free(&a);
        return (ULONGLONG)v&0x0FFFFFFF;
    }
    /* crosses into the next FAT sector */
    {
        ABuf nxt={0};
        unsigned char tail[4]={0,0,0,0};
        unsigned i,first=f->bps-off,remain=4-first;
        if(!abuf_ensure(&nxt,f->bps,f->bps)){abuf_free(&a);return 0;}
        if(!rd_sec(f->dev,s+1,1,nxt.buf)){abuf_free(&a);abuf_free(&nxt);return 0;}
        for(i=0;i<remain;i++)tail[i]=((unsigned char*)nxt.buf)[i];
        v=0;
        for(i=0;i<first;i++) v|= (unsigned long)((unsigned char*)a.buf)[off+i]<<(8*i);
        for(i=0;i<remain;i++)v|=(unsigned long)tail[i]<<(8*(first+i));
        abuf_free(&a);abuf_free(&nxt);
        return (ULONGLONG)v&0x0FFFFFFF;
    }
}

/* read one full directory chain (root or subdir) into a heap buffer.
 * Returns malloc'ed buffer (caller frees) and its length, or NULL. */
typedef struct { unsigned char*data; size_t len; } DirBuf;
static int dir_read(const Fat*f,ULONGLONG start,DirBuf*db,int quiet)
{
    size_t cap=0,used=0;
    ULONGLONG c=start;
    int guard=0;

    db->data=NULL;db->len=0;
    for(;;){
        unsigned char *t;
        if(guard++>200000){if(!quiet)printf("  [警告] 目录簇链过长，中止\n");break;}
        if(c<2||c>f->maxclu)break;
        if(c>=0x0FFFFFF8)break;   /* end of chain marker reached */
        if(used+(size_t)f->spc*f->bps>cap){
            cap=used+(size_t)f->spc*f->bps+65536;
            t=(unsigned char*)realloc(db->data,cap);
            if(!t){free(db->data);db->data=NULL;return 0;}
            db->data=t;
        }
        if(!rd_sec(f->dev,clu_lba(f,c),(unsigned)f->spc,db->data+used)){
            if(!quiet)printf("  [警告] 读取簇 %llu 失败\n",c);
            free(db->data);db->data=NULL;return 0;
        }
        used+=(size_t)f->spc*f->bps;
        c=fat_next(f,c);
        if(c==0)break;
    }
    db->len=used;
    return used>0;
}

/* decode a raw 11-byte 8.3 name into "NAME.EXT" (uppercase) */
static void name83(const unsigned char*d,char*out,size_t cap)
{
    char a[9]={0},b[4]={0};
    int i;
    for(i=0;i<8&&d[i]!=' '&&d[i]!='\0';i++)a[i]=(char)d[i];
    a[i]=0;
    for(i=0;i<3&&d[8+i]!=' '&&d[8+i]!='\0';i++)b[i]=(char)d[8+i];
    b[i]=0;
    if(b[0])snprintf(out,cap,"%s.%s",a,b);
    else    snprintf(out,cap,"%s",a);
}

/* case-insensitive ASCII compare of two names */
static int nameeq(const char*a,const char*b)
{
    if(!a||!b)return 0;
    while(*a&&*b){if(my_toupper((unsigned char)*a)!=my_toupper((unsigned char)*b))return 0;a++;b++;}
    return *a==0&&*b==0;
}

/* ------------------------------------------------------------------ */
/* FAT32: is this the tail region?  helper for injection only          */
/* ------------------------------------------------------------------ */

/* compute tail free region of a *physical disk* (or image): bytes that
 * belong to no partition, after the end of the last partition. */
typedef struct { int found; ULONGLONG start,end; } Tail;
static void tail_of(const Dev*d,const MbrEnt*e,int n,Tail*t)
{
    ULONGLONG maxend=0;
    int i;
    memset(t,0,sizeof *t);
    /* MBR metadata occupies sector 0.., but that is before partitions and
     * never part of the tail we report.  Only partition extents matter. */
    for(i=0;i<n;i++){
        if(!e[i].used)continue;
        ULONGLONG en=((ULONGLONG)e[i].start_lba+e[i].count)*d->sector;
        if(en>maxend)maxend=en;
    }
    if(d->total&&maxend<d->total){
        t->found=1;t->start=maxend;t->end=d->total;
    }
}

/* ------------------------------------------------------------------ */
/* UAC auto-relaunch (copied idea from disk_layout.c)                  */
/* ------------------------------------------------------------------ */

static int running_elevated(void)
{
    HANDLE h=CreateFileA("\\\\.\\PhysicalDrive0",GENERIC_READ,
                         FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
                         OPEN_EXISTING,0,NULL);
    if(h!=INVALID_HANDLE_VALUE){CloseHandle(h);return 1;}
    return GetLastError()!=ERROR_ACCESS_DENIED;
}

static void add_arg(char*d,size_t cap,size_t*off,const char*s)
{while(*s&&*off+1<cap)d[(*off)++]=*s++;d[*off]=0;}

static int relaunch_elevated(int argc,char**argv)
{
    typedef HINSTANCE(WINAPI*ShellA)(HWND,LPCSTR,LPCSTR,LPCSTR,LPCSTR,INT);
    HMODULE sh=LoadLibraryA("shell32.dll");
    ShellA runas=NULL; char exe[MAX_PATH],dir[MAX_PATH],args[1024],params[2048];
    size_t off=0; int i; HINSTANCE r;
    if(!sh)return 0;
    runas=(ShellA)(void*)GetProcAddress(sh,"ShellExecuteA");
    if(!runas){FreeLibrary(sh);return 0;}
    GetModuleFileNameA(NULL,exe,sizeof exe);
    GetCurrentDirectoryA(sizeof dir,dir);
    args[0]=0;
    for(i=1;i<argc;i++){if(i>1)add_arg(args,sizeof args,&off," ");add_arg(args,sizeof args,&off,argv[i]);}
    snprintf(params,sizeof params,"/k \"\"%s\" %s\"",exe,args);
    r=runas(NULL,"runas","cmd.exe",params,dir,SW_SHOWNORMAL);
    FreeLibrary(sh);
    return ((INT_PTR)r>32)?1:0;
}

/* ------------------------------------------------------------------ */
/* helpers to lock (then unlock) volumes that live on a disk           */
/* ------------------------------------------------------------------ */

#define MAX_VOL_LOCK 16
static HANDLE g_volh[MAX_VOL_LOCK];
static int    g_nvol;

/* try to FSCTL_LOCK every volume located on this disk number.
 * Returns number of volumes locked (best effort; failures are fine). */
static int lock_disk_volumes(int disknum)
{
    wchar_t root[512];
    HANDLE hf=FindFirstVolumeW(root,512);
    int n=0;
    g_nvol=0;
    if(hf==INVALID_HANDLE_VALUE)return 0;
    do{
        HANDLE hv;
        VOLUME_DISK_EXTENTS vde;
        DWORD ret=0;
        if(!root[0])continue;
        /* open volume device */
        hv=CreateFileW(root,GENERIC_READ|GENERIC_WRITE,
                       FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        if(hv==INVALID_HANDLE_VALUE)continue;
        if(DeviceIoControl(hv,IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,NULL,0,
                           &vde,sizeof vde,&ret,NULL)&&ret>=sizeof vde
           &&vde.NumberOfDiskExtents>=1
           &&(int)vde.Extents[0].DiskNumber==disknum){
            DWORD lr=0;
            if(DeviceIoControl(hv,FSCTL_LOCK_VOLUME,NULL,0,NULL,0,&lr,NULL)&&n<MAX_VOL_LOCK){
                g_volh[n++]=hv;continue;   /* keep handle locked */
            }
        }
        CloseHandle(hv);
    }while(FindNextVolumeW(hf,root,512));
    FindVolumeClose(hf);
    g_nvol=n;
    return n;
}

static void unlock_disk_volumes(void)
{
    int i;
    for(i=0;i<g_nvol;i++){
        DWORD lr=0;
        DeviceIoControl(g_volh[i],FSCTL_UNLOCK_VOLUME,NULL,0,NULL,0,&lr,NULL);
        CloseHandle(g_volh[i]);
    }
    g_nvol=0;
}

/* ------------------------------------------------------------------ */
/* interactive confirmation before a destructive write                 */
/* ------------------------------------------------------------------ */

/* ask the user to type exactly the given number. returns 1 if matches. */
static int confirm_number(const char*prompt,int num)
{
    char buf[64];
    printf("%s 输入 %d 确认，其它任意取消：",prompt,num);
    if(!fgets(buf,sizeof buf,stdin))return 0;
    buf[strcspn(buf,"\r\n")]=0;
    return atoi(buf)==num;
}

/* ------------------------------------------------------------------ */
/* feature 1: list physical disks                                      */
/* ------------------------------------------------------------------ */

static void list_disks(int*out_count)
{
    long n; int absent_run=0,cnt=0;
    for(n=0;n<64;n++){
        char path[32]; Dev d; DWORD err;
        snprintf(path,sizeof path,"\\\\.\\PhysicalDrive%ld",n);
        if(dev_open(&d,(int)n,NULL,0,1)){
            char hb[32],sz[32];
            human(d.total,sz,sizeof sz);
            /* peek MBR signature + type */
            unsigned char m[512];
            int gpt=0,mbrmark=0;
            if(rd_sec(&d,0,1,m)&&m[510]==0x55&&m[511]==0xAA){
                mbrmark=1;
                if(d.total>=1024){unsigned char h1[512];if(rd_sec(&d,1,1,h1)&&memcmp(h1,"EFI PART",8)==0)gpt=1;}
            }
            snprintf(hb,sizeof hb,"%s%s",mbrmark?(gpt?"GPT":"MBR"):"无表",mbrmark?"":"?");
            printf("  [%ld] %-8s  %-12s %s\n",n,sz,hb,d.title);
            cnt++;absent_run=0;dev_close(&d);continue;
        }
        err=GetLastError();
        if(err==ERROR_ACCESS_DENIED){printf("  [%ld] 存在（无管理员权限无法访问）\n",n);absent_run=0;continue;}
        if(++absent_run>=2)break;
    }
    if(out_count)*out_count=cnt;
}

/* ------------------------------------------------------------------ */
/* feature 2: show MBR in Chinese                                      */
/* ------------------------------------------------------------------ */

static void show_mbr(Dev*d)
{
    ABuf a={0}; unsigned char*m=NULL;
    MbrEnt e[4];
    int i,nact=0;

    if(d->total<d->sector){printf("  磁盘太小，无 MBR。\n");return;}
    if(!abuf_ensure(&a,d->sector,d->sector)){printf("  内存不足\n");return;}
    m=(unsigned char*)a.buf;
    if(!rd_sec(d,0,1,m)){printf("  无法读取 0 号扇区。\n");abuf_free(&a);return;}

    printf("\n  == 0 号扇区 (MBR) ==\n");
    if(!(m[510]==0x55&&m[511]==0xAA)){
        printf("  [!] 结束标志不是 55 AA —— 这不是标准 MBR；若 FAT32 则属无分区表类型。\n");
        printf("      引导代码区: %u 字节  分区表区(0x1BE-0x1FD): 64 字节\n",0x1BE);
        abuf_free(&a);return;
    }
    printf("  MBR 签名 0x55AA：正确\n");
    printf("  引导代码区 0x000..0x1BD（%u 字节），分区表区 0x1BE..0x1FD（64 字节）\n",0x1BE);

    mbr_decode(m,d->sector,e,4);

    printf("\n  -- 主分区表（4 项，每项 16 字节，自 0x1BE 起） --\n");
    for(i=0;i<4;i++){
        char tn[40],hb[32];
        const unsigned char*p=m+0x1BE + 16*i;
        printf("\n  [分区 %d] 位置 0x1BE+%d  (0x%03X)\n",i,i*16,0x1BE + i*16);
        printf("    原始 16 字节: ");
        {int k;for(k=0;k<16;k++)printf("%02X ",p[k]);printf("\n");}
        if(!e[i].used){
            printf("    -> 空槽位（类型=0 或扇区数=0）\n");
            continue;
        }
        if(e[i].boot==0x80)snprintf(hb,sizeof hb,"活动/可引导 (0x80)");
        else               snprintf(hb,sizeof hb,"非活动 (0x00)");
        mbr_type_name(e[i].type,tn,sizeof tn);
        {
            char bs0[32],bs1[32],bsz[32];
            ULONGLONG s=(ULONGLONG)e[i].start_lba*d->sector;
            ULONGLONG eof=s+(ULONGLONG)e[i].count*d->sector;
            human(s,bs0,sizeof bs0);
            human(eof,bs1,sizeof bs1);
            human(eof-s,bsz,sizeof bsz);
            printf("    引导标志 : %s\n",hb);
            printf("    起始 CHS : 磁头 %u  扇区 %u  柱面 %u\n",e[i].chs0[0],e[i].chs0[1],e[i].chs0[2]);
            printf("    分区类型 : 0x%02X = %s\n",e[i].type,tn);
            printf("    结束 CHS : 磁头 %u  扇区 %u  柱面 %u\n",e[i].chs1[0],e[i].chs1[1],e[i].chs1[2]);
            printf("    起始 LBA : %lu\n",e[i].start_lba);
            printf("    扇区数   : %lu\n",e[i].count);
            printf("    字节范围 : %s ~ %s  大小 %s\n",bs0,bs1,bsz);
        }
        if(e[i].boot==0x80)nact++;
    }
    printf("\n  活动分区数量：%d\n",nact);
    if(nact==0)
        printf("  -> 说明：这块盘没有“活动分区”。若只当数据盘使用这是正常的；\n"
               "     活动标志(0x80)只在 BIOS 从该盘引导时才有意义，引导扇区才会被读。\n");
    if(d->total>=2*d->sector){
        ABuf hb={0};
        if(abuf_ensure(&hb,d->sector,d->sector)&&rd_sec(d,1,1,hb.buf)
           &&memcmp(hb.buf,"EFI PART",8)==0)
            printf("  -> 检测到 LBA1 有 \"EFI PART\"，这是 GPT 盘（0 号扇区是保护性 MBR）。\n");
        abuf_free(&hb);
    }
    abuf_free(&a);
}

/* ------------------------------------------------------------------ */
/* feature 3: show tail free region                                    */
/* ------------------------------------------------------------------ */

static void show_tail(Dev*d,int do_show)
{
    ABuf a={0}; unsigned char*m=NULL;
    MbrEnt e[4]; Tail t;
    char c1[32],c2[32],c3[32];
    int i;

    if(!abuf_ensure(&a,d->sector,d->sector)){printf("  内存不足\n");return;}
    m=(unsigned char*)a.buf;
    if(!rd_sec(d,0,1,m)){printf("  无法读取 0 号扇区。\n");abuf_free(&a);return;}
    if(!(m[510]==0x55&&m[511]==0xAA)){printf("  不是标准 MBR，无法按分区计算盘尾空闲区。\n");abuf_free(&a);return;}
    mbr_decode(m,d->sector,e,4);
    tail_of(d,e,4,&t);
    abuf_free(&a);

    if(!t.found){printf("  未发现“最后一个分区之后的空闲区”（分区直达盘尾）。\n");return;}

    human(t.end-t.start,c1,sizeof c1);
    human(t.start,c2,sizeof c2);
    human(t.end,c3,sizeof c3);
    printf("\n  == 盘尾空闲区（对齐/未分区，OS 无法识别） ==\n");
    printf("  起始字节 : %s  (%llu 字节)\n",c2,(unsigned long long)t.start);
    printf("  结束字节 : %s  (%llu 字节, 盘末)\n",c3,(unsigned long long)t.end);
    printf("  大小     : %s\n",c1);
    printf("  起始 LBA : %llu  (扇区 %lu 字节)\n",(unsigned long long)(t.start/d->sector),(unsigned long)d->sector);
    printf("  -> 这块空间不属于任何分区；Windows 资源管理器/磁盘管理器看不到它。\n");
    for(i=0;i<4;i++)if(e[i].used&&(ULONGLONG)(e[i].start_lba+e[i].count)*d->sector==t.start)
        printf("     紧跟的分区是第 %d 项。\n",i);
    if(do_show)return;
}

/* ------------------------------------------------------------------ */
/* byte-level read / write at an arbitrary absolute device offset      */
/* (the self-made file system lives at non-sector-aligned places)      */
/* ------------------------------------------------------------------ */

/* read `n` bytes starting at absolute byte offset `absoff`. returns 1 on ok. */
static int rd_bytes(Dev*d,ULONGLONG absoff,unsigned char*buf,size_t n)
{
    unsigned sec=d->sector;
    ULONGLONG s0=absoff/sec;
    size_t off=(size_t)(absoff%sec);
    unsigned long long nsec=((unsigned long long)off+n+sec-1)/sec;
    ABuf a={0}; int ok=0;

    if(n==0)return 1;
    if(!abuf_ensure(&a,(size_t)nsec*(size_t)sec,sec))return 0;
    if(rd_sec(d,s0,(unsigned)nsec,a.buf)){
        memcpy(buf,(unsigned char*)a.buf+off,n);
        ok=1;
    }
    abuf_free(&a);
    return ok;
}

/* read-modify-write `n` bytes at absolute byte offset `absoff`; never
 * touches bytes outside [absoff,absoff+n). returns 1 on ok. */
static int wr_bytes(Dev*d,ULONGLONG absoff,const unsigned char*buf,size_t n)
{
    unsigned sec=d->sector;
    ULONGLONG s0=absoff/sec;
    size_t off=(size_t)(absoff%sec);
    unsigned long long nsec=((unsigned long long)off+n+sec-1)/sec;
    ABuf a={0}; int ok=0;

    if(n==0)return 1;
    if(!abuf_ensure(&a,(size_t)nsec*(size_t)sec,sec))return 0;
    if(!rd_sec(d,s0,(unsigned)nsec,a.buf)){abuf_free(&a);return 0;}
    memcpy((unsigned char*)a.buf+off,buf,n);
    ok=wr_sec(d,s0,(unsigned)nsec,a.buf);
    abuf_free(&a);
    return ok;
}

/* ------------------------------------------------------------------ */
/* self-made hidden file system over the tail free region              */
/* ------------------------------------------------------------------ */
/*
 * Layout (base = byte offset of the tail-region start):
 *
 *   base + 0x00 .. base + 0xBD   directory: 10 entries x 19 bytes
 *   base + 0xBE .. base + 0xBF   terminator 0x55 0xAA
 *   base + 0xC0 ..               file data, appended in order
 *
 * One 19-byte directory entry (8.3 short-name style):
 *   [0]..[7]    file name   (8 ASCII bytes, zero-padded)
 *   [8]..[10]   extension   (3 ASCII bytes, zero-padded)
 *   [11]..[14]  start offset, u32 LE, counted from the file-data origin
 *   [15]..[18]  end   offset, u32 LE, INCLUSIVE  (size = end-start+1)
 *
 * File-data origin = base + 0xC0 (right after the 0xAA), so the first stored
 * file is at offset 0.  An entry whose 19 bytes are all zero is free; the ten
 * slots are scanned independently, so deleting one never hides the others.
 * The 32-bit offsets bound the data area only by the tail region's real size
 * (checked at add time).  The "defrag" step repacks files so that they abut
 * head-to-tail with no dead gaps left by deletions.
 */
#define FS_DIR_N   10
#define FS_NL       8                 /* 8.3 main-name bytes  */
#define FS_XL       3                 /* 8.3 extension bytes  */
#define FS_ESZ     19                 /* name 8 + ext 3 + u32 start + u32 end */
#define FS_DIR_SZ  (FS_DIR_N*FS_ESZ)  /* 190 (0xBE) */
#define FS_MARK    FS_DIR_SZ          /* 0xBE : the 0x55AA sits here */
#define FS_DATA    (FS_DIR_SZ+2)      /* 0xC0 : file-data origin */
#define FS_HDR     (FS_DIR_SZ+2)      /* total header bytes = 192 */
#define FS_OSTART  (FS_NL+FS_XL)      /* 11 : u32 start  in an entry */
#define FS_OEND    (FS_NL+FS_XL+4)    /* 15 : u32 end     in an entry */

/* little-endian 32-bit store */
static void put32(unsigned char*p,unsigned long v)
{p[0]=(unsigned char)(v&255);p[1]=(unsigned char)((v>>8)&255);
 p[2]=(unsigned char)((v>>16)&255);p[3]=(unsigned char)((v>>24)&255);}

/* read the FS_HDR-byte directory block. returns 1 and fills dir[] when the
 * 0x55AA marker is present, else 0 and dir[] is cleared. */
static int fs_read_dir(Dev*d,ULONGLONG base,unsigned char dir[FS_HDR])
{
    unsigned char z[FS_HDR];
    memset(z,0,sizeof z);
    if(!rd_bytes(d,base,dir,FS_HDR)){memcpy(dir,z,sizeof z);return 0;}
    if(dir[FS_MARK]!=0x55||dir[FS_MARK+1]!=0xAA){
        memcpy(dir,z,sizeof z);return 0;
    }
    return 1;
}

static int fs_write_dir(Dev*d,ULONGLONG base,const unsigned char dir[FS_HDR])
{return wr_bytes(d,base,dir,FS_HDR);}

static int fs_init_dir(Dev*d,ULONGLONG base)
{
    unsigned char dir[FS_HDR];
    memset(dir,0,sizeof dir);
    dir[FS_MARK]=0x55; dir[FS_MARK+1]=0xAA;
    return wr_bytes(d,base,dir,FS_HDR);
}

static int fs_entry_free(const unsigned char e[FS_ESZ])
{int i;for(i=0;i<FS_ESZ;i++)if(e[i])return 0;return 1;}

/* format one entry's stored 8.3 fields as a printable "NAME.EXT" string
 * (zero padding trimmed, source case kept). */
static void fs_entry_name(const unsigned char*e,char*out,size_t cap)
{
    char nm[FS_NL+1],ex[FS_XL+1];
    int i,n=0,x=0;
    for(i=0;i<FS_NL&&e[i];i++)nm[n++]=e[i];
    nm[n]=0;
    for(i=0;i<FS_XL&&e[FS_NL+i];i++)ex[x++]=e[FS_NL+i];
    ex[x]=0;
    if(x)snprintf(out,cap,"%s.%s",nm,ex);
    else snprintf(out,cap,"%s",nm);
}

/* split a path's last component into an 8.3 name (<=8 bytes) and extension
 * (<=3 bytes); names are stored zero-padded, original case preserved.
 * Returns 1 when the file fits 8.3, else 0 (rejects, prints the reason). */
static int fs_split_name(const char*path,unsigned char nm[FS_NL],unsigned char ex[FS_XL])
{
    const char*s=path,*p,*dot=NULL;
    size_t stem,el;
    memset(nm,0,FS_NL);memset(ex,0,FS_XL);
    for(p=path;*p;p++)if(*p=='/'||*p=='\\')s=p+1;
    for(p=s;*p;p++)if(*p=='.')dot=p;
    stem=dot?(size_t)(dot-s):strlen(s);
    el=dot?strlen(dot+1):0;
    if(stem==0){
        printf("  [拒绝] 文件没有主名（如 .txt），无法登记。\n");return 0;
    }
    if(stem>FS_NL){
        printf("  [拒绝] 主名 %u 字节超过 8.3 上限 %d 字节，无法登记。\n",
               (unsigned)stem,FS_NL);return 0;
    }
    if(el>FS_XL){
        printf("  [拒绝] 扩展名 %u 字节超过 8.3 上限 %d 字节，无法登记。\n",
               (unsigned)el,FS_XL);return 0;
    }
    memcpy(nm,s,stem);
    if(dot)memcpy(ex,dot+1,el);
    return 1;
}

/* list the ten directory entries */
static void fs_list(Dev*d,ULONGLONG base)
{
    unsigned char dir[FS_HDR]; int i,n=0;
    if(!fs_read_dir(d,base,dir)){
        printf("  该区域还没有自制文件系统（base %llu 处没有 0x55AA 结尾标志）。\n"
               "  先用“添加文件”会自动初始化目录区。\n",(unsigned long long)base);
        return;
    }
    printf("  目录区：base %llu .. %llu，10 项 x %dB，以 0x55AA 结尾\n",
           (unsigned long long)base,(unsigned long long)(base+FS_MARK+1),FS_ESZ);
    for(i=0;i<FS_DIR_N;i++){
        const unsigned char*e=dir+i*FS_ESZ;
        char fn[FS_NL+FS_XL+2],rng[32],sz[32];
        if(fs_entry_free(e))continue;
        fs_entry_name(e,fn,sizeof fn);
        {unsigned long long s=rd32(e+FS_OSTART),f=rd32(e+FS_OEND);
        unsigned long long l=s<=f?(f-s+1):0;
        snprintf(rng,sizeof rng,"%llu..%llu",s,f);
        human(l,sz,sizeof sz);
        printf("  [%d]  %-12s  %-18s %-10s\n",i,fn,rng,sz);}
        n++;
    }
    printf(n?"  共 %d 个文件。\n":"  目录为空。\n",n);
}

/* append one local file (any mounted disk) into the file system */
static void fs_add(Dev*d,ULONGLONG base,ULONGLONG span)
{
    unsigned char dir[FS_HDR]; unsigned char*file=NULL;
    unsigned long flen=0,ferr=0;
    unsigned char nm[FS_NL],ex[FS_XL],e[FS_ESZ];
    char path[1024];
    char fname[FS_NL+FS_XL+2];
    unsigned long long cur,need;
    int i,k,init=0;

    printf("  源文件完整路径（可在其它盘/分区，如 D:\\xx\\a.txt；直接回车取消）：");
    if(!fgets(path,sizeof path,stdin))return;
    path[strcspn(path,"\r\n")]=0;
    if(!path[0])return;

    file=read_whole_file(path,&flen,&ferr);
    if(!file){printf("  无法读取文件（err %lu）。\n",ferr);return;}
    if(flen==0){printf("  空文件，不能添加。\n");free(file);return;}
    if(!fs_split_name(path,nm,ex)){free(file);return;}

    memset(e,0,sizeof e);
    memcpy(e,nm,FS_NL);memcpy(e+FS_NL,ex,FS_XL);
    fs_entry_name(e,fname,sizeof fname);

    if(!fs_read_dir(d,base,dir))init=1;
    /* reject a duplicate name (case-insensitive) so that exporting several
     * stored files into one folder can never overwrite one with another */
    for(k=0;k<FS_DIR_N;k++){
        const unsigned char*q=dir+k*FS_ESZ;
        char fn2[FS_NL+FS_XL+2];
        if(fs_entry_free(q))continue;
        fs_entry_name(q,fn2,sizeof fn2);
        if(nameeq(fname,fn2)){
            printf("  目录里已有同名文件 %s，拒绝添加（避免导出时互相覆盖）。\n",fn2);
            free(file);return;
        }
    }
    for(i=0;i<FS_DIR_N;i++)if(fs_entry_free(dir+i*FS_ESZ))break;
    if(i>=FS_DIR_N){
        printf("  目录区 10 项已满，无法添加。请先删除或整理。\n");free(file);return;
    }

    printf("  文件 %lu 字节，将以名称 %s 登记。\n",flen,fname);

    /* append right after the last stored byte of every existing file */
    cur=0;
    for(k=0;k<FS_DIR_N;k++){const unsigned char*q=dir+k*FS_ESZ;
        if(!fs_entry_free(q)){unsigned long long en=rd32(q+FS_OEND);if(en+1>cur)cur=en+1;}}
    need=cur+flen;
    if(need-1>0xFFFFFFFFULL){
        printf("  文件过大：位置字段仅 4 字节，无法寻址到偏移 %llu。\n",
               (unsigned long long)(need-1));
        free(file);return;
    }
    if((unsigned long long)FS_DATA+need>span){
        printf("  盘尾空闲区空间不足：需要 %llu 字节，该区域只有 %llu 字节。\n",
               (unsigned long long)(FS_DATA+need),(unsigned long long)span);
        printf("  可先用“整理”消除已删除文件留下的空隙。\n");
        free(file);return;
    }

    if(init){
        printf("  尚未有自制文件系统，初始化目录区（%d 字节清零 + 0x55AA）……\n",FS_HDR);
        if(!fs_init_dir(d,base)){printf("  写入目录区失败。\n");free(file);return;}
        memset(dir,0,sizeof dir);dir[FS_MARK]=0x55;dir[FS_MARK+1]=0xAA;
    }

    /* file bytes first, then the directory slot */
    if(!wr_bytes(d,base+FS_DATA+cur,file,flen)){
        printf("  写入文件数据失败。\n");free(file);return;
    }
    /* verify */
    {
        unsigned char*chk=(unsigned char*)malloc(flen);
        if(chk){
            if(rd_bytes(d,base+FS_DATA+cur,chk,flen)&&memcmp(chk,file,flen)!=0)
                printf("  [!] 回读校验不一致。\n");
            free(chk);
        }
    }

    put32(e+FS_OSTART,(unsigned long)(cur&0xFFFFFFFFUL));
    put32(e+FS_OEND,(unsigned long)((cur+flen-1)&0xFFFFFFFFUL));
    memcpy(dir+i*FS_ESZ,e,FS_ESZ);
    if(!fs_write_dir(d,base,dir)){printf("  写入目录区失败。\n");free(file);return;}

    printf("  已添加 %s -> 目录项[%d]，数据区偏移 %llu..%llu（相对数据区起点）。\n",
           fname,i,(unsigned long long)cur,(unsigned long long)(cur+flen-1));
    printf("  该文件已登记并写盘，重开程序/退出后依然存在。\n");
    free(file);
}

/* copy one stored file back out to a chosen folder on any mounted disk,
 * keeping its stored 8.3 name.  read-only on the hidden region. */
static void fs_export(Dev*d,ULONGLONG base,ULONGLONG span,int idx)
{
    unsigned char dir[FS_HDR]; const unsigned char*e;
    unsigned long long s,f,len;
    unsigned char*buf;
    char fname[FS_NL+FS_XL+2],dirp[1024],full[1100],hs[32];
    size_t dl;
    unsigned long werr=0;

    if(idx<0||idx>=FS_DIR_N){printf("  序号需在 0..9 之间。\n");return;}
    if(!fs_read_dir(d,base,dir)){printf("  没有自制文件系统。\n");return;}
    e=dir+idx*FS_ESZ;
    if(fs_entry_free(e)){printf("  目录项[%d] 为空。\n",idx);return;}
    s=rd32(e+FS_OSTART); f=rd32(e+FS_OEND);
    if(f<s||(unsigned long long)FS_DATA+f+1>span){
        printf("  目录项[%d] 偏移异常（%llu..%llu），拒绝导出。\n",idx,s,f);return;
    }
    len=f-s+1;
    if(len>0xFFFFFFFFUL){printf("  文件超过 4GB，无法用本工具导出。\n");return;}
    fs_entry_name(e,fname,sizeof fname);
    human(len,hs,sizeof hs);
    printf("  目录项[%d] %s：数据区 %llu..%llu（%s）。\n",idx,fname,s,f,hs);

    printf("  导出到哪个目录（如 C:\\Users\\chenj\\Desktop\\work；回车取消）：");
    if(!fgets(dirp,sizeof dirp,stdin))return;
    dirp[strcspn(dirp,"\r\n")]=0;
    if(!dirp[0])return;
    if(!is_directory(dirp)){
        printf("  目录不存在：%s\n",dirp);return;
    }
    /* build "<dir>\<name>" (add a separator unless dir already ends with one) */
    dl=strlen(dirp);
    if(dl>0&&dirp[dl-1]!='\\'&&dirp[dl-1]!='/')
        snprintf(full,sizeof full,"%s\\%s",dirp,fname);
    else
        snprintf(full,sizeof full,"%s%s",dirp,fname);

    buf=(unsigned char*)malloc((size_t)len);
    if(!buf){printf("  内存不足\n");return;}
    if(!rd_bytes(d,base+FS_DATA+s,buf,(size_t)len)){
        free(buf);printf("  从盘上读取数据失败。\n");return;
    }
    if(file_exists(full))printf("  [提示] %s 已存在，将被覆盖。\n",full);
    if(!write_whole_file(full,buf,(unsigned long)len,&werr)){
        printf("  写文件失败（err %lu）：%s\n",werr,full);free(buf);return;
    }
    free(buf);
    printf("  已导出 %s（%s）到 %s；隐藏区内文件仍保留（导出不会删除）。\n",
           fname,hs,full);
}

/* clear one directory slot (data bytes stay but become unreachable) */
static void fs_delete(Dev*d,ULONGLONG base,int idx)
{
    unsigned char dir[FS_HDR];
    if(idx<0||idx>=FS_DIR_N){printf("  序号需在 0..9 之间。\n");return;}
    if(!fs_read_dir(d,base,dir)){printf("  没有自制文件系统。\n");return;}
    if(fs_entry_free(dir+idx*FS_ESZ)){printf("  目录项[%d] 本来为空。\n",idx);return;}
    memset(dir+idx*FS_ESZ,0,FS_ESZ);
    if(!fs_write_dir(d,base,dir)){printf("  写入目录区失败。\n");return;}
    printf("  已删除目录项[%d]。其数据字节仍留在区内但不再被引用；可用“整理”回收空隙。\n",idx);
}

/* defrag: repack all stored files head-to-tail so no dead gap remains */
static void fs_defrag(Dev*d,ULONGLONG base,ULONGLONG span)
{
    unsigned char dir[FS_HDR];
    unsigned char*data[FS_DIR_N];
    int idx[FS_DIR_N]; int n=0,i,k;
    unsigned long long len[FS_DIR_N];
    unsigned long long total=0,high=0,off=0;

    if(!fs_read_dir(d,base,dir)){printf("  没有自制文件系统。\n");return;}
    for(i=0;i<FS_DIR_N;i++)data[i]=NULL;
    /* read every used file out of the region first, so repacking may safely
     * overwrite their old bytes */
    for(i=0;i<FS_DIR_N;i++){
        const unsigned char*e=dir+i*FS_ESZ;
        unsigned long long s,f,l;
        if(fs_entry_free(e))continue;
        s=rd32(e+FS_OSTART); f=rd32(e+FS_OEND);
        if(f<s||(unsigned long long)FS_DATA+f+1>span){
            printf("  目录项[%d] 偏移异常（%llu..%llu），中止整理。\n",i,s,f);
            goto fail;
        }
        l=f-s+1;
        data[i]=(unsigned char*)malloc((size_t)l);
        if(!data[i]){printf("  内存不足\n");goto fail;}
        if(!rd_bytes(d,base+FS_DATA+s,data[i],(size_t)l)){
            printf("  读取目录项[%d] 数据失败。\n",i);goto fail;
        }
        idx[n]=i; len[n]=l; total+=l; n++;
        if(f+1>high)high=f+1;
    }
    if(n==0){printf("  目录为空，无需整理。\n");return;}
    if((unsigned long long)FS_DATA+total>span){
        printf("  整理后的数据量超出空闲区，无法整理。\n");goto fail;
    }
    for(i=0;i<n;i++){
        k=idx[i];
        if(!wr_bytes(d,base+FS_DATA+off,data[k],(size_t)len[i])){
            printf("  写入数据失败（目录项[%d]）。\n",k);goto fail;
        }
        {unsigned char*en=dir+k*FS_ESZ;
        put32(en+FS_OSTART,(unsigned long)(off&0xFFFFFFFFUL));
        put32(en+FS_OEND,(unsigned long)((off+len[i]-1)&0xFFFFFFFFUL));}
        off+=len[i];
    }
    if(!fs_write_dir(d,base,dir)){printf("  写目录区失败。\n");goto fail;}
    printf("  整理完成：%d 个文件已首尾相接。\n",n);
    if(high>total)
        printf("  已释放空隙 %llu 字节（原占用到 %llu，现仅 %llu）。\n",
               high-total,high,total);
    else
        printf("  数据本就连续，无空隙可释放。\n");
fail:
    for(i=0;i<FS_DIR_N;i++)if(data[i])free(data[i]);
}

/* interactive manager for the self-made hidden file system */
static void custom_fs_menu(Dev*d,int disknum)
{
    ABuf a={0}; unsigned char*m=NULL;
    MbrEnt e[4]; Tail t;
    ULONGLONG base,span;
    char c1[32],c2[32];
    int loop=1;

    if(!abuf_ensure(&a,d->sector,d->sector)){printf("  内存不足\n");return;}
    m=(unsigned char*)a.buf;
    if(!rd_sec(d,0,1,m)||!(m[510]==0x55&&m[511]==0xAA)){
        printf("  无标准 MBR，无法确定盘尾空闲区。\n");abuf_free(&a);return;
    }
    mbr_decode(m,d->sector,e,4);
    tail_of(d,e,4,&t);
    abuf_free(&a);
    if(!t.found){printf("  盘尾没有空闲区。\n");return;}
    base=t.start; span=t.end-t.start;
    if(span<FS_HDR){
        printf("  盘尾空闲区仅 %llu 字节，不够放 %d 字节的目录区。\n",
               (unsigned long long)span,FS_HDR);
        return;
    }

    printf("\n  == 自制文件系统管理 ==\n");
    human(base,c1,sizeof c1);
    human(span-FS_DATA,c2,sizeof c2);
    printf("  隐藏区起点：字节 %llu（%s，LBA %llu）；数据区可用 %s（物理上限，位置字段为 4 字节）\n",
           (unsigned long long)base,c1,(unsigned long long)(base/d->sector),c2);

    if(!d->is_image){lock_disk_volumes(disknum);
        if(g_nvol==0)printf("  [注意] 未锁定卷，写入可能被 Windows 缓存覆盖，请先卸载/脱机该盘。\n");}

    while(loop){
        printf("\n  [自制文件系统] 1)列出  2)添加文件  3)导出/移出  4)删除  5)整理  0)返回主菜单  请选择：");
        fflush(stdout);
        {
            char b[32];int v;
            if(!fgets(b,sizeof b,stdin))break;
            v=atoi(b);
            switch(v){
            case 1: fs_list(d,base); break;
            case 2:
                if(!confirm_number("  确认写盘（不可逆）",disknum)){printf("  已取消。\n");break;}
                fs_add(d,base,span);
                break;
            case 3:
                {char bi[32];int ix;
                 printf("  要导出的目录项序号 [0..9]：");
                 if(!fgets(bi,sizeof bi,stdin))break;
                 ix=atoi(bi);
                 fs_export(d,base,span,ix);}
                break;
            case 4:
                {char bi[32];int ix;
                 printf("  要删除的目录项序号 [0..9]：");
                 if(!fgets(bi,sizeof bi,stdin))break;
                 ix=atoi(bi);
                 if(!confirm_number("  确认写盘（不可逆）",disknum)){printf("  已取消。\n");break;}
                 fs_delete(d,base,ix);}
                break;
            case 5:
                if(!confirm_number("  确认写盘（不可逆）",disknum)){printf("  已取消。\n");break;}
                fs_defrag(d,base,span);
                break;
            case 0: loop=0; break;
            default: printf("  无效选项。\n"); break;
            }
        }
    }
    if(!d->is_image)unlock_disk_volumes();
}

/* ------------------------------------------------------------------ */
/* feature 5: FAT32 directory-entry first-cluster rewrite              */
/* ------------------------------------------------------------------ */

typedef struct { int    nfiles; int ndirs; int nskip; } WalkStat;

/* recursively scan a directory chain and rewrite every 8.3 entry's first
 * cluster to `target` (unless it is "." ".." deleted/LFN/volume label).
 * When dry==1, no write is done; only statistics are returned. */
static void walk_rewrite(const Fat*f,ULONGLONG start,ULONGLONG target,
                         int dry,WalkStat*st,int depth)
{
    DirBuf db={0};
    size_t i;
    size_t clusz=(size_t)f->spc*f->bps;
    size_t nclu;
    int changed=0;

    if(depth>200)return;
    if(!dir_read(f,start,&db,1))return;
    if(db.len==0){free(db.data);return;}
    nclu=(db.len+clusz-1)/clusz;

    /* Pass 1: find sub-directory first-cluster values (original) BEFORE we
     * overwrite them, and recurse first so the tree stays intact on disk
     * while we still hold the original pointers. */
    for(i=0;i+32<=db.len;i+=32){
        const unsigned char*dn=db.data+i;
        if(dn[0]==0x00)break;              /* clean end of directory */
        if(dn[0]==0xE5)continue;           /* deleted */
        {
            unsigned attr=dn[11];
            if(attr==0x0F)continue;        /* LFN entry - never touch */
            if(attr&0x08)continue;         /* volume label or reserved */
            if(dn[0]==0x2E&&(dn[1]==0x20||dn[1]==0x00||dn[1]==0x2E))continue; /* . .. */
            if(attr&0x10){                 /* sub-directory entry */
                ULONGLONG c=((ULONGLONG)rd16(dn+0x14)<<16)|rd16(dn+0x1A);
                if(c>=2&&c<=f->maxclu){
                    st->ndirs++;
                    walk_rewrite(f,c,target,dry,st,depth+1);
                } else st->nskip++;
            } else st->nfiles++;
        }
    }

    /* Pass 2: apply the cluster rewrite on all remaining entries. */
    if(!dry){
        for(i=0;i+32<=db.len;i+=32){
            unsigned char*dn=db.data+i;
            if(dn[0]==0x00)break;
            if(dn[0]==0xE5)continue;
            {
                unsigned attr=dn[11];
                if(attr==0x0F)continue;
                if(attr&0x08)continue;
                if(dn[0]==0x2E&&(dn[1]==0x20||dn[1]==0x00||dn[1]==0x2E))continue;
                wr16(dn+0x14,(unsigned)((target>>16)&0xFFFF));
                wr16(dn+0x1A,(unsigned)(target&0xFFFF));
                changed=1;
            }
        }
        if(changed){
            unsigned long cl;size_t k;
            ULONGLONG c=start;
            for(k=0;k<nclu&&c>=2&&c<=f->maxclu;k++){
                cl=(unsigned long)(db.len>(k+1)*clusz?clusz:(db.len-k*clusz));
                if(!wr_sec(f->dev,clu_lba(f,c),(unsigned)((cl+f->bps-1)/f->bps),
                           db.data+k*clusz)){
                    printf("  [警告] 写入目录簇 %llu 失败\n",c);break;
                }
                c=fat_next(f,c);
                if(c<2)break;
            }
        }
    }
    free(db.data);
}

/* find a file/dir by a volume path like "dir\\file.txt" or "\\a\\b".
 * Walks 8.3 names from root.  returns first cluster, or 0 if not found. */
static ULONGLONG find_path(const Fat*f,const char*path)
{
    char tmp[1024]; char comps[128][128]; int ncomp=0;
    const char*p; ULONGLONG cur=f->rootclu;
    int ci=0;

    snprintf(tmp,sizeof tmp,"%s",path);
    /* split */
    p=tmp;
    while(*p){
        char*buf=comps[ncomp];
        int j=0;
        while(*p=='\\'||*p=='/')p++;
        while(*p&&*p!='\\'&&*p!='/'&&j<127)buf[j++]=*p++;
        if(j>0){buf[j]=0;ncomp++;}
        if(ncomp>=128)break;
    }
    if(ncomp==0)return 0;
    if(ncomp>1){
        /* resolve parents first */
        for(ci=0;ci<ncomp-1;ci++){
            DirBuf db={0};size_t i;int hit=0;ULONGLONG next=0;
            if(!dir_read(f,cur,&db,1))return 0;
            for(i=0;i+32<=db.len;i+=32){
                const unsigned char*dn=db.data+i;char nm[16];
                unsigned attr;
                if(dn[0]==0x00||dn[0]==0xE5)continue;
                attr=dn[11];
                if(attr==0x0F||(attr&0x08))continue;
                if(!(attr&0x10))continue;
                if(dn[0]==0x2E&&(dn[1]==0x20||dn[1]==0x00||dn[1]==0x2E))continue;
                name83(dn,nm,sizeof nm);
                if(nameeq(nm,comps[ci])){
                    next=((ULONGLONG)rd16(dn+0x14)<<16)|rd16(dn+0x1A);
                    hit=1;break;
                }
            }
            free(db.data);
            if(!hit)return 0;
            cur=next;
        }
    }
    /* last component can be a file or dir */
    {
        DirBuf db={0};size_t i;ULONGLONG found=0;
        if(!dir_read(f,cur,&db,1))return 0;
        for(i=0;i+32<=db.len;i+=32){
            const unsigned char*dn=db.data+i;char nm[16];
            unsigned attr;
            if(dn[0]==0x00||dn[0]==0xE5)continue;
            attr=dn[11];
            if(attr==0x0F||(attr&0x08))continue;
            if(dn[0]==0x2E&&(dn[1]==0x20||dn[1]==0x00||dn[1]==0x2E))continue;
            name83(dn,nm,sizeof nm);
            if(nameeq(nm,comps[ncomp-1])){
                found=((ULONGLONG)rd16(dn+0x14)<<16)|rd16(dn+0x1A);
                break;
            }
        }
        free(db.data);
        if(found<2)return 0;
        return found;
    }
}

/* try to parse a numeric cluster from a string */
static int parse_clu(const char*s,ULONGLONG*out)
{
    char*end;ULONGLONG v;
    if(!s||!*s)return 0;
    v=strtoull(s,&end,0);
    if(*end)return 0;
    *out=v;return 1;
}

static int fat32_open_part(Dev*d,int idx,Fat*f,char*err,size_t errcap)
{
    ABuf a={0},b0={0}; unsigned char*m=NULL,*bs=NULL; MbrEnt e[4];
    ULONGLONG plba=0,psecs=0;
    int k,found=-1,scan=0;
    char sig[9];

    err[0]=0;
    if(!abuf_ensure(&a,d->sector,d->sector)){snprintf(err,errcap,"内存不足");return 0;}
    m=(unsigned char*)a.buf;
    if(!rd_sec(d,0,1,m)||!(m[510]==0x55&&m[511]==0xAA)){
        snprintf(err,errcap,"无标准 MBR");abuf_free(&a);return 0;
    }
    mbr_decode(m,d->sector,e,4);
    abuf_free(&a);

    for(k=0;k<4;k++){
        if(!e[k].used)continue;
        if(e[k].type==0x0B||e[k].type==0x0C||e[k].type==0x1B||e[k].type==0x1C){
            if(scan==idx){found=k;break;}
            scan++;
        }
    }
    if(found<0){
        if(idx==0)snprintf(err,errcap,"未找到 FAT32 类型(0x0B/0x0C)分区");
        else snprintf(err,errcap,"FAT32 分区不足 %d 个",idx+1);
        return 0;
    }
    plba=e[found].start_lba; psecs=e[found].count;

    /* read the boot sector into a device-sector-sized aligned buffer (safe
     * even on 4Kn media where the logical sector is >512 bytes) */
    if(!abuf_ensure(&b0,d->sector,d->sector)){
        snprintf(err,errcap,"内存不足");return 0;
    }
    bs=(unsigned char*)b0.buf;
    if(!rd_sec(d,plba,1,bs)){
        snprintf(err,errcap,"无法读取 FAT32 引导扇区");abuf_free(&b0);return 0;
    }
    if(!(bs[510]==0x55&&bs[511]==0xAA)){
        snprintf(err,errcap,"分区 %d 引导扇区无 55AA 签名",found);abuf_free(&b0);return 0;
    }
    memset(sig,0,sizeof sig);memcpy(sig,bs+0x52,8);
    if(memcmp(sig,"FAT32",5)!=0){
        /* some formatters keep it blank; warn only */
        snprintf(err,errcap,"分区 %d 无 \"FAT32\" 标志（可能非 FAT32）",found);
        abuf_free(&b0);return 0;
    }

    memset(f,0,sizeof *f);
    f->dev=d; f->part_lba=plba; f->part_secs=psecs;
    f->bps=rd16(bs+11); if(f->bps<512)f->bps=512;
    f->spc=bs[13]; if(f->spc==0)f->spc=1;
    f->rsvd=rd16(bs+14);
    f->nfat=bs[16];
    f->fatsz=rd32(bs+36);          /* FAT32: 4-byte sectors/FAT */
    f->rootclu=rd32(bs+44);        /* FAT32: root first cluster */
    f->fat0_lba=plba+f->rsvd;
    f->datalba=plba+f->rsvd+(ULONGLONG)f->nfat*f->fatsz;
    /* max valid cluster: derived from partition length */
    if(f->rsvd+(ULONGLONG)f->nfat*f->fatsz < psecs){
        ULONGLONG data=psecs-(f->rsvd+(ULONGLONG)f->nfat*f->fatsz);
        f->maxclu=2+data/f->spc;
    } else f->maxclu=2;

    printf("  FAT32 分区：第 %d 项 0x0%02X，起始 LBA %llu，大小 %llu 扇区\n",
           found,e[found].type,(unsigned long long)plba,(unsigned long long)psecs);
    printf("  每扇区 %u B，每簇 %u 扇区，保留 %llu，FAT 数 %u，每 FAT %llu 扇区，根簇 %llu\n",
           f->bps,f->spc,(unsigned long long)f->rsvd,f->nfat,
           (unsigned long long)f->fatsz,(unsigned long long)f->rootclu);
    abuf_free(&b0);
    return 1;
}

static void fat32_rewrite_menu(Dev*d,int disknum)
{
    Fat fat={0}; char err[160];
    char input[1024];
    ULONGLONG target=0;
    WalkStat st;
    int imgmode=0;

    printf("\n  == FAT32 目录项起始簇改写 ==\n");
    if(!fat32_open_part(d,0,&fat,err,sizeof err)){
        printf("  %s\n",err);return;
    }

    printf("  目标文件：输入卷内路径（如 \\\\dir\\\\file.txt 或 target.txt），\n"
           "  或直接输入一个起始簇号（形如 5）。回车取消：");
    if(!fgets(input,sizeof input,stdin))return;
    input[strcspn(input,"\r\n")]=0;
    if(!input[0])return;

    if(parse_clu(input,&target)){
        if(target<2||target>fat.maxclu){
            printf("  簇号 %llu 越界（有效 2..%llu）。\n",
                   (unsigned long long)target,(unsigned long long)fat.maxclu);
            return;
        }
        printf("  使用裸簇号 %llu。\n",(unsigned long long)target);
    } else {
        target=find_path(&fat,input);
        if(target<2){
            printf("  卷内找不到 \"%s\"。请检查路径/大小写，或用裸簇号。\n",input);
            return;
        }
        printf("  找到文件，起始簇 = %llu\n",(unsigned long long)target);
    }

    if(!d->is_image){
        /* physical disk: try to lock the volumes on this disk first */
        lock_disk_volumes(disknum);
        if(g_nvol>0)printf("  已锁定该盘上 %d 个卷，避免文件系统缓存干扰。\n",g_nvol);
        else printf("  [注意] 未锁定卷；若目标分区被挂载，Windows 可能用缓存覆盖你的修改。\n");
    } else imgmode=1;

    /* dry run */
    memset(&st,0,sizeof st);
    walk_rewrite(&fat,fat.rootclu,target,1,&st,0);
    printf("\n  [预演] 将把下列起始簇改为 %llu：\n",(unsigned long long)target);
    printf("    普通文件目录项 : %d 个\n",st.nfiles);
    printf("    子目录目录项   : %d 个\n",st.ndirs);
    printf("    跳过(保留)     : %d 个 (LFN/卷标/.\"..\" /越界)\n",st.nskip);
    if(st.nfiles+st.ndirs==0){printf("  数据区没有任何可改的目录项。\n");goto unlock;}

    printf("\n  该操作会同时改写子目录项 —— 目录结构将被破坏（演示“指向同一文件”）。\n");
    if(!confirm_number("  确认执行写盘（不可逆）",disknum)){
        printf("  已取消。\n");goto unlock;
    }
    memset(&st,0,sizeof st);
    walk_rewrite(&fat,fat.rootclu,target,0,&st,0);
    printf("  改写完成：文件 %d 项、目录 %d 项已改为起始簇 %llu。\n",
           st.nfiles,st.ndirs,(unsigned long long)target);
    printf("  -> 现在资源管理器里这些条目读取的簇链都从 %llu 开始，内容会指向该文件。\n",
           (unsigned long long)target);
    if(!imgmode)printf("  请刷新/重插设备查看；本程序已尽力锁定卷。\n");
unlock:
    if(!d->is_image)unlock_disk_volumes();
}

/* ------------------------------------------------------------------ */
/* shrink the last FAT32 partition (grow the tail free region)         */
/* ------------------------------------------------------------------ */

/* parse FAT32 geometry for a partition at absolute LBA `plba`. */
static int part_fat_geom(Dev*d,ULONGLONG plba,unsigned long psecs,Fat*f)
{
    ABuf b0={0}; unsigned char*bs=NULL;
    if(!abuf_ensure(&b0,d->sector,d->sector))return 0;
    bs=(unsigned char*)b0.buf;
    if(!rd_sec(d,plba,1,bs)){abuf_free(&b0);return 0;}
    if(!(bs[510]==0x55&&bs[511]==0xAA)){abuf_free(&b0);return 0;}
    memset(f,0,sizeof *f);
    f->dev=d; f->part_lba=plba; f->part_secs=psecs;
    f->bps=rd16(bs+11); if(f->bps<512)f->bps=512;
    f->spc=bs[13]; if(f->spc==0)f->spc=1;
    f->rsvd=rd16(bs+14);
    f->nfat=bs[16];
    f->fatsz=rd32(bs+36);
    f->rootclu=rd32(bs+44);
    f->fat0_lba=plba+f->rsvd;
    f->datalba=plba+f->rsvd+(ULONGLONG)f->nfat*f->fatsz;
    if(f->rsvd+(ULONGLONG)f->nfat*f->fatsz < psecs)
        f->maxclu=2+(psecs-(f->rsvd+(ULONGLONG)f->nfat*f->fatsz))/f->spc;
    else f->maxclu=2;
    abuf_free(&b0);
    return 1;
}

/* highest data cluster whose FAT0 entry is non-zero (allocated / bad), or 0
 * when the data area is completely free.  On a read failure *fail is set. */
static unsigned long long fat_highest_used(const Fat*f,int*fail)
{
    unsigned long long i,hi=0,entmax,fatbytes,need,bytes;
    ABuf a={0};
    *fail=0;
    if(f->maxclu<2)return 0;
    /* FAT entry for cluster c sits at byte c*4 in FAT0.  Read at most the whole
     * FAT0 (fatsz sectors); never more than that (past it lies FAT1/data). */
    fatbytes=f->fatsz*f->bps;
    need=(f->maxclu+1)*4ULL;
    bytes= need<fatbytes? need:fatbytes;
    if(bytes<12)return 0;                      /* no room for cluster-2 entry */
    {
        /* read whole sectors only (abuf block must hold every byte written) */
        unsigned long long sz=((bytes+f->bps-1)/f->bps)*f->bps;
        if(!abuf_ensure(&a,(size_t)sz,f->bps)){*fail=1;return 0;}
        if(!rd_sec(f->dev,f->fat0_lba,(unsigned)(sz/f->bps),a.buf)){
            *fail=1;abuf_free(&a);return 0;
        }
    }
    entmax=(bytes/4ULL)-1;
    if(entmax>f->maxclu)entmax=f->maxclu;
    {
        unsigned char*b=(unsigned char*)a.buf;
        for(i=2;i<=entmax;i++)
            if(rd32(b+(size_t)(i*4))!=0)hi=i;
    }
    abuf_free(&a);
    return hi;
}

/* fill the 3 CHS bytes of a *partition end* sector from an LBA */
static void chs_of_lba(unsigned long lba,unsigned char*o)
{
    unsigned head,sect,cyl;
    const unsigned hpc=255,spt=63,spc=hpc*spt;
    if(lba>=(unsigned long)1023*spc){o[0]=0xFE;o[1]=0xFF;o[2]=0xFF;return;}
    cyl=lba/spc; head=(lba%spc)/spt; sect=(lba%spt)+1;
    o[0]=(unsigned char)head;
    o[1]=(unsigned char)((sect&0x3F)|((cyl>>8)<<6));
    o[2]=(unsigned char)(cyl&0xFF);
}

static void shrink_last_partition(Dev*d,int disknum)
{
    ABuf a={0}; MbrEnt e[4]; unsigned char*m=NULL;
    int i,last=-1,fail=0; int isfat32=0;
    unsigned long long start=0,count=0,lastend=0;
    unsigned long long metadata,floor,newcount,high;
    Fat fat;
    char tn[40],c1[32],c2[32],c3[32],line[64];
    unsigned char mbr[512];

    if(!abuf_ensure(&a,d->sector,d->sector)){printf("  内存不足\n");return;}
    m=(unsigned char*)a.buf;
    if(!rd_sec(d,0,1,m)||!(m[510]==0x55&&m[511]==0xAA)){
        printf("  无标准 MBR。\n");abuf_free(&a);return;
    }
    mbr_decode(m,d->sector,e,4);
    /* pick the partition with the largest end */
    for(i=0;i<4;i++){
        if(!e[i].used)continue;
        unsigned long long en=(unsigned long long)e[i].start_lba+e[i].count;
        if(en>lastend){lastend=en;last=i;}
    }
    if(last<0){printf("  MBR 里没有分区。\n");abuf_free(&a);return;}
    start=e[last].start_lba; count=e[last].count;
    mbr_type_name(e[last].type,tn,sizeof tn);
    switch(e[last].type){case 0x0B:case 0x0C:case 0x1B:case 0x1C:isfat32=1;break;}
    human(start*d->sector,c1,sizeof c1);
    human(count*d->sector,c2,sizeof c2);
    human(lastend*d->sector,c3,sizeof c3);
    printf("\n  == 收缩最后一个分区（扩大盘尾隐藏区） ==\n");
    printf("  最后一个分区：第 %d 项，类型 %s，起始 LBA %llu（%s），%llu 扇区（%s），结束 LBA %llu（%s）\n",
           last,tn,(unsigned long long)start,c1,(unsigned long long)count,c2,
           (unsigned long long)lastend,c3);
    if(!isfat32){
        printf("  该分区不是 FAT32（0x0B/0x0C/0x1B/0x1C）。为不毁数据，只能收缩可自动检查空闲簇的 FAT32 分区。\n");
        abuf_free(&a);return;
    }
    if(!part_fat_geom(d,start,(unsigned long)count,&fat)){
        printf("  无法解析该分区引导扇区（可能无 55AA，或不是 FAT32）。\n");
        abuf_free(&a);return;
    }
    high=fat_highest_used(&fat,&fail);
    if(fail){printf("  读取 FAT 失败，无法安全计算可收缩量。\n");abuf_free(&a);return;}

    metadata=fat.rsvd+(ULONGLONG)fat.nfat*fat.fatsz;      /* sectors before data */
    floor=metadata;
    if(high>=2){
        unsigned long long cand=metadata+(high-1)*fat.spc;
        if(cand>floor)floor=cand;
    }
    if(floor>count){
        printf("  分区元数据就已超出现有长度（数据不一致），拒绝收缩。\n");
        abuf_free(&a);return;
    }
    if(high>=2)
        printf("  数据区已用到最高簇 %llu —— 分区长度下限 %llu 扇区（结束 LBA %llu）可安全包含它。\n",
               (unsigned long long)high,(unsigned long long)floor,
               (unsigned long long)(start+floor));
    else
        printf("  数据区全部空闲 —— 分区可一路缩到元数据末尾 %llu 扇区。\n",(unsigned long long)floor);

    if(floor==count){
        printf("  分区已用到无法再缩（已用数据贴着分区末尾），没有可释放的尾部扇区。\n");
        abuf_free(&a);return;
    }

    if(d->total){
        unsigned long long before=(unsigned long long)lastend*d->sector;
        if(before<d->total)human(d->total-before,c3,sizeof c3);
    }
    human((count-floor)*d->sector,c2,sizeof c2);
    printf("  当前尾部空闲区若缩到下限，可多释放 %s。\n",c2);
    printf("  输入要缩减的扇区数（0 取消；回车=缩到下限，即最大安全收缩）：");
    if(!fgets(line,sizeof line,stdin)){abuf_free(&a);return;}
    line[strcspn(line,"\r\n")]=0;
    if(!line[0])newcount=floor;
    else {
        unsigned long long v=strtoull(line,0,0);
        if(v==0){printf("  已取消。\n");abuf_free(&a);return;}
        if(count<=v){printf("  缩减量不能达到或超过分区长度。\n");abuf_free(&a);return;}
        newcount=count-v;
    }
    if(newcount<floor){
        printf("  收缩到 %llu 扇区会切到已用数据（下限 %llu），拒绝。\n",
               (unsigned long long)newcount,(unsigned long long)floor);
        abuf_free(&a);return;
    }
    if(newcount>=count){printf("  没有实际变化，取消。\n");abuf_free(&a);return;}

    if(!confirm_number("  确认写盘（不可逆）",disknum)){
        printf("  已取消。\n");abuf_free(&a);return;
    }
    if(!d->is_image)lock_disk_volumes(disknum);

    /* rewrite the partition-table entry: new sector count + end CHS */
    memcpy(mbr,m,d->sector<sizeof mbr?d->sector:(DWORD)sizeof mbr);
    {
        unsigned char*p=mbr + 0x1BE + 16*last;
        unsigned long ns=(unsigned long)(newcount&0xFFFFFFFFUL);
        p[12]=(unsigned char)(ns&255);p[13]=(unsigned char)((ns>>8)&255);
        p[14]=(unsigned char)((ns>>16)&255);p[15]=(unsigned char)((ns>>24)&255);
        chs_of_lba((unsigned long)((start+newcount-1)&0xFFFFFFFFUL),p+5);
    }
    if(!wr_sec(d,0,1,mbr)){
        printf("  写 MBR 失败。\n");
    } else {
        unsigned long long ntail=d->total?(d->total-(start+newcount)*d->sector):0;
        human(ntail,c1,sizeof c1);
        printf("  已把分区 %d 的扇区数改为 %llu（结束 LBA %llu）。\n",
               last,(unsigned long long)newcount,(unsigned long long)(start+newcount));
        printf("  从分区末尾 %llu 到盘尾的空间现在有 %s（字节 %llu .. %llu），OS 视为未分区。\n",
               (unsigned long long)(start+newcount),c1,
               (unsigned long long)(start+newcount)*d->sector,
               d->total?(unsigned long long)d->total:0);
        printf("  警告：Windows 磁盘管理/分区工具会把这段显示为“未分配”，只是内容平时不可见。\n");
        printf("  接下来可进“3 自制文件系统管理”把这截空间用作隐藏区。\n");
    }
    if(!d->is_image)unlock_disk_volumes();
    abuf_free(&a);
}

/* ------------------------------------------------------------------ */
/* main / menu                                                         */
/* ------------------------------------------------------------------ */

static void usage(void)
{
    printf(
      "getfat —— FAT32 实验工具（交互菜单，中文输出）\n\n"
      "用法:\n"
      "  getfat                交互：先选物理盘再进菜单\n"
      "  getfat N              直接对 PhysicalDriveN 交互\n"
      "  getfat 镜像.img        对 raw 磁盘镜像交互（安全，无需管理员）\n\n"
      "功能: MBR 中文分析 / 盘尾空闲区 /\n"
      "      自制文件系统管理(目录10项+0x55AA, 8.3文件名;列出/添加/导出到指定目录/删除/整理) /\n"
      "      FAT32 目录项起始簇改写 / 收缩最后一个 FAT32 分区扩大盘尾隐藏区(自动避开已用簇)\n"
      "物理盘需要管理员权限；写操作前会要求再次确认，且默认拒绝写 0 号盘。\n");
}

int main(int argc,char**argv)
{
    int disknum=-1;
    const char*image=NULL;
    Dev d;
    int ok;

    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout,NULL,_IONBF,0);   /* prompts appear even through a pipe */
    setvbuf(stderr,NULL,_IONBF,0);
    SetConsoleCP(CP_UTF8);

    printf("getfat —— FAT32 实验工具\n");

    /* parse args: a single numeric argument = disk, else = image path */
    if(argc>1){
        if(_stricmp(argv[1],"-h")==0||_stricmp(argv[1],"--help")==0){usage();return 0;}
        if(isdigit((unsigned char)argv[1][0])&&strspn(argv[1],"0123456789")==strlen(argv[1])){
            disknum=atoi(argv[1]);
        } else image=argv[1];
    }

    if(!image&&disknum<0){
        /* picker */
        int count=0;
        printf("\n  可用物理磁盘：\n");
        list_disks(&count);
        if(count==0&&!running_elevated()){
            printf("\n  未以管理员运行且打不开磁盘。尝试提权……\n");
            if(relaunch_elevated(argc,argv)){
                printf("  UAC 提权窗口已弹出，请在弹窗中选“是”。\n");
                return 0;
            }
            printf("\n  请手动“右键→以管理员身份运行”。\n");
            return 1;
        }
        printf("  输入要操作的磁盘号：");
        {
            char b[32];long v;
            if(!fgets(b,sizeof b,stdin))return 0;
            v=atol(b);
            if(v<0||v>255){printf("  非法磁盘号\n");return 1;}
            disknum=(int)v;
        }
    }

    if(!image&&disknum<0){usage();return 0;}

    if(image){
        if(!dev_open(&d,0,image,1,0))return 1;
        printf("  已打开镜像: %s (无需管理员)\n",image);
    } else {
        if(!dev_open(&d,disknum,NULL,1,0)){
            printf("\n  若因权限失败，请以管理员运行（或让程序自动提权）。\n");
            return 1;
        }
    }
    if(d.total==0){printf("  设备长度未知。\n");dev_close(&d);return 1;}

    /* main menu loop */
    ok=1;
    while(ok){
        char c1[32];
        human(d.total,c1,sizeof c1);
        printf("\n========================================================\n");
        printf("  当前对象: %s   容量 %s\n",d.title,c1);
        printf("--------------------------------------------------------\n");
        printf("  1) 中文分析 MBR / 主分区表\n");
        printf("  2) 盘尾空闲区（最后一个分区之后，OS 无法识别）\n");
        printf("  3) 自制文件系统管理（隐藏区：目录10项+0x55AA；列出/添加/导出/删除/整理）\n");
        printf("  4) FAT32 目录项起始簇改写（全部 -> 指定文件，含预演）\n");
        printf("  5) 收缩最后一个 FAT32 分区，扩大盘尾隐藏区（自动避开已用簇）\n");
        printf("  0) 退出\n");
        printf("  请选择：");
        fflush(stdout);
        {
            char b[32];int v;
            if(!fgets(b,sizeof b,stdin))break;
            v=atoi(b);
            switch(v){
            case 1: show_mbr(&d); break;
            case 2: show_tail(&d,0); break;
            case 3: {
                /* write op: refuse disk 0 by default */
                if(!d.is_image&&disknum==0&&!confirm_number(
                     "  0 号通常是系统盘！若确要写它",-99)){
                    printf("  已取消（保护系统盘）。\n");
                } else custom_fs_menu(&d,d.is_image?0:disknum);
                break;
            }
            case 4: {
                if(!d.is_image&&disknum==0&&!confirm_number(
                     "  0 号通常是系统盘！若确要写它",-99)){
                    printf("  已取消（保护系统盘）。\n");
                } else fat32_rewrite_menu(&d,d.is_image?0:disknum);
                break;
            }
            case 5: {
                if(!d.is_image&&disknum==0&&!confirm_number(
                     "  0 号通常是系统盘！若确要写它",-99)){
                    printf("  已取消（保护系统盘）。\n");
                } else shrink_last_partition(&d,d.is_image?0:disknum);
                break;
            }
            case 0: ok=0; break;
            default: printf("  无效选项。\n"); break;
            }
        }
    }
    dev_close(&d);
    printf("\n  再见。\n");
    return 0;
}
