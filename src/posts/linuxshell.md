---
title: Linux 的 Shell 脚本学习
date: 2026-09-03
category: 学习记录
tags: ["linux", "shell"]
excerpt: Shell 脚本学习：基础命令、变量、条件、循环、函数、数组与文本处理
---
# Shell 基础与变量

Shell 是 Linux 中用户与内核之间的桥梁：它接收用户输入的命令并交给操作系统内核执行，也支持把命令按逻辑写进文本文件，批量自动化运行。

## 核心概念速查

| 概念 | 说明 |
|---|---|
| Shell | 命令行解释器，接收用户输入的命令并传递给操作系统内核执行（人机交互的桥梁） |
| Bash | Bourne Again Shell，Linux 中最默认、最通用的 Shell（macOS 默认已换成 zsh） |
| 脚本 | 将多个 Shell 命令按逻辑写入一个文本文件，批量自动化执行（`.sh` 文件） |
| Shebang | 脚本第一行的 `#!/bin/bash`，告诉系统用哪个解释器来执行这个脚本 |
| 标准输入（STDIN） | 文件描述符为 0，默认来自键盘输入 |
| 标准输出（STDOUT） | 文件描述符为 1，默认输出到终端屏幕 |
| 标准错误（STDERR） | 文件描述符为 2，默认输出到终端屏幕 |
| 管道 | `\|`，将前一个命令的 STDOUT 作为后一个命令的 STDIN |
| 重定向 | `>` 或 `>>`，改变命令默认的输入输出方向 |
| 环境变量 | 全局变量，对所有 Shell 和子进程生效（如 `$PATH`） |
| 局部变量 | 只在当前 Shell 进程中生效的变量 |
| 退出状态码 | `$?`，上一个命令执行后的返回值（0 表示成功，非 0 表示失败） |

> **小贴士：一切皆文件。** Linux 系统的特点是一切皆文件，设备、进程等底层资源都以文件形式暴露；同时几乎所有配置都是文本形式，可以直接查看和修改。

## 变量与环境变量

变量分为局部变量与环境变量：局部变量只在当前 Shell 进程中生效，环境变量通过 `export` 导出后对所有 Shell 和子进程生效。定义变量时，等号两边不能有空格。

| 操作 | 语法 | 示例 |
|---|---|---|
| 定义局部变量 | `变量名=值`（等号两边不能有空格） | `name="Tom"` |
| 使用变量 | `$变量名` 或 `${变量名}` | `echo $name`；`echo ${name}_log` |
| 重新赋值 | `变量名=新值` | `name="Jerry"` |
| 只读变量 | `readonly 变量名` | `readonly PI=3.14` |
| 删除变量 | `unset 变量名` | `unset name` |
| 定义环境变量 | `export 变量名=值` | `export PATH=$PATH:/new/path` |
| 查看环境变量 | `env` / `printenv` / `export` | `env` |
| 查看所有变量（含局部） | `set` | `set` |

常用环境变量：

- `$HOME`：家目录
- `$USER`：用户名
- `$PWD`：当前路径
- `$SHELL`：当前 Shell
- `$PATH`：命令搜索路径，可用 `echo $PATH` 查看

脚本内的特殊变量：

- `$0`：脚本名
- `$1`~`$9`：第 1 到第 9 个参数
- `$#`：参数个数
- `$@`：所有参数
- `$?`：上一条命令的退出码
- `$$`：当前进程 PID

例如 `echo "脚本名: $0, 参数: $1"`。

# 文件与文本处理

## 文件与目录命令

