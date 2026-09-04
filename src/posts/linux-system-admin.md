---
title: Linux 系统管理与常用工具
date: 2026-06-12
category: 学习记录
tags: ["linux", "系统管理", "后台进程", "磁盘", "wget", "ffmpeg"]
excerpt: 后台进程与端口管理、磁盘挂载与块设备、wget 下载与 ffmpeg 音视频处理
---
# 后台进程与端口管理

## 前台进程与后台进程

Linux 下的进程按是否占用终端分为两种：

- **前台进程**：占用终端，必须等待其执行完毕才能继续操作。
- **后台进程**：在终端后台运行，不阻塞当前终端操作。

## 进程状态

| 状态 | 含义 | 说明 |
|------|------|------|
| R | Running | 正在运行或可运行 |
| S | Sleeping | 睡眠状态，等待事件 |
| D | Uninterruptible Sleep | 不可中断睡眠（通常是 I/O） |
| T | Stopped | 停止或跟踪 |
| Z | Zombie | 僵尸进程 |

## 后台运行方法

### 使用 `&` 直接后台运行

基本语法：

```bash
command &
```

示例：

```bash
# 后台运行 Python 脚本
python3 server.py &

# 后台运行 Node.js 应用
node app.js &

# 后台运行并输出日志
python3 script.py > output.log 2>&1 &
```

注意：`&` 会让进程在后台运行，但**终端关闭时进程也会终止**，推荐结合 `nohup` 使用。

### 使用 `nohup` 忽略挂断信号

基本语法：

```bash
nohup command [arg] &
```

示例：

```bash
# 基本使用
nohup python3 server.py &

# 指定日志文件
nohup node app.js > app.log 2>&1 &

# 后台运行 Java 应用
nohup java -jar myapp.jar > /dev/null 2>&1 &
```

要点：

- `nohup` 忽略 `SIGHUP` 信号，终端关闭后进程继续运行。
- 默认输出到 `nohup.out` 文件。
- `2>&1` 将错误输出重定向到标准输出。

### 使用 `disown` 解除终端关联

把正在运行的前台任务转为后台，并解除与终端的关联：

```bash
# 1. 运行命令
sleep 1000

# 2. 按下 Ctrl+Z 暂停任务
# 3. 放入后台
bg

# 4. 解除与终端关联
disown %1
```

### 使用 `screen` 保持会话

即使断开 SSH，会话中的程序也会继续运行，适合远程任务。

安装：

```bash
sudo apt-get install screen   # Ubuntu
```

常用命令：

```bash
# 创建新会话
screen -S session_name

# 分离会话（Ctrl+A，然后按 D）
# Ctrl+A D

# 列出会话
screen -ls

# 重新连接
screen -r session_name

# 杀死会话
screen -X -S session_name quit
```

### 使用 `tmux` 保持会话

类似 screen 的终端复用工具，安装：

```bash
sudo apt-get install tmux
```

常用命令：

```bash
# 创建新会话
tmux new -s session_name

# 分离会话（Ctrl+B D）
# Ctrl+B D

# 列出会话
tmux ls

# 重新连接
tmux attach -t session_name

# 杀死会话
tmux kill-session -t session_name
```

## 进程管理命令

### ps：查看进程

```bash
# 查看所有进程
ps aux

# 查看特定用户进程
ps -u username

# 查看进程树
ps auxf

# 查看特定进程
ps aux | grep python

# 查看进程详细信息
ps -ef
```

ps 输出字段说明：

- `USER`：进程所有者
- `PID`：进程 ID
- `%CPU`：CPU 使用率
- `%MEM`：内存使用率
- `VSZ`：虚拟内存大小
- `RSS`：物理内存大小
- `TTY`：终端类型
- `STAT`：进程状态
- `START`：启动时间
- `TIME`：CPU 占用时间
- `COMMAND`：命令名称

### top / htop：实时监控

```bash
# 实时查看进程
top

# 按内存排序
top -o %MEM

# 查看特定用户的进程
top -u username

# 非交互模式，只显示一次
top -b -n 1
```

top 交互命令：

