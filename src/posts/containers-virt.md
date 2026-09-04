---
title: Docker 与虚拟化：Dockerfile / Compose / QEMU / WSL
date: 2026-04-10
category: 学习记录
tags: ["docker", "dockerfile", "yaml", "qemu", "wsl"]
excerpt: 虚拟机与容器、Docker 安装与命令、Dockerfile、Compose 的 YAML、QEMU 与 WSL
---
# 虚拟机与容器概览

布置虚拟机（搭建虚拟化环境）有很多种方法，主要方案如下：

| 方案 | 说明 |
|---|---|
| Docker | 用镜像进行容器构建，内核极小 |
| Hyper-V | Windows 专业版使用的虚拟化软件 |
| VMware | 最流行的虚拟机软件，唯一支持外接设备的虚拟机 |
| WSL | Windows 内置的 Linux 子系统，与 Windows 系统交互性好；需要在控制面板“启用或关闭 Windows 功能”里打开“适用于 Linux 的 Windows 子系统”等功能 |

Docker 属于容器技术（共享宿主机内核），Hyper-V / VMware 属于传统虚拟机，WSL 则是 Windows 下的轻量级 Linux 子系统，它们各有适用场景，下文分别介绍 Docker、QEMU 虚拟机与 WSL。

# Docker：安装与国内镜像源

## 在 Linux 中安装

拉取 Docker 官方安装脚本：

```bash
curl -fsSL https://get.docker.com -o get-docker.sh
```

运行脚本进行安装：

```bash
sudo sh get-docker.sh
```

安装完成后，可把当前用户加入 `docker` 组，之后免 sudo 使用 docker：

```bash
sudo usermod -aG docker $USER
```

## 在 Windows 中安装

在 Docker 官网下载 Docker 的 Windows 桌面版（Docker Desktop for Windows），或者试试 Windows 的包管理工具（如 winget）来安装。

## 镜像拉取与国内镜像源

镜像拉取默认使用国外源，国内往往无法直接拉取，所以需要指定国内镜像源：把国内镜像源地址加在镜像名字前面即可，例如：

```bash
docker pull docker.1ms.run/ubuntu:latest
```

# Dockerfile 编写

## 核心概念

| 概念 | 说明 | 示例 |
|---|---|---|
| Dockerfile | 文本文件，包含构建 Docker 镜像的全部指令 | `vim Dockerfile` |
| 镜像 | 只读模板，包含运行应用所需的一切（代码、运行时、库、配置） | `docker build -t myapp .` |
| 容器 | 镜像的运行实例，可读可写 | `docker run -it myapp` |
| 层 | 每个指令创建一个镜像层，层会缓存复用 | `FROM`、`RUN`、`COPY` 各自一层 |
| 基础镜像 | 其他镜像的起点，所有 Dockerfile 必须以 `FROM` 开头 | `FROM ubuntu:22.04` |
| 上下文 | `COPY` / `ADD` 的资源来源路径（通常用 `.` 表示当前目录） | `docker build -t app .` |
| 多阶段构建 | 多个 `FROM`，最终只保留需要的产物（大幅减小镜像体积） | `FROM golang AS builder`<br>`FROM alpine AS final` |

## Dockerfile 指令详解

| 指令 | 说明 | 示例 |
|---|---|---|
| `FROM` | 指定基础镜像（必须写在第一行，除了 `ARG`） | `FROM ubuntu:22.04` |
| `FROM:AS` | 给构建阶段命名（多阶段构建用） | `FROM node:18 AS builder` |
| `RUN` | 执行 Shell 命令（安装依赖、编译等） | `RUN apt-get update && apt-get install -y gcc` |
| `COPY` | 从宿主机复制文件到镜像中 | `COPY . /app` |
| `ADD` | 复制文件（支持 URL 下载和自动解压 tar 包） | `ADD app.tar.gz /opt/` |
| `WORKDIR` | 设置工作目录（后续 `RUN` / `COPY` / `CMD` 的相对路径基准） | `WORKDIR /app` |
| `CMD` | 容器启动后默认执行的命令（可被 `docker run` 覆盖） | `CMD ["python3", "app.py"]` |
| `ENTRYPOINT` | 容器启动入口点（不会被覆盖，和 `CMD` 配合使用） | `ENTRYPOINT ["python3"]` |
| `EXPOSE` | 声明容器运行时监听的端口（文档性质） | `EXPOSE 8080` |
| `ENV` | 设置环境变量（贯穿整个镜像构建和运行过程） | `ENV APP_ENV=production` |
| `ARG` | 构建时变量（只在构建阶段有效，运行时消失） | `ARG VERSION=1.0` |
| `LABEL` | 给镜像添加元数据（作者、版本、描述等） | `LABEL maintainer="dev@example.com"` |
| `VOLUME` | 声明挂载卷（数据持久化目录） | `VOLUME /data` |
| `USER` | 指定后续指令和容器运行时的用户 | `USER appuser` |
| `HEALTHCHECK` | 容器健康检查命令 | `HEALTHCHECK --interval=30s CMD curl -f http://localhost/ \|\| exit 1` |
| `ONBUILD` | 触发器指令（当此镜像被其他镜像 `FROM` 时才执行） | `ONBUILD COPY . /app` |
| `STOPSIGNAL` | 设置停止容器时发送的系统信号 | `STOPSIGNAL SIGTERM` |
| `SHELL` | 更改后续 `RUN` / `CMD` 使用的默认 Shell | `SHELL ["/bin/bash", "-c"]` |
| `MAINTAINER` | 设置维护者信息（已废弃，用 `LABEL` 替代） | `MAINTAINER dev@example.com` |

## CMD 与 ENTRYPOINT 的区别与配合

| 场景 | CMD 写法 | ENTRYPOINT 写法 | 行为说明 |
|---|---|---|---|
| 单独使用 CMD | `CMD ["python3", "app.py"]` | - | `docker run` 可覆盖命令：`docker run myimg bash`（不执行 python3） |
| 单独使用 ENTRYPOINT | - | `ENTRYPOINT ["python3", "app.py"]` | `docker run` 的参数追加到后面：`docker run myimg arg1`（变成 python3 app.py arg1） |
| 两者配合 | `CMD ["app.py"]` | `ENTRYPOINT ["python3"]` | CMD 作为 ENTRYPOINT 的默认参数，`docker run myimg` 等价于 python3 app.py，`docker run myimg test.py` 变成 python3 test.py |
| 覆盖规则 | CMD 会被 `docker run` 后面的命令覆盖 | CMD 不会被覆盖（除非 `docker run --entrypoint`） | - |

## COPY 与 ADD 对比

| 对比项 | COPY | ADD |
|---|---|---|
| 基本复制 | `COPY src dest` | `ADD src dest` |
| URL 下载 | 不支持 | 支持（`ADD https://xxx/file.tar.gz /opt/`） |
| 自动解压 tar | 不支持 | 支持（`ADD file.tar.gz /opt/` 自动解压） |
| 推荐使用 | 大部分情况用 COPY 就够了 | 只在需要解压/下载时使用 |
| 可读性 | COPY 语义清晰，所见即所得 | ADD 行为较多，容易踩坑 |
| 性能 | 更直接、更快 | 额外处理（解压/下载），稍慢 |

## RUN 的两种写法

| 写法 | 说明 | 示例 |
|---|---|---|
| Shell 格式 | `RUN <命令>`，默认用 `/bin/sh -c` 执行 | `RUN apt-get update && apt-get install -y vim` |
| Exec 格式 | `RUN ["可执行文件", "参数1", "参数2"]` | `RUN ["pip", "install", "flask"]` |
| 推荐 Shell 格式 | 适合需要链式命令的场景 | `RUN apt-get update && apt-get install -y gcc && rm -rf /var/lib/apt/lists/*` |
| 推荐 Exec 格式 | 适合指定解释器的场景 | `RUN ["python3", "-m", "pip", "install", "-r", "requirements.txt"]` |
| 合并 RUN 命令 | 减少镜像层数，把多条命令用 `&&` 连成一条 | `RUN apt-get update && apt-get install -y gcc make && rm -rf /var/lib/apt/lists/*` |
| 为什么要清理缓存 | apt 缓存如果不清理，镜像层会白白多出几百 MB | `rm -rf /var/lib/apt/lists/*` |