| 命令 | 说明 | 示例 |
|---|---|---|
| `pwd` | Print Working Directory，显示当前所在路径 | `pwd` |
| `ls` | 列出目录内容 | `ls -la`（显示所有文件含隐藏文件，长格式） |
| `cd` | 切换目录 | `cd /home/user`；`cd ..`（上一级）；`cd ~`（家目录） |
| `mkdir` | 创建目录 | `mkdir -p a/b/c`（递归创建多级目录） |
| `touch` | 创建空文件或更新时间戳 | `touch file.txt` |
| `cp` | 复制文件或目录 | `cp -r dir1/ dir2/`（递归复制目录） |
| `mv` | 移动或重命名文件 | `mv old.txt new.txt`；`mv file.txt /tmp/` |
| `rm` | 删除文件或目录 | `rm -rf dir/`（强制递归删除，极其危险！） |
| `find` | 查找文件 | `find /tmp -name "*.log" -type f` |
| `ln` | 创建链接 | `ln -s target link_name`（创建软链接） |
| `tree` | 以树形结构显示目录 | `tree -L 2`（只显示 2 层深度） |
| `du` | 查看磁盘使用量 | `du -sh *`（人类可读格式，查看当前目录下所有文件大小） |

> **小贴士**：文件名存在空格时，需要用引号把整个文件名引起来；`.bashrc` 是终端启动时运行的脚本，除此之外还有更全局的启动前脚本。

## 文件查看

| 命令 | 说明 | 示例 |
|---|---|---|
| `cat` | 查看文件全部内容 | `cat file.txt`；`cat a.txt b.txt > c.txt`（合并文件） |
| `less` | 分页查看大文件（支持上下翻页、搜索） | `less /var/log/syslog` |
| `head` | 查看文件开头前 N 行（默认 10 行） | `head -n 20 file.txt` |
| `tail` | 查看文件末尾 N 行 | `tail -n 50 -f file.log`（`-f` 实时追踪更新） |

## grep：文本过滤工具

grep 用于在文件或输出中查找符合特定模式（支持正则表达式）的行，是日常排查日志的利器。基本语法为 `grep [选项] [模式] [文件]`。

```bash
grep "error" server.log          # 在日志中搜索包含 error 的行
grep -i "warning" app.log        # 忽略大小写搜索 warning
grep -r "TODO" ./src/            # 递归搜索 src 目录下所有文件
grep -E 'error|warn' log.txt     # 扩展正则，匹配 error 或 warn
```

常用选项：

| 选项 | 说明 | 示例 |
|---|---|---|
| `-i` | 忽略大小写 | `grep -i "hello" file.txt` |
| `-v` | 反向匹配，显示不包含的行 | `grep -v "error" file.txt` |
| `-n` | 显示行号 | `grep -n "function" app.js` |
| `-c` | 只显示匹配的行数 | `grep -c "var" file.js` |
| `-l` | 只显示包含匹配的文件名 | `grep -l "WB_PROP_KEY" *.js` |
| `-L` | 只显示不包含匹配的文件名 | `grep -L "TODO" *.js` |
| `-w` | 匹配整个单词 | `grep -w "var" file.js` |
| `-x` | 匹配整行 | `grep -x "exact line" file.txt` |
| `-r` / `-R` | 递归搜索子目录 | `grep -r "TODO" ./src/`；`grep -r "function" .` |
| `--include` | 只搜索特定文件类型 | `grep -r --include="*.js" "setItem" .` |
| `--exclude` | 排除特定文件 | `grep -r --exclude="*.map" "save" .` |
| `--exclude-dir` | 排除目录 | `grep -r --exclude-dir=node_modules "require" .` |
| `-A NUM` | 显示匹配行及后面 NUM 行 | `grep -A 5 "error" log.txt` |
| `-B NUM` | 显示匹配行及前面 NUM 行 | `grep -B 5 "error" log.txt` |
| `-C NUM` | 显示匹配行及前后 NUM 行 | `grep -C 5 "error" log.txt` |
| `-o` | 只显示匹配的部分 | `grep -o "[0-9]+" file.txt` |
| `--color` | 高亮显示匹配 | `grep --color "pattern" file.txt` |