- `P`：按 CPU 使用率排序
- `M`：按内存使用率排序
- `k`：杀死进程（输入 PID）
- `q`：退出

htop 是更友好的增强版（安装：`sudo apt-get install htop`），界面更直观、支持鼠标操作、彩色显示与树形视图。

### kill：终止进程

```bash
# 发送 TERM 信号（正常终止）
kill PID

# 发送 KILL 信号（强制终止）
kill -9 PID

# 发送特定信号
kill -SIGNAL PID

# 杀死所有同名进程
killall process_name

# 按名称杀死进程
pkill process_name
```

常用信号：

| 信号 | 编号 | 用途 |
|------|------|------|
| SIGTERM | 15 | 正常终止 |
| SIGKILL | 9 | 强制终止 |
| SIGHUP | 1 | 重新加载配置 |
| SIGINT | 2 | 中断（Ctrl+C） |
| SIGSTOP | 19 | 暂停进程 |

### 批量管理进程

```bash
# 杀死所有 Python 进程
killall python3
pkill -f "python3"

# 根据条件杀死进程
ps aux | grep 'defunct' | awk '{print $2}' | xargs kill -9

# 杀死占用特定端口的进程
kill -9 $(lsof -t -i:8080)
```

## 端口管理

### lsof：查看端口占用

```bash
# 查看所有端口
lsof -i

# 查看特定端口
lsof -i:8080

# 查看 TCP 端口
lsof -iTCP

# 查看 UDP 端口
lsof -iUDP

# 查看特定协议的端口
lsof -iTCP:80

# 显示进程 PID（不做域名解析）
lsof -i:8080 -P -n
```

### netstat：查看端口

```bash
# 查看所有端口
netstat -tuln

# 查看 TCP 端口
netstat -tln

# 查看 UDP 端口
netstat -uln

# 显示进程信息
netstat -tulnp

# 查看路由表
netstat -r
```

选项说明：

- `-t`：TCP 端口
- `-u`：UDP 端口
- `-l`：监听状态
- `-n`：显示数字地址
- `-p`：显示进程信息

### ss：现代替代方案

```bash
# 查看所有端口
ss -tuln

# 查看 TCP 端口
ss -tln

# 查看 UDP 端口
ss -uln

# 显示进程信息
ss -tulnp

# 查看连接状态汇总
ss -s
```

### 杀死占用端口的进程

```bash
# 方法 1：使用 lsof
lsof -i:8080
kill -9 PID

# 方法 2：使用 fuser
fuser -k 8080/tcp

# 方法 3：使用 kill + lsof
kill -9 $(lsof -t -i:8080)

# 方法 4：使用 netstat + awk
netstat -tlnp | grep 8080 | awk '{print $7}' | cut -d'/' -f1 | xargs kill -9
```

### 端口转发

使用 `iptables` 实现本地端口转发：

```bash
iptables -t nat -A PREROUTING -p tcp --dport 80 -j REDIRECT --to-port 8080
```

远程端口转发：

```bash
iptables -t nat -A PREROUTING -p tcp --dport 80 -j DNAT --to-destination 192.168.1.100:8080
```

使用 `socat`（安装：`sudo apt-get install socat`）：

```bash
socat TCP-LISTEN:80,fork TCP:localhost:8080
```

### 端口扫描

```bash
# 使用 nmap
nmap -p 1-1000 localhost

# 使用 nc
nc -zv localhost 80

# 使用 telnet
telnet localhost 80
```

## 守护进程管理：systemd 服务

创建服务文件：

```bash
sudo vim /etc/systemd/system/myapp.service
```

服务文件内容：

```ini
[Unit]
Description=My Application Service
After=network.target

[Service]
Type=simple
User=myuser
WorkingDirectory=/opt/myapp
ExecStart=/usr/bin/python3 /opt/myapp/server.py
Restart=on-failure
RestartSec=5
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```

管理服务：

```bash
# 启动服务
sudo systemctl start myapp

# 停止服务
sudo systemctl stop myapp

# 重启服务
sudo systemctl restart myapp

# 查看状态
sudo systemctl status myapp

# 开机自启
sudo systemctl enable myapp

# 查看日志（-f 实时跟踪）
sudo journalctl -u myapp -f
```