## EXPOSE 与端口映射

| 指令 | 说明 | 示例 |
|---|---|---|
| `EXPOSE` | 声明容器监听的端口（仅文档性质，不自动发布） | `EXPOSE 8080` |
| `EXPOSE` 多端口 | 声明多个端口 | `EXPOSE 80 443 8080` |
| `EXPOSE` 协议 | 指定协议类型 | `EXPOSE 53/udp` |
| `docker run -p` | 将容器端口映射到宿主机端口（必须手动映射才能外部访问） | `docker run -p 8080:80 myapp`（宿主 8080 到容器 80） |
| `docker run -P` | 随机映射所有 EXPOSE 的端口到宿主机高位端口 | `docker run -P myapp` |

## ENV 与 ARG 对比

| 对比项 | ENV | ARG |
|---|---|---|
| 生效范围 | 构建阶段 + 运行阶段都有效 | 仅构建阶段有效 |
| `docker run` 能看到 | 环境变量会被继承到容器中 | 构建完就消失 |
| `docker run -e` 覆盖 | 可以覆盖 | 无法覆盖 |
| 构建时传参 | - | `docker build --build-arg VERSION=1.0` |
| 默认值设置 | `ENV APP_ENV=prod` | `ARG VERSION=latest` |
| 多行设置 | `ENV A=1 B=2` 或 `ENV A=1`<br>`ENV B=2` | `ARG A=1`<br>`ARG B=2` |
| 使用场景 | 数据库密码、端口等运行时需要的环境变量 | 版本号、构建选项等仅在构建时用的变量 |
| 安全注意 | 不要把密码写死在 ENV 中，用 `docker run -e` 或 secrets 传入 | 同理 |

## VOLUME 数据持久化

| 指令 | 说明 | 示例 |
|---|---|---|
| `VOLUME` | 声明容器内的数据目录（容器删除后数据不丢失） | `VOLUME /var/lib/mysql` |
| `VOLUME` 多目录 | 声明多个挂载目录 | `VOLUME ["/data", "/logs"]` |
| `docker run -v` | 运行时绑定挂载宿主机目录到容器 | `docker run -v /host/path:/container/path myapp` |
| `docker run --mount` | 更高级的挂载方式（支持类型区分） | `docker run --mount type=bind,src=/host,dst=/container myapp` |
| 匿名卷 | 不指定宿主机路径，Docker 自动分配目录 | `docker run -v /var/lib/mysql myapp` |
| 命名卷 | 用名称管理卷（推荐，比匿名卷好管理） | `docker volume create mydata`<br>`docker run -v mydata:/data myapp` |

## WORKDIR 与 USER

| 指令 | 说明 | 示例 |
|---|---|---|
| `WORKDIR` | 设置工作目录（不存在会自动创建） | `WORKDIR /app/src` |
| 多次 WORKDIR | 累加路径 | `WORKDIR /app`<br>`WORKDIR src`（最终是 /app/src） |
| 没有 WORKDIR | 默认工作目录是根目录 `/` | COPY 和 RUN 都在 `/` 下操作 |
| `USER` | 切换运行用户（提升安全性，不用 root 运行） | `USER appuser` |
| 创建用户 | 需要先在 RUN 中创建用户 | `RUN useradd -m -s /bin/bash appuser` |
| `USER 1000:1000` | 直接用 UID:GID 指定用户（无需创建） | `USER 1000:1000` |
| 安全性建议 | 生产环境绝不要用 root 运行容器 | `WORKDIR /app`<br>`USER appuser` |

## LABEL 元数据

| 语法 | 说明 | 示例 |
|---|---|---|
| 单行 LABEL | `LABEL key="value"` | `LABEL maintainer="dev@example.com"` |
| 多行 LABEL | `LABEL key1="v1" key2="v2"` | `LABEL version="1.0" description="My App"` |
| 查看 LABEL | `docker inspect` 查看镜像的 LABEL | `docker inspect myapp` |
| 常用元数据 | 版本、描述、作者、源码地址 | `LABEL version="1.0" source="https://github.com/myapp"` |

## HEALTHCHECK 健康检查

| 参数 | 说明 | 默认值 |
|---|---|---|
| `--interval` | 检查间隔 | 30s |
| `--timeout` | 单次检查超时时间 | 30s |
| `--start-period` | 容器启动后等待多久开始检查（给应用启动时间） | 0s |
| `--retries` | 连续失败多少次判定为 unhealthy | 3 |
| CMD 格式 | 执行健康检查的命令（exit 0 = healthy，非 0 = unhealthy） | `HEALTHCHECK --interval=30s --timeout=3s --retries=3 CMD curl -f http://localhost:8080/health \|\| exit 1` |
| `NONE` | 禁用健康检查（在 FROM 父镜像中关闭） | `HEALTHCHECK NONE` |

## ONBUILD 触发器

| 语法 | 说明 | 示例 |
|---|---|---|
| `ONBUILD COPY` | 当此镜像被别人 FROM 时，自动执行 COPY | `ONBUILD COPY . /app` |
| `ONBUILD RUN` | 当此镜像被别人 FROM 时，自动执行 RUN | `ONBUILD RUN npm install` |
| 使用场景 | 制作基础镜像供团队使用 | `ONBUILD COPY . /app`<br>`ONBUILD RUN make` |
| 查看触发器 | `docker inspect` 可以看到触发的 ONBUILD 指令 | `docker inspect mybase` |

## 多阶段构建（减小镜像体积的利器）

| 概念 | 说明 | 示例 |
|---|---|---|
| 问题 | 编译阶段需要 gcc/make 等工具，但运行时不需要，镜像臃肿 | `FROM golang:1.21`<br>`RUN go build -o app`<br># 镜像 1.2GB |
| 方案 | 第一阶段编译，第二阶段只复制编译产物到干净镜像 | `FROM golang:1.21 AS builder`<br>`RUN go build -o app`<br>`FROM alpine:3.18`<br>`COPY --from=builder /app/app /app/app` |
| `--from=阶段名` | 从指定构建阶段复制文件 | `COPY --from=builder /build/mygame /usr/local/bin/mygame` |
| `--from=0` | 用编号引用阶段（0 是第一个 FROM） | `COPY --from=0 /app/config.yaml /etc/app/config.yaml` |
| `--from=镜像名` | 直接从其他已有镜像复制文件（不需要在 Dockerfile 中定义） | `COPY --from=nginx:latest /etc/nginx/nginx.conf /etc/nginx/` |

## .dockerignore 文件

| 作用 | 说明 | 示例 |
|---|---|---|
| 作用 | 排除不需要的文件进入构建上下文（类似 .gitignore） | `vim .dockerignore` |
| 减少构建上下文 | 不排除 node_modules/.git 等，构建会非常慢 | `.git`<br>`node_modules`<br>`__pycache__`<br>`*.pyc`<br>`.env`<br>`Dockerfile` |
| 常用排除项 | 版本控制、依赖目录、临时文件、敏感文件 | `.git`<br>`.gitignore`<br>`node_modules`<br>`venv`<br>`*.log`<br>`.env`<br>`.dockerignore` |
| 为什么重要 | 上下文越小，COPY 越快，构建越快 | docker build 会先把上下文全部发给 daemon |

## 实战模板

Python 应用：