正则表达式：基本正则中 `+`、`?`、`{}` 这类元字符需要加 `-E`（扩展正则）才生效，且 `|`、`()` 在扩展正则下不需要转义。

| 元字符 | 含义 | 示例 |
|---|---|---|
| `.` | 匹配任意单个字符 | `grep "a.c" file.txt` |
| `*` | 匹配前一个字符 0 次或多次 | `grep "ab*" file.txt` |
| `^` | 匹配行首 | `grep "^function" file.js` |
| `$` | 匹配行尾 | `grep "return;$" file.js` |
| `[]` | 匹配括号内任一字符 | `grep "[0-9]" file.txt` |
| `[^]` | 匹配不在括号内的字符 | `grep "[^a-z]" file.txt` |
| `\|` | 或 | `grep "error\|warning" log.txt` |
| `\(\)` | 分组 | `grep "\(ab\)\+" file.txt` |
| `+` | 匹配前一个字符 1 次或多次（`-E`） | `grep -E "ab+" file.txt` |
| `?` | 匹配前一个字符 0 次或 1 次（`-E`） | `grep -E "ab?" file.txt` |
| `{n}` | 匹配 n 次（`-E`） | `grep -E "a{3}" file.txt` |
| `{n,}` | 匹配至少 n 次（`-E`） | `grep -E "a{3,}" file.txt` |
| `{n,m}` | 匹配 n 到 m 次（`-E`） | `grep -E "a{2,4}" file.txt` |
| `\|` | 或，不需要转义（`-E`） | `grep -E "error\|warning" log.txt` |
| `()` | 分组，不需要转义（`-E`） | `grep -E "(ab)+" file.txt` |

## sed：流式替换

sed 是流式（非交互式）编辑器，可以自动执行查找替换、删除、插入等编辑操作，非常适合在脚本中批量修改文件。

```bash
sed 's/old/new/g' file.txt                  # 把 file.txt 中所有 old 替换为 new，输出结果
sed -i 's/old/new/g' file.txt               # -i 表示直接修改原文件
sed -i 's/127.0.0.1/0.0.0.0/g' config.conf  # 把配置文件中的 IP 地址替换掉
```

## tr 与 echo

`tr` 从标准输入读取数据，进行字符映射、删除或压缩；`echo` 则在终端显示文本或变量。

```bash
echo "Hello" | tr 'a-z' 'A-Z'   # 小写转大写，输出 HELLO
echo "HELLO" | tr 'A-Z' 'a-z'   # 大写转小写
echo $PATH | tr ':' '\n'        # 把 PATH 中的冒号替换为换行符，每个路径独占一行
```

```bash
echo "Hello, World!"            # 直接输出一段文本
echo "当前用户是：$USER"          # 输出变量值，$USER 会被替换为当前用户名
echo -n "不换行输出"             # 输出后不换行，方便在同一行继续输出
echo -e "第一行\n第二行"          # -e 启用转义字符，\n 表示换行
```

## awk：按列分析

awk 是强大的文本分析工具，适合按列处理数据。先看最常用的用法：打印第 1 和第 3 列。

```bash
awk '{print $1, $3}' file.txt   # 打印第 1 和第 3 列
```

awk 内置变量：

| 变量 | 含义 |
|---|---|
| `$0` | 整行 |
| `$1` ~ `$n` | 第 1 到第 n 列 |
| `NR` | 行号 |
| `NF` | 列数 |
| `FS` | 列分隔符 |
| `OFS` | 输出分隔符 |

awk 自带条件判断和统计能力，例如按条件打标记、对某列求和并计算平均值：

```bash
awk '{if($3 > 100) print $1, "High"}' file.txt
awk '{sum+=$1} END {print sum}' file.txt
awk '{sum+=$1} END {print "Sum:", sum, "Avg:", sum/NR}' data.txt
```

## 其他文本工具