## 日志管理

```bash
# 重定向输出
nohup python3 server.py > app.log 2>&1 &

# 使用 logger
nohup python3 server.py 2>&1 | logger -t myapp &

# logrotate 配置文件
sudo vim /etc/logrotate.d/myapp
```

logrotate 配置示例：

```
/var/log/myapp/*.log {
    daily
    rotate 7
    compress
    delaycompress
    missingok
    notifempty
    create 644 root root
}
```

## 进程监控

使用 cron 定期检查：

```bash
# 每 5 分钟运行一次检查脚本
*/5 * * * * /usr/bin/python3 /scripts/check_process.py
```

使用 monit（安装：`sudo apt-get install monit`），配置示例：

```
check process myapp with pidfile /var/run/myapp.pid
    start program = "/usr/bin/systemctl start myapp"
    stop program = "/usr/bin/systemctl stop myapp"
    if failed port 8080 protocol http then restart
    if 5 restarts within 5 cycles then timeout
```

## 资源限制

```bash
# 查看进程限制
ulimit -a

# 设置打开文件数限制
ulimit -n 65535

# 使用 nice 调整优先级
nice -n -10 python3 server.py

# 使用 cgroups 限制资源
sudo cgcreate -g cpu,memory:/mygroup
sudo cgset -r cpu.shares=512 mygroup
sudo cgset -r memory.limit_in_bytes=512M mygroup
sudo cgclassify -g cpu,memory:/mygroup PID
```

## 常见问题与故障排除

### 僵尸进程处理

```bash
# 查找僵尸进程
ps aux | grep Z

# 杀死僵尸进程的父进程
kill -9 PPID

# 批量处理
ps aux | awk '{if ($8 == "Z") print $2}' | xargs kill -9
```

### 端口被占用

```bash
# 查看端口占用
lsof -i:8080

# 强制释放端口
fuser -k 8080/tcp

# 检查是否为 root 权限（低端口需要 root）
sudo lsof -i:80
```

### 进程无法终止

```bash
# 使用 SIGKILL
kill -9 PID

# 使用 killall
killall -9 process_name

# 使用 pkill
pkill -9 -f "process_name"

# 使用 systemd
sudo systemctl kill -s SIGKILL myapp
```

### 后台进程丢失

```bash
# 使用 jobs 查看当前终端的后台任务
jobs

# 使用 ps 查找
ps aux | grep process_name

# 使用 screen 恢复
screen -r session_name

# 使用 tmux 恢复
tmux attach -t session_name
```

### 端口无法访问

```bash
# 检查防火墙
sudo ufw status
sudo iptables -L -n

# 检查服务状态
sudo systemctl status myapp

# 检查监听状态
ss -tlnp | grep 8080

# 检查本地连接
curl localhost:8080
telnet localhost 8080
```

## 总结与最佳实践

核心命令速查表：

| 操作 | 命令 |
|------|------|
| 后台运行 | `nohup command &` |
| 查看进程 | `ps aux`、`top`、`htop` |
| 杀死进程 | `kill -9 PID`、`killall name` |
| 查看端口 | `lsof -i:port`、`netstat -tuln` |
| 杀死端口 | `lsof -t -i:port \| xargs kill -9` |
| 创建服务 | `systemctl enable/start/stop` |

推荐工作流程：

- 开发阶段：使用 `screen` 或 `tmux`
- 测试阶段：使用 `nohup` + 日志重定向
- 生产环境：使用 `systemd` 服务管理

安全注意事项：

- 不要轻易使用 `kill -9`，优先使用 `SIGTERM`。
- 生产环境使用 `systemd` 管理服务。
- 定期检查僵尸进程。
- 设置合理的资源限制。
- 使用防火墙限制端口访问。
# 磁盘管理与挂载