```dockerfile
FROM python:3.11-slim AS base
WORKDIR /app
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
COPY . .
EXPOSE 8080
HEALTHCHECK --interval=30s CMD curl -f http://localhost:8080/health || exit 1
CMD ["python3", "app.py"]
```

Node.js 应用（多阶段）：

```dockerfile
FROM node:18-alpine AS builder
WORKDIR /app
COPY package*.json ./
RUN npm ci && npm run build

FROM node:18-alpine AS final
WORKDIR /app
COPY --from=builder /app/dist ./dist
COPY --from=builder /app/node_modules ./node_modules
EXPOSE 3000
CMD ["node", "dist/server.js"]
```

C++ 应用（多阶段编译）：

```dockerfile
FROM gcc:12 AS builder
WORKDIR /build
COPY . .
RUN make -j4

FROM ubuntu:22.04 AS final
RUN apt-get update && apt-get install -y --no-install-recommends libstdc++6 && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=builder /build/mygame .
RUN useradd -m gamer
USER gamer
EXPOSE 8080
CMD ["./mygame"]
```

Go 应用：

```dockerfile
FROM golang:1.21-alpine AS builder
WORKDIR /app
COPY go.mod go.sum ./
RUN go mod download
COPY . .
RUN CGO_ENABLED=0 go build -o myapp .

FROM alpine:3.18
RUN apk --no-cache add ca-certificates
WORKDIR /app
COPY --from=builder /app/myapp .
EXPOSE 8080
CMD ["./myapp"]
```

静态网站（Nginx）：

```dockerfile
FROM nginx:alpine
COPY --from=builder /app/dist /usr/share/nginx/html
COPY nginx.conf /etc/nginx/conf.d/default.conf
EXPOSE 80
CMD ["nginx", "-g", "daemon off;"]
```
# YAML 与 Docker Compose

Docker Compose 的多容器编排配置、CI/CD 流水线、Kubernetes 资源定义等都用 YAML 编写，所以整理一下 YAML 语法。

## YAML 基本语法与核心规则

| 规则 | 说明 | 示例 |
|---|---|---|
| 键值对 | 基本单位，用冒号 + 空格分隔 | `name: 植物大战僵尸` |
| 缩进 | 用空格缩进表示层级关系（严禁用 Tab） | 见下方“对象/映射写法” |
| `#` | 单行注释 | `# 这是注释` |
| `---` | 文档起始标记（可选，分隔多个文档） | `---` |
| `...` | 文档结束标记（可选） | `...` |
| 严格大小写 | YAML 中 true ≠ True ≠ TRUE（布尔值只有 true/false 生效） | `is_alive: true` |
| 不允许 Tab 缩进 | 混用 Tab 和空格会导致解析失败 | 必须全部用空格 |
| 冒号后必须有空格 | `key: value`（正确），`key:value`（报错） | `key: value` |
| 列表项用 `-` 加空格 | 列表每一项前面加横杠和空格 | `- 豌豆射手`<br>`- 向日葵` |
| 字符串 | 一般不需要引号，有特殊字符时用引号 | `msg: "hello: world"` |

## 数据类型

| 类型 | 说明 | 示例 |
|---|---|---|
| 字符串 | string，不需要引号，有空格建议加引号 | `name: 张三`；`msg: "hello world"` |
| 整数 | 支持十进制、十六进制（0x）、八进制（0o）、二进制（0b） | `port: 8080`；`hex: 0xFF` |
| 浮点数 | 支持科学计数法 | `price: 9.99`；`big: 1.2e10` |
| 布尔值 | true / false（注意大小写，True/False 在 1.2 版本也是布尔值） | `debug: true`；`enabled: false` |
| null / Null / ~ | 空值 | `missing: null`；`empty: ~` |
| 时间 | ISO 8601 格式时间 | `created: 2024-01-15T10:30:00+08:00` |
| 日期 | YYYY-MM-DD 格式 | `birthday: 2000-01-01` |
| 二进制 | `!!binary` 标签 | `avatar: !!binary \|` |

## 三种数据结构

| 结构 | 说明 | 示例 |
|---|---|---|
| 对象/映射 | 键值对集合（类似 Python 字典 / JS 对象 / JSON 对象） | `name: 张三`<br>`age: 18` |
| 数组/列表 | 有序元素集合（用 `-` 开头） | `fruits:`<br>`  - 苹果`<br>`  - 香蕉`<br>`  - 橙子` |
| 标量 | 单个的不可再分的值 | `port: 8080` |

## 对象与映射写法

普通写法（每行一个键值对，缩进表示层级）：

```yaml
name: 张三
age: 18
city: 北京
```

行内写法（写在一行，用花括号，类似 JSON）：

```yaml
person: {name: 张三, age: 18, city: 北京}
```

嵌套对象（对象里面套对象）：

```yaml
database:
  host: localhost
  port: 3306
  credentials:
    username: root
    password: linux
```

多行键（键名非常复杂时用 `?` 声明）：

```yaml
? >
  这是多行键名
: 这是值
```

## 数组与列表写法

普通写法（每项一行，用 `-` 开头）：

```yaml
plants:
  - 豌豆射手
  - 向日葵
  - 坚果墙
```

行内写法（写在一行，用方括号）：

```yaml
colors: [red, green, blue]
```

对象数组（数组中的每个元素是对象）：

```yaml
plants:
  - name: 豌豆射手
    cost: 100
  - name: 向日葵
    cost: 50
```

嵌套数组（数组中套数组）：

```yaml
matrix:
  - [1, 2, 3]
  - [4, 5, 6]
```

## 多行字符串

YAML 用 `|`（字面量模式，保留换行）和 `>`（折叠模式，把换行折成空格）来表示多行字符串，后面的 `-` / `+` 控制是否保留末尾空行。

`|`：保留换行和末尾空行：

```yaml
description: |
  第一行
  第二行
  第三行
```

`|-`：保留换行，去除末尾空行：

```yaml
description: |-
  第一行
  第二行
```

`|+`：保留换行，保留末尾所有空行：

```yaml
description: |+
  第一行
  第二行
```

`>`：折叠换行为空格（末尾的换行保留为一个换行）：

```yaml
description: >
  这是一段
  很长的文字
  自动合并
```

`>-`：折叠换行，去除末尾换行：

```yaml
description: >-
  这是一段很长的文字
```

`>+`：折叠换行，保留末尾所有空行：

```yaml
description: >+
  这是一段很长的文字
```

双引号：支持 `\n`、`\t` 等转义字符：

```yaml
description: "hello\tworld\n换行"
```

单引号：不解析任何转义字符（所见即所得）：

```yaml
description: 'hello\tworld\n'  # 原样输出 \t 和 \n
```

## 锚点与别名（复用重复内容，DRY）

- `&锚点名`：定义锚点，标记一段内容，供后面引用。
- `*别名名`：引用锚点，相当于复制粘贴那段内容。
- `<<`：合并键，把引用的内容合并到当前位置，继承后还可以覆盖或新增自己的键值对。

定义锚点：

```yaml
defaults: &defaults
  hp: 100
  speed: 5
```

引用锚点：

```yaml
plant_a:
  <<: *defaults
  name: 豌豆
```

完整示例：定义公共配置，多处复用：

```yaml
common: &common
  port: 8080
  host: localhost

dev:
  <<: *common
  env: dev

prod:
  <<: *common
  env: prod
  port: 443
```

## 类型强制转换（用 !! 标签指定类型）

| 标签 | 说明 | 示例 |
|---|---|---|
| `!!str` | 强制转为字符串 | `num: !!str "123"`（本应是数字，强制为字符串） |
| `!!int` | 强制转为整数 | `port: !!int "8080"` |
| `!!float` | 强制转为浮点数 | `price: !!float "9.99"` |
| `!!bool` | 强制转为布尔值 | `flag: !!bool "true"` |
| `!!null` | 强制转为空值 | `val: !!null "anything"` |
| `!!set` | 强制转为集合（YAML 1.1 支持） | `tags: !!set {a, b, c}` |