| 命令 | 说明 | 示例 |
|---|---|---|
| `sort` | 排序 | `sort -nr file.txt`（按数字逆序排序） |
| `uniq` | 去重，通常与 sort 连用 | `sort file.txt \| uniq -c`（统计重复次数） |
| `wc` | 统计行数、单词数、字节数 | `wc -l file.txt`（只统计行数） |
| `cut` | 按指定分隔符切割列 | `cut -d':' -f1 /etc/passwd`（以冒号分隔取第 1 列） |
| `tee` | 将 STDOUT 同时输出到文件和屏幕 | `ls \| tee output.txt` |

## 三剑客定位对比

| 工具 | 定位与适用场景 |
|---|---|
| grep | 过滤：按正则表达式匹配行，适合找特定字符串 |
| sed | 替换：流式按行编辑，适合批量替换文本 |
| awk | 分析：按列处理，自带变量和逻辑控制，适合统计、格式化输出 |

# 权限与用户管理

Linux 的文件和文件夹存在权限问题，找不到或无法操作文件往往是因为权限不足，需要用 `chmod` 等命令调整。

## 用户与组管理

| 命令 | 说明 | 示例 |
|---|---|---|
| `su` | Switch User，切换用户 | `su - root`（加 `-` 表示连环境变量一起切换） |
| `sudo` | Superuser Do，以管理员权限执行命令 | `sudo apt update` |
| `whoami` | 显示当前登录用户名 | `whoami` |
| `id` | 显示当前用户的 UID、GID 和所属组 | `id` |
| `useradd` | 创建新用户 | `useradd -m -s /bin/bash newuser`（`-m` 创建家目录） |
| `passwd` | 设置或修改用户密码 | `passwd newuser` |
| `userdel` | 删除用户 | `userdel -r newuser`（`-r` 连家目录一起删） |
| `usermod` | 修改用户属性 | `usermod -aG docker newuser`（把用户加入 docker 组） |

## 文件权限基础

`ls -l` 输出中的 `rwx r-x r--` 按“拥有者 / 同组 / 其他人”分成三段：拥有者可读可写可执行，同组可读可执行，其他人只读。修改权限与归属的命令：

```bash
chmod 755 script.sh          # 直接设定权限
chmod u+x script.sh          # 给拥有者加执行权限
chown user:group file.txt    # 修改文件拥有者和组
chgrp group file.txt         # 修改文件所属组
```

## 权限数字表示法

权限位可以用数字累加表示：读为 4、写为 2、执行为 1。

| 数字 | 权限 | 说明 |
|---|---|---|
| 4 | r | 读权限 |
| 2 | w | 写权限 |
| 1 | x | 执行权限 |
| 7 | rwx | 读 + 写 + 执行（4+2+1） |
| 6 | rw- | 读 + 写（4+2） |
| 5 | r-x | 读 + 执行（4+1） |
| 755 | rwxr-xr-x | 拥有者全权限，其他人读和执行（脚本/目录常用） |
| 644 | rw-r--r-- | 拥有者读写，其他人只读（普通文件常用） |
| 600 | rw------- | 只有拥有者可以读写（密钥文件常用） |
| 700 | rwx------ | 只有拥有者可以读写执行（私有脚本常用） |

> **小贴士：chmod 数字就是 1+2+4。** 1+2+4 = 7 表示“可读可写可执行”，三组数字分别对应拥有者、同组和其他人的权限，任意组合出 0~7。

# 进程与系统信息

## 进程管理