Linux 命令行版本不会自动识别并挂载新插入的存储设备，需要手动挂载到某一目录后才能访问。所谓挂载（Mount），简单来说就是把一个存储设备连接到 Linux 文件系统树的过程——Windows 的文件共享、插入的存储设备、docker 的存储盘等，都要先挂载才能进行操作，相当于建立起操作平台。

## 查看块设备：lsblk

### 基本用法

`lsblk` 用于列出块设备（block 设备），是查看磁盘信息最常用的命令：

```bash
lsblk          # 列出块设备（最常用）
lsblk -f       # 显示文件系统信息（UUID、文件系统类型、挂载点）
lsblk -a       # 显示所有设备（包括空设备）
lsblk -l       # 以列表形式显示（非树状）
```

插入 U 盘后先用 `lsblk` 查看设备，通常 U 盘以 `sdb`、`sdc` 等标识；再用 `lsblk -f` 查看卷标、UUID（通用唯一识别码），主要确认它使用了什么文件系统。

### 输出字段含义

默认输出的字段：

| 字段 | 含义 | 示例 |
|------|------|------|
| NAME | 设备名 | sda, sda1, sdb |
| MAJ:MIN | 主设备号:次设备号 | 8:0 |
| RM | 是否可移除 | 0=固定, 1=可移除 |
| SIZE | 容量 | 20G, 931.5G |
| RO | 是否只读 | 0=可写, 1=只读 |
| TYPE | 设备类型 | disk（磁盘）, part（分区）, lvm（逻辑卷） |
| MOUNTPOINT | 挂载点 | /, /boot, /media/usb |

使用 `lsblk -f` 还会额外显示：

| 字段 | 含义 | 示例 |
|------|------|------|
| UUID | 唯一标识符 | 1234-5678 |
| FSTYPE | 文件系统类型 | ext4, ntfs, exfat |

## U 盘的挂载与卸载

`/media` 是 Linux 专门用来挂载可移动媒体的目录，可在这里创建一个用于挂载的文件夹，通常用卷标命名。`/mnt` 是 mount（挂载）的缩写，早期用于临时手动挂载；后来桌面版有了自动挂载，就又设计了 `/media` 文件夹。

完整挂载步骤（以卷标为 PS1008 的 U 盘为例）：

```bash
# 1. 用 lsblk 查看设备，确认 U 盘对应的设备名与分区（如 /dev/sdb2）

# 2. 在 /media 下创建挂载点目录（用卷标命名）
sudo mkdir -p /media/PS1008

# 3. 用 mount 挂载具体分区（-t 指定文件系统类型）
sudo mount -t exfat /dev/sdb2 /media/PS1008
```

挂载成功后就可以访问 U 盘里的文件了。

使用完毕后，需要先退出 U 盘所在目录（bash 程序占据目录时不能卸载），再执行卸载。卸载可以指定设备文件，也可以指定挂载路径，两种方法都可以：

```bash
sudo umount /dev/sdb2        # 直接指定设备文件
sudo umount /media/PS1008    # 指定挂载路径
```
# 使用 wget 下载

## 基本语法

```bash
wget [选项] [URL]
```

wget 是 Linux 下最常用的非交互式网络下载工具，支持断点续传、递归下载整个网站，即使后台运行也不怕断开 SSH。

## 基本选项

| 选项 | 说明 | 示例 |
|------|------|------|
| `-O` | 指定下载文件的保存名（大写 O） | `wget -O game.zip https://example.com/file.zip` |
| `-P` | 指定下载文件保存到的目录 | `wget -P /home/user/downloads/ https://example.com/file.zip` |
| `-c` | 断点续传（下载中断后继续，极其常用） | `wget -c https://example.com/large_file.iso` |
| `-b` | 后台下载（退出终端也不影响） | `wget -b https://example.com/file.zip` |
| `-q` | 安静模式（不输出下载进度条） | `wget -q https://example.com/file.zip` |
| `-V` | 查看 wget 版本号 | `wget -V` |
| `--limit-rate` | 限制下载速度（防占满带宽） | `wget --limit-rate=1M https://example.com/file.zip` |
| `--tries` | 设置最大重试次数（默认 20） | `wget --tries=3 https://example.com/file.zip` |
| `--timeout` | 设置超时时间（秒） | `wget --timeout=10 https://example.com/file.zip` |
| `-Q` | 设置最大下载配额（超过则停止） | `wget -Q 100M https://example.com/large_file.zip` |
| `--spider` | 爬虫模式（只检查文件是否存在，不下载） | `wget --spider https://example.com/file.zip` |
| `--no-check-certificate` | 忽略 SSL 证书验证（解决某些报错） | `wget --no-check-certificate https://example.com` |