## YAML 与 JSON 对比

| 区别 | JSON | YAML |
|---|---|---|
| 扩展名 | .json | .yaml / .yml |
| 注释 | 不支持注释 | 支持 `#` 注释 |
| 引号 | 键和字符串值必须用双引号 | 通常不需要引号 |
| 缩进 | 不依赖缩进，用花括号/方括号 | 严格依赖空格缩进 |
| 可读性 | 机器友好，人不友好 | 人友好，极其直观 |
| 数据类型 | 基本类型 | 丰富（时间、日期、多行字符串、锚点等） |
| 重复内容 | 不支持复用 | 支持锚点 `&` 和别名 `*` 复用 |
| 多文档 | 不支持 | 支持用 `---` 分隔多个文档 |
| 兼容性 | 是 YAML 的子集 | 合法的 JSON 基本也是合法的 YAML |

## 常见 YAML 配置文件模板

Docker Compose 示例：

```yaml
version: '3.8'
services:
  web:
    image: nginx:latest
    ports:
      - "80:80"
    volumes:
      - ./html:/usr/share/nginx/html
    environment:
      - NODE_ENV=production
    depends_on:
      - db
  db:
    image: mysql:5.7
    environment:
      MYSQL_ROOT_PASSWORD: linux
      MYSQL_DATABASE: game_db
    volumes:
      - db_data:/var/lib/mysql
    ports:
      - "3306:3306"
volumes:
  db_data:
```

GitLab CI 示例：

```yaml
stages:
  - build
  - test
  - deploy
variables:
  APP_NAME: my_game
  BUILD_DIR: build
build_job:
  stage: build
  script:
    - make -j4
    - ls -la ${BUILD_DIR}
  artifacts:
    paths:
      - ${BUILD_DIR}/
test_job:
  stage: test
  script:
    - ./run_tests.sh
deploy_job:
  stage: deploy
  script:
    - ./deploy.sh
  only:
    - main
```

应用配置文件示例：

```yaml
app:
  name: 植物大战僵尸
  version: 1.0.0
  debug: true
server:
  host: 0.0.0.0
  port: 8080
database:
  type: mysql
  host: localhost
  port: 3306
  name: game_db
  username: root
  password: linux
logging:
  level: info
  file: logs/game.log
  max_size: 100MB
  backup_count: 5
```

## 各语言解析 YAML 的常用库

| 语言 | 库 | 安装命令 |
|---|---|---|
| Python | PyYAML | `pip install pyyaml` |
| Python | ruamel.yaml（保留注释和格式） | `pip install ruamel.yaml` |
| JavaScript | js-yaml | `npm install js-yaml` |
| Java | Jackson (jackson-dataformat-yaml) | Gradle: `implementation 'com.fasterxml.jackson.dataformat:jackson-dataformat-yaml'` |
| Java | SnakeYAML | Maven: `org.yaml.snakeyaml` |
| Go | gopkg.in/yaml.v3 | `go get gopkg.in/yaml.v3` |
| C++ | yaml-cpp | CMake: `find_package(yaml-cpp REQUIRED)` |
| Rust | serde_yaml | `cargo add serde_yaml` |
| C# | YamlDotNet | NuGet: `Install-Package YamlDotNet` |
| Shell | yq（命令行 YAML 处理器） | `brew install yq` / `snap install yq` |

# QEMU 虚拟机

## 核心架构与概念

| 概念 | 说明 | 示例 |
|---|---|---|
| QEMU | 开源的机器模拟器和虚拟化器（由 Fabrice Bellard 开发），全称 Quick Emulator | - |
| 系统模拟 | 完整模拟 CPU、内存、外设（可跨架构，如 x86 运行 ARM 程序） | `qemu-system-aarch64` |
| 用户态模拟 | 仅模拟 CPU 指令集，直接调用宿主机系统调用（极快） | `qemu-arm ./hello_arm` |
| KVM | Linux 内核虚拟化模块，QEMU 配合 KVM 实现接近原生的性能 | `qemu-system-x86_64 -enable-kvm` |
| TCG | Tiny Code Generator，QEMU 自带的 JIT 动态翻译引擎（不用 KVM 时的纯软件模拟） | 慢，但可跨架构 |
| HVF | macOS 上的硬件虚拟化框架（类似 KVM，苹果 M 芯片专用） | `qemu-system-aarch64 -accel hvf` |
| WHPX | Windows 上的硬件虚拟化平台 | `qemu-system-x86_64 -accel whpx` |
| virtio | 半虚拟化 I/O 驱动（极大提升网络和磁盘性能，必须配合 Guest OS 内的驱动使用） | `-device virtio-net-pci` |
| QMP | QEMU Machine Protocol，QEMU 的 JSON 格式管理接口（用于编程控制虚拟机） | `-qmp unix:/tmp/qmp.sock,server,nowait` |
| HMP | Human Monitor Protocol，人机交互控制台（输入命令管理虚拟机） | 按 `Ctrl+a` 然后 `c` 进入 |

## QEMU 两大模式对比

| 对比项 | 系统模拟 | 用户态模拟 |
|---|---|---|
| 命令前缀 | `qemu-system-*` | `qemu-*` |
| 模拟对象 | 完整 PC / 开发板（CPU + 内存 + 网卡 + 显卡等） | 仅模拟 CPU 指令集 |
| 是否需要操作系统 | 需要安装完整的 Guest OS | 不需要，直接运行宿主机的 ELF 可执行文件 |
| 性能 | 较慢（尤其是纯软件模拟时） | 非常快（直接复用宿主机内核） |
| 主要用途 | 运行完整的 Linux / Windows / Android 系统 | 交叉编译测试、跑单个 ARM 程序 |
| 典型场景 | 嵌入式开发、运行树莓派镜像 | 交叉编译后快速验证 ARM 程序逻辑 |

## 常用系统模拟命令（qemu-system-*）

| 命令 | 说明 | 适用场景 |
|---|---|---|
| `qemu-system-x86_64` | 模拟 64 位 x86 PC | 运行 Ubuntu / Windows 虚拟机 |
| `qemu-system-i386` | 模拟 32 位 x86 PC | 运行老旧 32 位系统 |
| `qemu-system-aarch64` | 模拟 64 位 ARMv8 架构 | 模拟树莓派 3/4、运行 ARM 版 Linux |
| `qemu-system-arm` | 模拟 32 位 ARMv7 架构 | 模拟树莓派 2、嵌入式板卡 |
| `qemu-system-riscv64` | 模拟 64 位 RISC-V 架构 | RISC-V 开发板模拟 |
| `qemu-system-mips` | 模拟 MIPS 架构 | 路由器固件模拟 |
| `qemu-system-s390x` | 模拟 IBM 大型机 | 学习大型机汇编 |

## 核心硬件参数选项

| 选项 | 说明 | 示例 |
|---|---|---|
| `-m` / `-memory` | 设置内存大小（支持 M/G 单位） | `-m 2G` 或 `-memory 4096` |
| `-smp` | 设置 CPU 核心数（sockets, cores, threads） | `-smp 4`（4 核）或 `-smp 2,sockets=1,cores=2,threads=1` |
| `-cpu` | 指定模拟的 CPU 型号 | `-cpu qemu64` / `-cpu Haswell` / `-cpu max`（模拟最强特性） |
| `-machine` | 指定机器类型 / 开发板型号 | `-machine pc`（标准 PC）/ `-machine raspi3b`（树莓派 3） |
| `-bios` | 指定 BIOS 固件文件（如 SeaBIOS） | `-bios /usr/share/seabios/bios.bin` |
| `-kernel` | 直接加载 Linux 内核文件（跳过 BIOS / Bootloader 阶段） | `-kernel arch/arm64/boot/Image` |
| `-initrd` | 指定 initramfs 初始内存文件系统 | `-initrd initramfs.cpio.gz` |
| `-append` | 传递给内核的启动参数（配合 `-kernel` 使用） | `-append "console=ttyAMA0 root=/dev/ram0"` |
| `-dtb` | 指定设备树文件（ARM/RISC-V 必须） | `-dtb arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b.dtb` |