| 命令 | 说明 | 示例 |
|---|---|---|
| `ps` | 查看当前终端进程 | `ps aux`（查看系统所有进程） |
| `top` | 实时动态显示进程资源占用，也常用来监控 CPU、内存 | `top`；`htop`（更美观的替代品） |
| `kill` | 发送信号给进程 | `kill 1234`（默认 SIGTERM，15）；`kill -9 1234`（强制杀死，SIGKILL，9） |
| `killall` | 按进程名杀死所有同名进程 | `killall nginx` |
| `pkill` | 按模式匹配杀死进程 | `pkill -f "python my_script.py"` |
| `nohup` | 不挂断地运行命令，退出终端后继续运行 | `nohup python app.py > log.txt 2>&1 &` |
| `&` | 将命令放到后台运行 | `sleep 100 &` |
| `jobs` | 查看当前终端后台任务 | `jobs -l` |
| `fg` | 将后台任务调到前台 | `fg %1` |
| `bg` | 将挂起的任务放到后台 | `bg %1` |
| `pstree` | 以树形结构显示进程关系 | `pstree -p` |

## 系统信息与性能监控

| 命令 | 说明 | 示例 |
|---|---|---|
| `uname` | 查看系统内核信息 | `uname -a`（查看所有信息） |
| `hostname` | 查看主机名 | `hostname` |
| `uptime` | 查看系统运行时间、负载 | `uptime` |
| `free` | 查看内存使用情况 | `free -h`（人类可读格式） |
| `df` | 查看文件系统磁盘空间/分区使用情况 | `df -h` |
| `iostat` | 监控 CPU 和磁盘 IO | `iostat -x 1` |
| `iotop` | 监控哪个进程在读写磁盘 | `sudo iotop` |
| `vmstat` | 监控虚拟内存、进程、CPU | `vmstat 1` |
| `lscpu` | 查看 CPU 架构信息 | `lscpu` |
| `lsblk` | 查看块设备（硬盘分区）信息 | `lsblk` |
| `fdisk -l` | 查看磁盘分区表 | `sudo fdisk -l` |
| `mount` | 挂载文件系统 | `sudo mount /dev/sdb1 /mnt/usb` |
| `umount` | 卸载文件系统 | `sudo umount /mnt/usb` |

# 网络与软件包管理

## 网络命令

| 命令 | 说明 | 示例 |
|---|---|---|
| `ping` | 测试网络连通性 | `ping -c 4 baidu.com`（发 4 个包） |
| `ifconfig` / `ip` | 查看本机 IP 地址 | `ip addr show`（新语法） |
| `netstat` | 查看网络连接、端口、路由表 | `netstat -tlnp`（查看 TCP 监听端口） |
| `ss` | netstat 的现代替代品，更快 | `ss -tlnp`（查看 TCP 监听端口） |
| `curl` | 命令行 HTTP 客户端（发请求/下载文件） | `curl -O https://example.com/file.zip` |
| `wget` | 非交互式网络下载器 | `wget https://example.com/file.zip` |
| `scp` | 基于 SSH 的远程文件拷贝 | `scp local.txt user@ip:/remote/path/` |
| `ssh` | 远程安全登录主机 | `ssh user@192.168.1.100` |
| `nslookup` / `dig` | DNS 域名解析查询 | `nslookup baidu.com`；`dig baidu.com` |
| `traceroute` | 追踪数据包到目标主机的路由路径 | `traceroute baidu.com` |
| `nc` | Netcat，网络瑞士军刀（调试端口） | `nc -vz 192.168.1.100 80`（测试端口连通） |
| `lsof` | 查看打开的文件和端口占用 | `lsof -i :8080`（查看 8080 端口被谁占用） |

## 软件包管理器

不同发行版使用各自的包管理器，但命令职责一一对应：搜索、安装、卸载、更新源、升级所有包。

| 系统 | 包管理器 | 搜索 | 安装 | 卸载 | 更新源 | 更新所有 |
|---|---|---|---|---|---|---|
| Debian/Ubuntu | `apt` | `sudo apt search pkg` | `sudo apt install pkg` | `sudo apt remove pkg` | `sudo apt update` | `sudo apt upgrade` |
| CentOS/RHEL | `yum`/`dnf` | `sudo yum search pkg` | `sudo yum install pkg` | `sudo yum remove pkg` | `sudo yum makecache` | `sudo yum update` |
| Arch Linux | `pacman` | `sudo pacman -Ss pkg` | `sudo pacman -S pkg` | `sudo pacman -R pkg` | `sudo pacman -Sy` | `sudo pacman -Syu` |
| Alpine | `apk` | `sudo apk search pkg` | `sudo apk add pkg` | `sudo apk del pkg` | — | `sudo apk upgrade` |
| 源码编译 | — | — | `./configure && make && sudo make install` | — | — | 没有包管理器时的通用方法 |

