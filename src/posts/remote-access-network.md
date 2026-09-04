---
title: 远程连接与网络访问
date: 2026-04-01
category: 学习记录
tags: ["ssh", "vnc", "smb", "nfs", "内网穿透", "vpn"]
excerpt: SSH 远程登录与密钥、VNC 图形桌面、SMB/NFS 文件共享、内网穿透与异地组网
---
# SSH 远程连接

SSH（Secure Shell）是远程登录 Linux 服务器最常用的方式，除了基本登录，还支持密钥认证和端口转发。下面从服务端安装开始，一直到常用进阶用法。

## 安装并启用 sshd

在服务器上安装对应服务：

```bash
sudo apt install openssh-server -y
```

打开服务：

```bash
sudo systemctl start ssh
```

设置为开机自启：

```bash
sudo systemctl enable ssh
```

## 密码登录

基本语法为 `ssh 用户名@服务器地址`，例如：

```bash
ssh root@192.168.1.100
```

## 免密登录：SSH 密钥

为了免密登录（比如用于 gitee 等平台），可以先生成一对密钥。密钥类型首选 Ed25519，备用 `-t rsa -b 4096`。

语法：

```bash
ssh-keygen -t ed25519 -C "your_email@example.com"
```

参数说明：

- `ssh-keygen`：生成密钥的命令
- `-t ed25519`：指定密钥类型为 Ed25519
- `-C`：添加注释（Comment），通常用邮箱标识

执行过程中会弹出几次选择，用于设置密钥的保存位置以及密钥密码（passphrase）。

密钥文件默认都存放在 `~/.ssh/` 目录下：

```bash
# 查看生成的文件
ls -la ~/.ssh/
# 查看公钥
cat ~/.ssh/id_ed25519.pub
# 查看私钥（永远不要分享）
cat ~/.ssh/id_ed25519
```

## 端口转发

SSH 端口转发分为两种方向：

```bash
# 远程端口映射到本地：本地端口:目标地址:目标端口
ssh -L 本地端口:目标地址:目标端口 跳板机用户@跳板机地址
# 本地端口映射到远程：远程端口:本地地址:本地端口
ssh -R 远程端口:本地地址:本地端口 用户@公网服务器
```

其中跳板机就是有公网的服务器，用来进行流量转接；不需要跳板机时直接填写目标 IP 即可。
# VNC 图形远程桌面

VNC 是适用于 Linux 系统的图形远程桌面连接方案。

## 安装与启动

1. 安装服务：`sudo apt install tigervnc-standalone-server`

2. 设置密码：`vncpasswd`（会询问是否设置只读密码，选 n 就是不设置「只有只读权限的密码」登录）

3. 启动 VNC 服务（常用启动方式，可指定分辨率）：

```bash
vncserver :1 -geometry 1366x768 -localhost no
```

- `:1`：指定虚拟桌面序号（端口默认为 5900+序号；`:0` 通常表示物理桌面）
- `-geometry`：设置分辨率
- `-localhost no`：设置为允许远程连接（默认不允许，只能本机连接）

4. 查看运行中的会话：`vncserver -list`
# 局域网文件共享：SMB 与 NFS

## SMB 与 NFS 协议对比

SMB 和 NFS 是网络文件共享领域最经典的两个协议，可以把它们理解成解决同一个问题（网络共享文件）。区别在于：SMB 是 Windows 世界的“社交达人”，而 NFS 是 Linux 世界的“效率极客”。

## Windows 搭建 SMB 服务端

首先在“打开或关闭 Windows 功能”里打开 SMB 服务，然后可以在“网络共享中心”中进行设置：选择一个需要共享的文件夹，点击“属性”中的“共享”，创建一个用于登录的账户并设置权限即可（Windows 对 SMB 的支持和生态极好）。

## Linux 挂载 SMB 共享

先安装 SMB 客户端：

```bash
sudo apt install cifs-utils
```

然后进行挂载：

```bash
sudo mount -t cifs //192.168.126.1/share /mnt/windows_share -o username=chen,password=123456
```

- `//192.168.126.1/share`：服务器 IP 和共享的文件夹名字
- `/mnt/windows_share`：本地创建的、用于被挂载的文件夹
# 内网穿透：localtunnel

localtunnel 是一款通过 npm 安装的内网穿透工具，可临时把本机端口开放到公网，原理是使用第三方服务器进行流量转接。

```bash
# 1. 安装 js 库
npm install -g localtunnel
# 2. 开放本机端口到公网
lt --port 8000 --subdomain yourname
```

其中 `--port 8000` 指定要开放的本地端口，`--subdomain yourname` 可自定义分配的域名前缀。
# 异地组网：VPN

## Tailscale

Linux 下安装：

```bash
curl -fsSL https://tailscale.com/install.sh | sh
```

## EasyTier

Linux 下安装，先安装需要的解压工具：

```bash
sudo apt install unzip -y
```

再下载并运行安装脚本：

```bash
wget -O /tmp/easytier.sh "https://raw.githubusercontent.com/EasyTier/EasyTier/main/script/install.sh" && sudo bash /tmp/easytier.sh install --gh-proxy https://ghfast.top/
```