## 磁盘与存储选项

| 选项 | 说明 | 示例 |
|---|---|---|
| `-drive` | 定义存储驱动器（最底层参数） | `-drive file=disk.qcow2,format=qcow2,if=virtio` |
| `-hda` / `-hdb` | 快速指定第 1/2 块 IDE 硬盘（老写法） | `-hda ubuntu.qcow2` |
| `-cdrom` | 指定光驱镜像文件 | `-cdrom ubuntu-22.04.iso` |
| `-boot` | 设置启动顺序（c = 硬盘，d = 光驱，n = 网络） | `-boot order=dc`（先光驱后硬盘） |
| `-fda` | 指定软盘镜像 | `-fda boot.img` |
| `-snapshot` | 快照模式：所有写入都不保存到磁盘（保护原始镜像） | `-snapshot` |
| qcow2 格式 | QEMU 推荐的磁盘格式，支持快照、加密、压缩 | `qemu-img create -f qcow2 disk.qcow2 20G` |
| raw 格式 | 原始裸格式，性能最好但不支持快照 | `qemu-img create -f raw disk.img 20G` |

## 网络配置选项

| 选项 | 说明 | 示例 |
|---|---|---|
| `-netdev` | 定义后端网络设备（底层配置） | `-netdev user,id=net0,hostfwd=tcp::8080-:80` |
| `-device` | 定义前端虚拟网卡（插入到虚拟机） | `-device virtio-net-pci,netdev=net0` |
| user 模式 | 默认 NAT 模式，通过宿主机端口转发访问虚拟机 | `hostfwd=tcp::2222-:22`（宿主机 2222 映射到虚拟机 22） |
| tap 模式 | 桥接模式，虚拟机和宿主机在同一局域网（需配置宿主机网桥） | `-netdev tap,id=net0,ifname=tap0` |
| socket 模式 | 两个 QEMU 虚拟机直接互联 | `-netdev socket,id=net0,connect=127.0.0.1:1234` |
| `-nic` | 快捷定义网卡（合并了 `-netdev` 和 `-device`） | `-nic user,model=virtio-net-pci,hostfwd=tcp::8080-:80` |
| `-net none` | 完全禁用网络 | `-net none` |
| `-mac` | 指定网卡的 MAC 地址 | `-net nic,macaddr=52:54:00:12:34:56` |

## 显示与图形界面选项

| 选项 | 说明 | 示例 |
|---|---|---|
| `-display sdl` | 使用 SDL 库渲染图形窗口（默认） | `-display sdl` |
| `-display gtk` | 使用 GTK 库渲染窗口 | `-display gtk` |
| `-display none` | 完全禁用图形输出（纯命令行 / 服务器环境必备） | `-display none` |
| `-nographic` | 等同于 `-display none -serial mon:stdio`（重定向串口到终端） | `-nographic` |
| `-serial` | 将虚拟机的串口重定向到宿主机 | `-serial stdio`（输出到当前终端）/ `-serial file:log.txt` |
| `-vga` | 指定显卡类型（cirrus / std / virtio / virtio-gpu） | `-vga virtio-gpu`（性能最好） |
| `-vnc` | 开启 VNC 服务，远程连接图形界面 | `-vnc :1`（端口 5901）/ `-vnc :0,password=on` |
| `-spice` | 开启 SPICE 协议（比 VNC 流畅，支持 USB 重定向） | `-spice port=5900,addr=0.0.0.0,disable-ticketing` |
| `-gl` | 启用 OpenGL 硬件加速渲染 | `-display sdl,gl=on` |

## 外设与硬件直通选项

| 选项 | 说明 | 示例 |
|---|---|---|
| `-usb` | 启用 USB 控制器 | `-usb` |
| `-usbdevice` | 添加 USB 设备（鼠标、键盘、U 盘等） | `-usbdevice mouse` / `-usbdevice disk:/dev/sdb` |
| `-device usb-...` | 更细粒度的 USB 设备添加 | `-device usb-mouse` / `-device usb-tablet` |
| `-audio` | 启用音频支持 | `-audio driver=pa,model=hda` |
| `-device intel-hda` | 添加 Intel 高清声卡 | `-device intel-hda -device hda-duplex` |
| `-hostdev` | PCI / USB 设备直通（把宿主机物理设备给虚拟机用） | `-hostdev 0000:01:00.0`（直通显卡） |

## 加速器选项

| 选项 | 说明 | 示例 |
|---|---|---|
| `-accel tcg` | 纯软件模拟（默认，最慢，可跨架构） | `-accel tcg,thread=multi` |
| `-accel kvm` | 使用 Linux KVM 硬件加速（最快，只能同架构） | `-accel kvm` |
| `-accel hvf` | 使用 macOS 硬件加速（M 芯片 Mac 必备） | `-accel hvf` |
| `-accel whpx` | 使用 Windows 硬件加速 | `-accel whpx` |
| `-enable-kvm` | 老写法开启 KVM（等同于 `-accel kvm`） | `-enable-kvm` |
| `-thread multi` | TCG 模式下开启多线程翻译（稍微提升性能） | `-accel tcg,thread=multi` |

## 快照与状态管理

| 选项 / 命令 | 说明 | 示例 |
|---|---|---|
| `-snapshot` | 运行时快照模式（退出后不保存修改） | `qemu-system-x86_64 -snapshot -hda disk.qcow2` |
| `qemu-img create` | 创建新的空磁盘镜像 | `qemu-img create -f qcow2 disk.qcow2 20G` |
| `qemu-img convert` | 转换镜像格式（如 raw 转 qcow2） | `qemu-img convert -f raw -O qcow2 disk.img disk.qcow2` |
| `qemu-img info` | 查看镜像详细信息 | `qemu-img info disk.qcow2` |
| `qemu-img snapshot` | 管理镜像内部的快照 | `qemu-img snapshot -c snap1 disk.qcow2`（创建） |
| `qemu-img resize` | 调整磁盘镜像大小（只能增大或用 shrink 参数缩小） | `qemu-img resize disk.qcow2 +10G` |
| `savevm` [HMP] | 在运行中的虚拟机创建快照 | `(savevm my_snapshot)` |
| `loadvm` [HMP] | 恢复到指定快照 | `(loadvm my_snapshot)` |
| `delvm` [HMP] | 删除指定快照 | `(delvm my_snapshot)` |

## qemu-img 镜像管理工具

| 命令 | 说明 | 示例 |
|---|---|---|
| `qemu-img create -f qcow2` | 创建 qcow2 格式镜像 | `qemu-img create -f qcow2 vm.qcow2 20G` |
| `qemu-img create -f raw` | 创建 raw 格式镜像 | `qemu-img create -f raw vm.img 20G` |
| `qemu-img convert -O qcow2` | raw 转 qcow2（压缩空间，启用特性） | `qemu-img convert -f raw -O qcow2 raw.img qcow2.img` |
| `qemu-img convert -O raw` | qcow2 转 raw（性能最好，用于某些云平台） | `qemu-img convert -f qcow2 -O raw qcow2.img raw.img` |
| `qemu-img commit` | 将 qcow2 快照中的修改合并到基础镜像 | `qemu-img commit -f qcow2 snapshot.qcow2` |
| `qemu-img check` | 检查镜像完整性 | `qemu-img check disk.qcow2` |
| `qemu-img dd` | 类 Unix 的 dd 命令，用于镜像切片 | `qemu-img dd if=disk.qcow2 of=part.img skip=0 count=1024` |
| `qemu-nbd` | 将 QEMU 镜像挂载为宿主机的网络块设备（可直接读写镜像内文件） | `qemu-nbd -c /dev/nbd0 disk.qcow2`<br>`mount /dev/nbd0p1 /mnt` |