# 重定向、管道与压缩

## 重定向与管道

重定向改变命令默认的输入输出方向，管道则把一个命令的 STDOUT 接到下一个命令的 STDIN。

| 符号 | 说明 | 示例 |
|---|---|---|
| `>` | 将 STDOUT 重定向到文件（覆盖原文件） | `echo "hello" > file.txt` |
| `>>` | 将 STDOUT 追加到文件 | `echo "world" >> file.txt` |
| `2>` | 将 STDERR 重定向到文件 | `command 2> error.log` |
| `&>` | 将 STDOUT 和 STDERR 都重定向到文件 | `command &> all.log` |
| `2>&1` | 将 STDERR 重定向到 STDOUT | `(command > out.log) 2>&1` |
| `<` | 将文件内容作为 STDIN | `sort < file.txt` |
| `<<` | Here Document，将输入行作为 STDIN | `cat << EOF > file.txt` |
| `\|` | 管道，连接前后命令 | `ps aux \| grep python` |
| `\| xargs` | 将管道传来的文本转为命令参数 | `find . -name "*.log" \| xargs rm -f` |
| `/dev/null` | 黑洞设备，丢弃所有输出 | `command > /dev/null 2>&1`（完全静默执行） |

## 压缩与解压

| 格式 | 打包/压缩命令 | 解压/查看命令 | 说明 |
|---|---|---|---|
| `.tar` | `tar -cvf file.tar dir/` | `tar -xvf file.tar` | 仅打包，不压缩 |
| `.gz` | `tar -czvf file.tar.gz dir/` | `tar -xzvf file.tar.gz` | 最常用的 Linux 压缩格式 |
| `.tgz` | 同 `.tar.gz` | 同 `.tar.gz` | `tar.gz` 的简写 |
| `.bz2` | `tar -cjvf file.tar.bz2 dir/` | `tar -xjvf file.tar.bz2` | 压缩率比 gz 高，速度慢 |
| `.xz` | `tar -cJvf file.tar.xz dir/` | `tar -xJvf file.tar.xz` | 压缩率最高，速度最慢 |
| `.zip` | `zip -r file.zip dir/` | `unzip file.zip` | 跨平台兼容（Windows 常用） |
| `.rar` | `rar a file.rar dir/` | `unrar x file.rar` | 需安装 rarlinux |
| 查看内容 | — | `tar -ztvf file.tar.gz` | 不解压直接查看包内文件列表 |
| 指定目录 | — | `tar -xzvf file.tar.gz -C /tmp/` | 解压到指定目录 |

# Shell 脚本编程

把变量、判断、循环、函数、数组组合进一个以 `#!/bin/bash` 开头的 `.sh` 文件，就成为一个可复用脚本，例如：

```bash
#!/bin/bash
# 变量 + 条件 + 循环的组合
name="Tom"
if [ -f /etc/passwd ]; then
    echo "用户文件存在，当前用户：$USER"
fi
for i in {1..3}; do
    echo "$i: hello, $name"
done
```

保存后用 `bash xxx.sh` 运行；也可以用 `chmod u+x xxx.sh` 加执行权限后直接 `./xxx.sh`，此时由 Shebang 指定的解释器来执行。

## 条件判断