## 模拟浏览器与身份验证

| 选项 | 说明 | 示例 |
|------|------|------|
| `-U` | 自定义 User-Agent（伪装成浏览器） | `wget -U "Mozilla/5.0" https://example.com` |
| `--user/--password` | 传入网站登录的用户名和密码 | `wget --user=admin --password=123456 https://example.com/admin.zip` |
| `--load-cookies` | 加载本地 cookies 文件（绕过登录验证） | `wget --load-cookies=cookies.txt https://example.com/vip.zip` |
| `--post-data` | 发送 POST 请求数据（常用于接口测试） | `wget --post-data="user=test" https://example.com/api` |
| `--header` | 添加自定义请求头 | `wget --header="Accept-Language: zh-CN" https://example.com` |

## 递归下载与镜像网站（爬虫/整站下载）

| 选项 | 说明 | 示例 |
|------|------|------|
| `-r` | 递归下载（下载网页里所有的链接资源） | `wget -r https://example.com` |
| `-l` | 设置递归深度（配合 `-r` 使用，0 为无限） | `wget -r -l 2 https://example.com`（只下前两层链接） |
| `-m` | 镜像网站（等同于 `-r -N -l inf`，自动建目录） | `wget -m https://example.com` |
| `-k` | 转换链接（让下载的 HTML 里的链接指向本地文件） | `wget -r -k https://example.com` |
| `-p` | 下载所有页面必需的资源（如 CSS、图片） | `wget -p https://example.com/index.html` |
| `-A` | 只下载指定后缀的文件（Accept） | `wget -r -A jpg,png,gif https://example.com` |
| `-R` | 拒绝下载指定后缀的文件（Reject） | `wget -r -R pdf,avi https://example.com` |
| `--reject-dir` | 拒绝进入指定的目录 | `wget -r --reject-dir=images https://example.com` |
| `-np` | 不追溯到父目录（防止跑到根目录去下载） | `wget -r -np https://example.com/dir/` |

## 日志与输出

| 选项 | 说明 | 示例 |
|------|------|------|
| `-o` | 将下载日志输出到指定文件（覆盖写入） | `wget -o download.log https://example.com/file.zip` |
| `-a` | 将下载日志追加到指定文件 | `wget -a download.log https://example.com/file.zip` |
| `-O -` | 将下载内容直接打印到屏幕（不保存文件） | `wget -O - https://example.com/index.html` |
# ffmpeg 音视频处理

## 基本语法

```bash
ffmpeg [全局选项] -i [输入文件] [处理参数] [输出文件]
```

ffmpeg 的参数顺序很重要：通常 `-i 输入文件` 放前面，输出文件放最后。输出文件的后缀名决定了生成的格式。

## 基本选项