## 监视器与调试选项

| 快捷键 / 选项 | 说明 | 示例 |
|---|---|---|
| `Ctrl+a, c` | 切换到 QEMU HMP 控制台 / 切回虚拟机 | 在运行界面按住 Ctrl+a，松开按 c |
| `Ctrl+a, x` | 强制退出 QEMU | 在运行界面按 |
| `info status` [HMP] | 查看虚拟机运行状态 | `(info status)` |
| `info network` [HMP] | 查看网络设备状态 | `(info network)` |
| `stop` / `cont` [HMP] | 暂停 / 继续虚拟机运行 | `(stop)` / `(cont)` |
| `system_reset` [HMP] | 热重启虚拟机 | `(system_reset)` |
| `-gdb` | 开启 GDB 调试服务器，等待 GDB 连接 | `-gdb tcp::1234` |
| `-S` | 启动时冻结 CPU（等待 GDB 连接后继续，配合 `-gdb` 用） | `-S -gdb tcp::1234` |
| `-d int` | 输出 CPU 执行日志（用于极度深度的逆向分析） | `-d int,out -D log.txt` |

## 实战：典型启动命令模板

1. x86_64 快速启动 Linux 虚拟机（KVM 加速）：

```bash
qemu-system-x86_64 \
  -enable-kvm \
  -m 4G \
  -smp 4 \
  -drive file=ubuntu.qcow2,format=qcow2,if=virtio \
  -cdrom ubuntu-22.04-live-server-amd64.iso \
  -boot order=dc \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -device virtio-net-pci,netdev=net0 \
  -nographic
```

2. ARM64 模拟树莓派 3B（无需 KVM，TCG 模拟）：

```bash
qemu-system-aarch64 \
  -machine raspi3b \
  -cpu cortex-a72 \
  -m 1G \
  -drive file=raspbian.img,format=raw,if=sd \
  -kernel kernel8.img \
  -dtb bcm2710-rpi-3-b.dtb \
  -append "console=serial0,115200 root=/dev/mmcblk0p2 rootfstype=ext4 rw" \
  -nographic
```

3. 纯命令行无图形启动（服务器 / 嵌入式常用）：

```bash
qemu-system-x86_64 \
  -m 2G \
  -drive file=server.img,format=qcow2 \
  -nographic \
  -snapshot
```

4. 远程 VNC 访问的 Windows 虚拟机：

```bash
qemu-system-x86_64 \
  -enable-kvm \
  -m 8G \
  -smp 4 \
  -drive file=win10.qcow2,format=qcow2 \
  -drive file=virtio-win.iso,media=cdrom \
  -boot c \
  -netdev user,id=net0 \
  -device virtio-net-pci,netdev=net0 \
  -vnc :1,password=on \
  -device virtio-balloon
```

5. GDB 交叉调试 ARM 程序（配合 user-mode 或 kernel）：

```bash
qemu-system-aarch64 \
  -machine virt \
  -cpu cortex-a57 \
  -kernel zImage \
  -initrd rootfs.cpio \
  -append "console=ttyAMA0" \
  -nographic \
  -S -gdb tcp::1234
```

另开终端：

```bash
aarch64-linux-gnu-gdb vmlinux
# (gdb) target remote :1234
```

## 云镜像与 libvirt 生态

| 概念 | 说明 | 示例 |
|---|---|---|
| libvirt | 虚拟机管理 API / C 库（virsh 命令行工具底层） | `virsh start vm1` |
| virsh | 基于 libvirt 的虚拟机管理命令行工具 | `virsh list --all` |
| virt-manager | 基于 libvirt 的图形化管理界面（GUI） | `virt-manager` |
| virt-install | 命令行快速创建虚拟机（自动生成 QEMU 长命令） | `virt-install --name test --ram 2048 --disk path=disk.qcow2 --cdrom iso` |
| Cloud-Init | 云实例初始化标准（自动配置网络、密码、SSH Key） | 配合 `-drive file=seed.img`（包含 meta-data 和 user-data） |
| qcow2 加密 | 创建加密的磁盘镜像 | `qemu-img create -f qcow2 -o encrypt.format=luks disk.qcow2 20G` |

# WSL

## 基本概念与核心定位

| 概念 | 说明 |
|---|---|
| WSL | Windows Subsystem for Linux，微软官方提供的 Windows 子系统，能在 Windows 上原生运行 Linux |
| WSL2 | WSL 的第二代架构，使用真正的 Linux 内核（微软定制），运行在轻量级 Hyper-V 虚拟机上，性能接近原生 |
| WSL1 | 第一代架构（已不推荐），通过系统调用翻译层运行 Linux，兼容性差、不支持 Docker、无完整 systemd |
| Ubuntu on WSL | WSL 中最流行的 Linux 发行版（默认安装），相当于一台完整的 Ubuntu 虚拟机，但和 Windows 共享文件系统 |
| 与虚拟机区别 | WSL2 秒级启动、秒级休眠，与 Windows 互操作（共享剪贴板 / 网络 / 文件），资源占用极低 |
| 与双系统区别 | 不需要重启切换、不需要单独分区，可以同时运行 Windows 和 Linux 程序 |
| 核心价值 | 让 Windows 用户拥有完整的 Linux 开发环境，无需安装 VMware / VirtualBox 等重量级虚拟机 |
| 前置要求 | Windows 10 版本 2004+（内部版本 19041+）或 Windows 11；需开启虚拟化（BIOS 中 VT-x / AMD-V） |

## 安装与卸载

| 操作 | 命令 / 方式 | 说明 |
|---|---|---|
| 检查 WSL 是否已安装 | `wsl --version` | 查看已安装的 WSL 版本和分发版 |
| 安装 WSL（推荐） | `wsl --install` | 一键安装 WSL2 + 默认 Ubuntu 发行版（需管理员 PowerShell） |
| 指定发行版安装 | `wsl --install -d Ubuntu-22.04` | 可指定版本：Ubuntu-22.04 / Ubuntu-20.04 / Debian / Kali-Linux |
| 列出可用发行版 | `wsl --list --online` | 查看微软商店中所有可安装的 Linux 发行版 |
| 安装指定版本 | `wsl --install -d kali-linux` | 安装 Kali Linux |
| 卸载发行版 | `wsl --unregister <发行版名称>` | 完全删除指定发行版及其所有文件（不可恢复！） |
| 卸载所有 | `wsl --unregister Ubuntu` | 注意数据会全部丢失 |
| 关闭 WSL | `wsl --shutdown` | 关闭所有 WSL 实例（相当于强制关闭虚拟机，下次启动重新加载内核） |
| 更新 WSL | `wsl --update` | 更新 WSL 到最新版本 |
| 设置默认版本 | `wsl --set-default-version 2` | 确保新建的发行版使用 WSL2 |
| 检查虚拟化是否开启 | `systeminfo \| findstr /i "虚拟化"` | 需显示“已启用固件中的虚拟化”，否则去 BIOS 开启 |

## WSL1 vs WSL2 详细对比