| 语法 | 说明 | 示例 |
|---|---|---|
| `if-then` | 条件成立则执行，以 `fi` 结束 | `if [ $a -gt $b ]; then echo "a>b"; fi` |
| `if-else` | 成立走 `then`，否则走 `else` | `if [ $a -gt $b ]; then echo "a>b"; else echo "a<=b"; fi` |
| `if-elif-else` | 多条件分支 | `if [ $x -eq 1 ]; then ... elif [ $x -eq 2 ]; then ... else ... fi` |
| `test` 命令 | 等价于 `[ ]`，用于条件测试 | `test -f file.txt`；`[ -f file.txt ]` |
| 文件判断 | `-e` 存在、`-f` 普通文件、`-d` 目录、`-r` 可读、`-w` 可写、`-x` 可执行、`-s` 非空 | `[ -f /etc/passwd ]` |
| 字符串判断 | `-z` 空串、`-n` 非空串、`=` 相等、`!=` 不相等 | `[ "$str" = "hello" ]` |
| 整数比较 | `-eq` 等于、`-ne` 不等、`-gt` 大于、`-ge` 大于等于、`-lt` 小于、`-le` 小于等于 | `[ $age -ge 18 ]` |
| 逻辑与或 | `-a`（AND）、`-o`（OR） | `[ $a -gt 0 -a $a -lt 10 ]` |
| `&&` 和 `\|\|` | 短路逻辑，用于命令连接 | `[ -f file ] && echo "exists" \|\| echo "not found"` |
| `case` 语句 | 多分支匹配 | `case $choice in 1) echo "one";; *) echo "other";; esac` |

## 循环

| 循环类型 | 语法 | 说明 |
|---|---|---|
| for（列表） | `for i in 1 2 3; do echo $i; done` | 遍历空格分隔的列表 |
| for（范围） | `for i in {1..10}; do echo $i; done` | 生成 1 到 10 的序列 |
| for（步长） | `for i in {1..10..2}; do echo $i; done` | 1 到 10，步长为 2 |
| for（C 风格） | `for ((i=0; i<10; i++)); do echo $i; done` | 类似 C 语言的 for 循环 |
| for（文件） | `for file in *.txt; do rm $file; done` | 遍历当前目录所有 txt 文件 |
| while | `while [ $i -lt 10 ]; do i=$((i+1)); done` | 条件满足时一直执行 |
| until | `until [ $i -ge 10 ]; do i=$((i+1)); done` | 条件不满足时一直执行，满足则退出 |
| 循环控制 `break` | `break` | 跳出整个循环 |
| 循环控制 `continue` | `continue` | 跳过本次循环，进入下一次 |
| 遍历命令结果 | `for line in $(cat file.txt); do echo $line; done` | 将命令输出作为列表遍历 |
| 读取文件每行 | `while read line; do echo $line; done < file.txt` | 标准按行读取文件方式 |

## 函数

| 方式 | 语法 | 说明 |
|---|---|---|
| 定义函数 | `func_name() { commands; }` | 最简单的定义方式 |
| function 关键字 | `function func_name { commands; }` | 另一种写法 |
| 带参数的函数 | 函数内用 `$1`、`$2` 接收参数 | `func() { echo "参数1: $1, 参数2: $2"; }` |
| 调用函数 | `func_name arg1 arg2` | 传参调用 |
| 返回值 | `return` | 返回状态码（0~255），例如 `return 0` |
| 返回字符串 | `echo` | 用 echo 输出，调用时用 `$(func)` 捕获：`result=$(get_name)` |
| 局部变量 | `local` | `local var=10`，避免污染全局变量 |
| 递归 | `factorial() { if [ $1 -le 1 ]; then echo 1; else echo $(( $1 * $(factorial $(( $1 - 1 ))) )); fi }` | 阶乘计算 |

## 数组