| 选项 | 说明 | 示例 |
|------|------|------|
| `-y` | 覆盖输出文件（不弹询问提示） | `ffmpeg -y -i input.mp4 output.mp4` |
| `-i` | 指定输入文件 | `ffmpeg -i video.avi` |
| `-f` | 强制指定输入或输出的格式 | `ffmpeg -i input.jpg -f gif output.gif` |
| `-t` | 设置处理时长（秒） | `ffmpeg -i input.mp4 -t 10 output.mp4`（只取前 10 秒） |
| `-ss` | 快速定位到指定时间开始处理 | `ffmpeg -ss 00:01:30 -i input.mp4 output.mp4`（从 1 分 30 秒开始） |
| `-to` | 处理到指定时间结束 | `ffmpeg -ss 00:00:10 -to 00:00:20 -i input.mp4 output.mp4` |
| `-c` | 指定编码器（`-c:v` 视频，`-c:a` 音频） | `ffmpeg -i input.mp4 -c:v libx264 output.mp4` |
| `-c copy` | 不重新编码，直接复制数据流（极快无损） | `ffmpeg -i input.mp4 -c copy output.mp4` |
| `-vn` | 丢弃视频流（去掉画面，只留声音） | `ffmpeg -i input.mp4 -vn output.mp3` |
| `-an` | 丢弃音频流（去掉声音，只留画面） | `ffmpeg -i input.mp4 -an output.mp4` |
| `-r` | 设置帧率 | `ffmpeg -i input.mp4 -r 30 output.mp4` |
| `-s` | 设置分辨率大小 | `ffmpeg -i input.mp4 -s 1280x720 output.mp4` |
| `-b:v` | 设置视频码率（控制文件体积） | `ffmpeg -i input.mp4 -b:v 1M output.mp4` |
| `-preset` | 编码速度预设（ultrafast 最快，slow 体积小） | `ffmpeg -i input.mp4 -preset ultrafast output.mp4` |

## 核心过滤器（-vf 视频滤镜 / -af 音频滤镜）

视频处理用 `-vf` 指定视频滤镜，音频处理用 `-af` 指定音频滤镜。

| 滤镜 | 说明 | 示例 |
|------|------|------|
| scale | 缩放画面分辨率 | `ffmpeg -i in.mp4 -vf "scale=800:600" out.mp4` |
| fps | 改变帧率（滤镜写法） | `ffmpeg -i in.mp4 -vf "fps=15" out.mp4` |
| pad | 给画面加黑边/填充颜色 | `ffmpeg -i in.mp4 -vf "pad=1280:720:100:50:black" out.mp4` |
| crop | 裁剪画面（宽:高:X:Y 起始点） | `ffmpeg -i in.mp4 -vf "crop=640:480:0:0" out.mp4` |
| drawtext | 在画面上添加文字水印 | `ffmpeg -i in.mp4 -vf "drawtext=text='Hello':fontcolor=white:fontsize=30:x=10:y=10" out.mp4` |
| overlay | 画中画（把图 2 贴在图 1 上） | `ffmpeg -i bg.mp4 -i small.mp4 -filter_complex "overlay=10:10" out.mp4` |
| volume | 调整音量（1.0 是原声，2.0 是两倍） | `ffmpeg -i in.mp4 -af "volume=1.5" out.mp4` |
| rotate | 旋转画面（顺时针 90 度） | `ffmpeg -i in.mp4 -vf "transpose=1" out.mp4` |

## GIF 专属处理（游戏开发重点）

| 用途 | 示例 |
|------|------|
| 高质量序列帧合成 GIF（防色块必备） | `ffmpeg -framerate 15 -i img_%03d.png -vf "split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" out.gif` |
| GIF 拆解为图片序列帧 | `ffmpeg -i input.gif frame_%03d.png` |
| 普通视频转 GIF 并压缩宽度 | `ffmpeg -i input.mp4 -vf "fps=12,scale=480:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" out.gif` |
| 截取视频某段转 GIF | `ffmpeg -ss 00:00:05 -t 3 -i input.mp4 -vf "fps=15,scale=480:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" out.gif` |

## 音视频分离与合并（游戏素材处理重点）

| 用途 | 示例 |
|------|------|
| 提取视频中的纯音频为 mp3 | `ffmpeg -i input.mp4 -vn -y bgm.mp3` |
| 提取视频中的纯画面（去声音） | `ffmpeg -i input.mp4 -an -y silent_video.mp4` |
| 给无声视频替换/添加背景音乐 | `ffmpeg -i video.mp4 -i bgm.mp3 -c:v copy -c:a aac -shortest final.mp4` |

多段视频无缝拼接需先建 `list.txt`，逐行写入待拼接的文件：

```
file '1.mp4'
file '2.mp4'
```

然后执行拼接命令（`-c copy` 直接复制流，不重新编码）：

```bash
ffmpeg -f concat -i list.txt -c copy merged.mp4
```