| 对比维度 | WSL1 | WSL2 |
|---|---|---|
| 架构 | 系统调用翻译层（无内核） | 真正的 Linux 内核（Hyper-V 轻量虚拟机） |
| 文件系统性能 | Windows 文件上速度还行 | Linux 文件上极快，但访问 Windows 文件（/mnt/c/）很慢 |
| Docker 支持 | 不支持 | 原生支持 Docker Desktop |
| systemd | 不支持 | 支持（需手动开启） |
| 完整 Linux | 大部分兼容 | 几乎 100% 兼容（包括 FUSE、iptables、DBus） |
| 网络 | 和 Windows 共享网络栈 | 独立的网络栈（有自己的 IP 地址） |
| USB 设备 | 不支持 | 支持 USB/IP 直通 |
| GUI 应用 | 不支持 | WSLg 支持（直接在 Windows 桌面显示 Linux GUI 程序） |
| 启动速度 | 更快 | 稍慢（需启动轻量虚拟机） |
| 推荐使用 | 仅限老 Windows 版本 | 绝对推荐（默认） |

## 基本操作与日常使用

| 命令 | 说明 | 示例 |
|---|---|---|
| `wsl` | 在 Windows 终端直接进入默认的 Linux 发行版 | `wsl` |
| `wsl -d <发行版>` | 进入指定的发行版 | `wsl -d kali-linux` |
| `wsl --list -v` | 列出所有已安装的发行版及其状态和版本 | `wsl --list -v`（简写 `wsl -l -v`） |
| `wsl --shutdown` | 关闭所有 WSL 实例（解决各种卡死问题） | `wsl --shutdown` |
| `wsl --terminate <名称>` | 只关闭指定的发行版 | `wsl --terminate Ubuntu` |
| `wsl --set-default <名称>` | 设置默认启动的发行版 | `wsl --set-default Ubuntu-22.04` |
| `wsl --export` | 导出整个发行版为 tar 备份文件 | `wsl --export Ubuntu D:\backup\ubuntu.tar` |
| `wsl --import` | 从 tar 备份文件导入恢复 | `wsl --import Ubuntu D:\WSL\Ubuntu D:\backup\ubuntu.tar` |
| 从 Windows 直接执行 Linux 命令 | 在 PowerShell 中执行 Linux 命令 | `wsl ls -la` |
| 从 Linux 中执行 Windows 命令 | 在 WSL 终端直接输入 `.exe` 程序名 | `explorer.exe .`（用 Windows 资源管理器打开当前目录） |

## 文件系统互访（核心重点）

| 场景 | 路径 / 命令 | 说明 |
|---|---|---|
| Windows 访问 WSL 文件 | 资源管理器地址栏输入 `\\wsl$\Ubuntu\home\用户名` | 或 WSL 终端输入 `explorer.exe .` |
| WSL 访问 Windows C 盘 | `/mnt/c/Users/你的用户名/Desktop` | `/mnt/` 相当于 Windows 的各个盘符 |
| WSL 访问 Windows D 盘 | `/mnt/d/projects` | 同理 |
| WSL 访问 Windows E 盘 | `/mnt/e/videos` | 同理 |
| 性能警告 | 在 `/mnt/c/` 下编译代码、运行 Node.js 极其缓慢！跨文件系统 IO 性能差 5-10 倍 | 务必把项目放在 WSL 的 Linux 原生文件系统内 |
| 推荐做法 | 项目文件存放在 `/home/用户名/` 下（Linux 原生文件系统） | 用 VS Code Remote-WSL 插件在 Windows 里编辑 WSL 中的文件 |
| 挂载其他磁盘 | `sudo mount -t drvfs E: /mnt/e` | WSL 启动时自动挂载所有 Windows 分区到 `/mnt/` |
| WSL 中自动挂载 | 在 `/etc/wsl.conf` 中配置 | 见下方 wsl.conf 配置 |

## VS Code 集成（开发体验最好的方式）

| 操作 | 说明 |
|---|---|
| 安装插件 | 在 VS Code 中安装 Remote - WSL 扩展 |
| 在 WSL 中打开项目 | WSL 终端输入 `code .`，自动在 VS Code 中打开当前目录（推荐方式） |
| 终端集成 | VS Code 底部终端自动使用 WSL 的 bash/zsh，在 Windows 里享受 Linux 终端 |
| 断点调试 | VS Code 调试器直接附加到 WSL 里运行的程序，完全透明的开发体验 |
| 文件保存 | 在 VS Code 里编辑的文件直接保存在 WSL 的 Linux 文件系统中，无需手动复制 |
| 好处 | 开发体验和原生 Linux 完全一致，但享受 Windows 的 GUI 和工具 |

## WSLg 图形界面支持（WSL2 独有）

| 概念 | 说明 |
|---|---|
| WSLg | WSL2 内置的 GUI 支持，可以直接在 Windows 桌面显示 Linux 图形应用 |
| 支持的 GUI | 任何 X11 或 Wayland 应用（Firefox、GIMP、VS Code Linux 版、Nautilus 等） |
| 使用方式 | 在 WSL 终端直接输入 GUI 程序名，窗口会弹到 Windows 桌面上 |
| 剪贴板 | Windows 和 Linux GUI 应用之间剪贴板互通，Ctrl+C / Ctrl+V 通用 |
| 音频 | 自动支持，无需额外配置 |
| 示例 | 在 WSL 中运行 `firefox &`（后台启动 Firefox，窗口显示在 Windows 桌面上） |
| 注意事项 | 部分 3D / GPU 加速应用可能不流畅，大多数日常 GUI 程序没问题 |

## 网络配置

| 特性 | 说明 | 示例 |
|---|---|---|
| 默认 NAT 模式 | WSL2 通过 NAT 共享 Windows 的网络，WSL 有自己的内部 IP | 有自己独立的 IP（与 Windows 不同） |
| 端口转发 | WSL 中监听的端口，Windows 的 localhost 可直接访问 | WSL 中跑 `flask run --port 5000`，Windows 浏览器访问 localhost:5000 |
| WSL 访问 Windows 服务 | Windows 上跑的服务，WSL 可通过 Windows 主机 IP 访问 | `getent hosts host.docker.internal` 或 `$(cat /etc/resolv.conf \| grep nameserver \| awk '{print $2}')` |
| 端口冲突 | WSL 和 Windows 不能监听同一端口 | 端口转发是自动的 |
| 防火墙 | WSL 中的服务默认会被 Windows 防火墙阻挡 | 首次访问时 Windows 会弹防火墙提示，需允许 |
| 静态 IP 配置 | `/etc/wsl.conf` 中可设置（WSL2 新特性） | 见下方 wsl.conf 配置 |
| 镜像网络模式 | `.wslconfig` 中设置 `networkingMode=mirrored`（WSL2 新特性） | WSL 和 Windows 共享同一 IP，无需端口转发 |

## wsl.conf 与 .wslconfig 配置文件

`/etc/wsl.conf`（WSL Linux 内部配置，以 root 权限编辑，配置 WSL 内的 Linux 行为）：

```ini
[boot]
command = /usr/sbin/service docker start   # WSL 启动时自动运行命令

[interop]
enabled = true           # 是否允许 Windows 和 Linux 互操作（默认 true）
appendWindowsPath = true # 是否将 Windows 的 PATH 追加到 Linux PATH（默认 true，设 false 可加快 Linux 命令查找速度）

[network]
generateResolvConf = true # 是否自动生成 /etc/resolv.conf（DNS 配置）

[automount]
enabled = true            # 是否自动挂载 Windows 磁盘到 /mnt/
root = /mnt/              # 挂载点
options = "metadata,umask=22,fmask=11"  # 挂载选项（metadata 保留 Linux 文件权限）

[user]
default = yourname        # 默认登录用户（不设则为 root）
```

`%UserProfile%\.wslconfig`（Windows 用户目录下，配置 WSL2 全局行为，主要是资源限制）：