| 操作 | 语法 | 示例 |
|---|---|---|
| 定义数组 | `arr=(value1 value2 "value3")` | 空格分隔 |
| 按索引赋值 | `arr[0]="a"`；`arr[1]="b"` | — |
| 获取单个元素 | `${arr[index]}` | `echo ${arr[0]}` |
| 获取所有元素 | `${arr[@]}` 或 `${arr[*]}` | `echo ${arr[@]}` |
| 获取数组长度 | `${#arr[@]}` | `len=${#arr[@]}` |
| 获取单个元素长度 | `${#arr[index]}` | `len=${#arr[0]}` |
| 遍历数组 | `for i in "${arr[@]}"; do echo $i; done` | 必须加双引号防止空格拆分 |
| 切片 | `${arr[@]:start:length}` | `echo ${arr[@]:1:2}`（从索引 1 开始取 2 个） |
| 追加元素 | `arr+=(new_element)` | — |
| 删除元素 | `unset arr[index]` | `unset arr[1]` |
| 关联数组（字典） | `declare -A dict`；`dict["key1"]="val1"` | 需用 `declare -A` 声明 |

# cron 定时任务与高阶技巧

## cron 定时任务

cron 是 Linux 的定时任务工具，用 `crontab` 管理。

| 操作 | 命令/语法 | 说明 |
|---|---|---|
| 编辑定时任务 | `crontab -e` | 打开当前用户的定时任务编辑器 |
| 查看定时任务 | `crontab -l` | 列出当前用户的所有定时任务 |
| 删除定时任务 | `crontab -r` | 清空所有定时任务（危险） |
| cron 表达式格式 | `* * * * * command` | 分 时 日 月 周 命令 |
| 每 5 分钟执行 | `*/5 * * * * /path/to/script.sh` | — |
| 每天凌晨 1 点执行 | `0 1 * * * /path/to/backup.sh` | — |
| 每周一上午 8 点执行 | `0 8 * * 1 /path/to/weekly.sh` | — |
| 每月 1 号 0 点执行 | `0 0 1 * * /path/to/monthly.sh` | — |
| 日志重定向 | `*/5 * * * * /path/to/script.sh > /dev/null 2>&1` | 定时任务必须重定向，否则会发邮件给系统用户 |

cron 表达式特殊符号：

| 符号 | 含义 |
|---|---|
| `*` | 任意 |
| `/` | 间隔 |
| `-` | 范围 |
| `,` | 列表 |

## 高阶技巧

| 技巧 | 命令/说明 | 示例 |
|---|---|---|
| `&&` 链式执行 | 前一个成功才执行后一个 | `cd /dir && rm -rf *`（防止目录不存在时误删） |
| `\|\|` 链式执行 | 前一个失败才执行后一个 | `cd /dir \|\| mkdir -p /dir` |
| 子 Shell | `( command )`，在子 Shell 中执行，不影响当前 Shell（`cd` 无效） | `(cd /tmp && ls)`（执行完还在原目录） |
| 命令替换 | `$(command)` 或 `` `command` ``，把命令输出赋值给变量 | `today=$(date +%Y-%m-%d)` |
| 算术运算 | `$(( expression ))` 或 `expr` | `sum=$((a + b))`；`expr 5 + 3` |
| 字符串截取 | `${var:起始:长度}` | `${str:0:5}` |
| 默认值 | `${var:-default}` | 如果 var 未设置或为空，返回 default |
| xargs 配合 find | `find . -name "*.log" \| xargs grep "ERROR"` | 处理带空格的文件名：`find . -print0 \| xargs -0` |
| 并行执行 | `xargs -P 4`，让 xargs 同时运行 4 个进程 | `cat urls.txt \| xargs -P 4 -I {} curl {}` |
| trap 捕获信号 | `trap 'cleanup' EXIT` | INT（Ctrl+C）、TERM（kill）、EXIT（脚本退出时） |
| 调试脚本 | `bash -x script.sh` | 逐行打印执行的命令（排错神器） |
| `set -e` | 遇到错误立即退出脚本 | 写在脚本开头，任何命令返回非 0 就终止 |
| `set -o pipefail` | 管道中任何一个命令失败，整个管道返回失败 | `set -e; set -o pipefail` |