```ini
[wsl2]
memory = 8GB               # 分配给 WSL2 的最大内存（默认使用 Windows 内存的一半）
processors = 4             # 分配给 WSL2 的 CPU 核心数
swap = 4GB                 # 交换空间大小
localhostForwarding = true # 是否自动转发 localhost 端口（默认 true）

[experimental]
networkingMode = mirrored    # 镜像网络模式（WSL 和 Windows 共享 IP）
autoMemoryReclaim = gradual  # 自动回收内存
sparseVhd = true             # 稀疏 VHD（自动回收磁盘空间）
hostAddressLoopback = true   # 允许通过 localhost 访问 Windows 上的服务
```

## systemd 支持（WSL2 新特性）

| 操作 | 命令 / 说明 |
|---|---|
| 开启 systemd | 编辑 `/etc/wsl.conf` 添加 `[boot]` 下的 `systemd=true`，然后在 PowerShell 执行 `wsl --shutdown` 并重启 WSL |
| 验证 systemd | `systemctl list-units --type=service`，能看到 systemd 管理的服务列表 |
| 用途 1 | Docker 服务可以用 `systemctl start docker` 管理，之前只能手动 `service docker start` |
| 用途 2 | SSH 服务可以用 `systemctl status sshd` 查看，管理更标准 |
| 用途 3 | 所有 Linux 服务管理回归标准 Linux 方式 |
| 注意 | 开启 systemd 后，WSL 启动会稍慢一些 |

## Docker Desktop 集成

| 配置 | 说明 |
|---|---|
| 安装 | Docker Desktop for Windows，安装时勾选 Use WSL 2 based engine |
| 集成 | Settings - Resources - WSL Integration，勾选你的 WSL 发行版，启用后 WSL 中可直接用 docker 命令 |
| 原理 | Docker Engine 运行在 WSL2 的后台虚拟机中，WSL 发行版通过 socket 通信，无需在 WSL 内再装 Docker |
| 好处 | 无需在 WSL 中手动安装 Docker，Windows 和 WSL 共享同一个 Docker 守护进程，性能极好 |
| 验证 | `docker --version && docker ps`，能在 WSL 终端直接使用 docker 命令 |
| 无 Docker Desktop 方案 | 在 WSL 内手动安装 Docker Engine：`curl -fsSL https://get.docker.com \| sh`，适合不需要 GUI 管理的场景 |

## 开发环境搭建实战（WSL 最佳实践）

```bash
# ===== 1. 系统更新与基础工具 =====
sudo apt update && sudo apt upgrade -y
sudo apt install -y build-essential cmake git curl wget vim tmux htop unzip net-tools

# ===== 2. 安装 zsh 与 oh-my-zsh（美化终端） =====
sudo apt install -y zsh
sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"

# ===== 3. 安装 Conda（Python 环境管理） =====
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
bash Miniconda3-latest-Linux-x86_64.sh
source ~/.bashrc   # 或 source ~/.zshrc

# ===== 4. 安装 Node.js（通过 nvm） =====
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.0/install.sh | bash
source ~/.zshrc
nvm install 18
nvm use 18

# ===== 5. 安装 GDB + Pwndbg（二进制调试） =====
sudo apt install -y gdb
git clone https://github.com/pwndbg/pwndbg.git ~/pwndbg
cd ~/pwndbg && ./setup.sh

# ===== 6. 安装 QEMU（虚拟化/模拟） =====
sudo apt install -y qemu-system-x86 qemu-system-arm qemu-utils

# ===== 7. 安装 Docker =====
# 见上文“Docker：安装与国内镜像源”一节（WSL 中同样适用）

# ===== 8. 配置 Git =====
git config --global user.name "yourname"
git config --global user.email "youremail@qq.com"
git config --global core.autocrlf false
git config --global core.quotepath false
git config --global init.defaultBranch main

# ===== 9. VS Code 连接 =====
# 在 WSL 终端执行：
code .
```

## 常见问题与排错（FAQ）

| 问题 | 原因与解决 |
|---|---|
| `wsl --install` 报错 0x8007019e | 未启用“适用于 Linux 的 Windows 子系统”功能，PowerShell 管理员执行：`Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux` |
| `wsl --install` 报错 0x80370102 | 未启用虚拟化或 CPU 不支持：1. BIOS 开启 VT-x / AMD-V；2. 检查 `systeminfo \| findstr "虚拟化"` |
| WSL 启动报错 WslService / ERROR | WSL 内核组件异常，尝试更新 `wsl --update --pre-release`，或重启电脑 |
| `/mnt/c/` 下编译极慢 | 跨文件系统 IO 性能差（5-10 倍），把项目放到 `/home/` 下 |
| WSL 中 ping 不通外网 | 1. 检查 /etc/resolv.conf DNS 配置；2. 手动设置 `sudo echo "nameserver 8.8.8.8" > /etc/resolv.conf` |
| WSL 内存占用过高 | WSL2 默认可用 Windows 一半内存，用 .wslconfig 限制：`[wsl2]` 下 `memory=4GB` |
| WSL 磁盘不释放 | WSL2 使用 VHD 虚拟磁盘，文件删除后 VHD 不会自动缩小：`wsl --shutdown` 后用 diskpart 压缩 VHD，或设置 `sparseVhd=true` |
| 磁盘空间不够 | WSL2 的 VHD 默认无大小限制，会不断增长：.wslconfig 中 `[wsl2]` 设 `memory=4GB`、`swap=2GB` |
| WSL 中 USB 设备不可用 | WSL1 不支持 USB，WSL2 需配置 USB/IP：安装 usbipd-win 工具（github.com/dorssel/usbipd-win） |
| SSH 连接 WSL | 在 WSL 内 `sudo service ssh start` 或 `systemctl start sshd`，Windows 端 `ssh 用户名@localhost` |
| Windows Terminal 美化 | 安装 Windows Terminal + 配置 Oh My Posh 或 Starship，终端体验接近 Mac/Linux |
| WSL 中运行 GUI 程序闪退 | 确保开启了 WSLg（WSL2 默认已开启），并安装所需的 GUI 库：`sudo apt install -y libx11-6 libgtk-3-0` |

## WSL 备份与迁移

| 场景 | 命令 / 操作 |
|---|---|
| 导出备份 | `wsl --export Ubuntu D:\backup\ubuntu-backup.tar`，导出整个发行版为 tar 文件 |
| 导入恢复 | `wsl --import MyUbuntu D:\WSL\MyUbuntu D:\backup\ubuntu-backup.tar`，可指定新的安装位置 |
| 迁移到其他盘 | `wsl --export Ubuntu D:\backup.tar && wsl --unregister Ubuntu && wsl --import Ubuntu E:\WSL\Ubuntu D:\backup.tar --version 2`，先导出再导入到新位置 |
| 定时备份 | 写 PowerShell 脚本：`wsl --export Ubuntu "D:\backup\ubuntu_$(Get-Date -Format 'yyyyMMdd').tar"`，配合 Windows 任务计划程序自动执行 |

## WSL 与虚拟机的选择建议

| 场景 | 推荐 WSL2 | 推荐虚拟机 |
|---|---|---|
| 操作系统 | Windows 主力机 | 任何 OS |
| Linux 用途 | 开发 / 命令行 / CTF / 学习 | 需要完整桌面环境（GNOME / KDE） |
| 性能需求 | 轻量级，秒级启动 | 需要大量资源（多核 / 大内存 / 大磁盘） |
| Docker | 原生支持 | 支持 |
| GUI 需求 | WSLg 支持基本 GUI（够用） | 完整桌面体验 |
| Kali Linux | 可安装 Kali-WSL | 完整 Kali 桌面体验 |
| QEMU 嵌套虚拟化 | 部分场景不行 | 完全支持嵌套虚拟化 |
| 文件共享 | 无缝（/mnt/ 互访） | 需配置共享文件夹 |
| 推荐 | 大多数开发和学习场景 | 需要完整 GUI / 嵌套虚拟化 / 特殊硬件 |
