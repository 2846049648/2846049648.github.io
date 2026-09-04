---
title: 开发工具与包管理：npm / Prettier
date: 2026-04-10
category: 学习记录
tags: ["npm", "prettier", "包管理"]
excerpt: 包管理工具总览、npm 常用命令与 Prettier 代码格式化
---
# 包管理工具总览

包管理器用来解决“装软件/装依赖”这件事，不同语言、不同系统各有各的工具。梳理下来大致有这些：

1. Python 的 `venv`——Python 标准虚拟环境，用于隔离项目依赖。
2. `conda`——跨语言包与环境管理器，Python/R 等都能管。
3. Windows 的“亲儿子”包管理工具 `winget`——微软官方命令行包管理器。
4. Windows 下的“软件自动化安装管家” `Chocolatey`——第三方社区包管理。
5. Ubuntu 的包管理工具 `apt`——Linux 发行版最常用的软件安装方式。
6. JS 的包管理器 `npm`——Node.js 官方内置，全球最大的开源库生态（后文详述）。
7. Ubuntu 里另一个装软件的工具——之前用它装过 VS Code。

不同生态各司其职：语言层面用 venv/conda/npm 管“依赖库”，系统层面用 winget/Chocolatey/apt 管“应用程序”。

# npm

## 基本概念与核心定位

npm（Node Package Manager）是 Node.js 官方内置的包管理器，背后是全球最大的开源库生态系统。

| 概念 | 说明 |
|---|---|
| Node.js | JavaScript 的服务端运行时环境（安装 Node.js 时自动附带安装 npm） |
| npx | Node Package eXecute，npm 5.2+ 自带，用于直接运行 npm 包中的可执行命令（无需全局安装） |
| package.json | 项目的核心配置文件，记录项目名称、版本、依赖列表、脚本命令等 |
| node_modules | 存放所有安装的第三方依赖包的目录（可删除后 `npm install` 重建） |
| package-lock.json | 锁定所有依赖的精确版本号和下载地址（确保团队和部署环境依赖完全一致） |
| registry | npm 官方包仓库（https://registry.npmjs.org/），所有 npm 包的上传和下载都经过这里 |
| npm vs yarn vs pnpm | yarn（Facebook 出品）和 pnpm（极节省磁盘）是 npm 的替代品，命令相似，pnpm 当前最火 |
| 核心作用 | 安装/卸载/管理第三方库、定义和运行项目脚本、发布自己的包到 npm 仓库 |

## 安装与版本管理

| 操作 | 命令/说明 | 备注 |
|---|---|---|
| 安装 Node.js（推荐） | 官网下载 LTS 版：https://nodejs.org/ | 自带 npm，无需单独装 |
| 安装 Node.js（nvm） | `nvm install 18 && nvm use 18` | 推荐用 nvm 管理多版本 Node.js |
| 安装 Node.js（winget） | `winget install OpenJS.NodeJS.LTS` | Windows 用户 |
| 安装 Node.js（choco） | `choco install nodejs-lts` | Windows 用户 |
| 安装 Node.js（apt） | `sudo apt install nodejs npm -y` | Linux（版本可能较旧，推荐 nvm） |
| 验证安装 | `node -v && npm -v` | 显示 Node.js 和 npm 的版本号 |
| 升级 npm（官方推荐） | `npm install -g npm@latest` | 升级到最新版 |
| 升级 npm（npm 自带） | `npm install -g npm@$(npm -v)` | 自动检测最新版 |
| 升级 npm（终极方案） | `npx npm@latest install -g npm@latest` | 用 npx 运行最新版 npm 来升级自己 |

nvm（Node Version Manager）用来管理多个 Node.js 版本：`nvm install 18; nvm use 18; nvm alias default 18`。它的替代品 `n` 更简洁：`n 18; n lts; n rm 14`。

## 初始化项目

| 命令 | 说明 | 示例 |
|---|---|---|
| `npm init` | 交互式创建 package.json（逐步问答） | `npm init` |
| `npm init -y` / `--yes` | 跳过问答，使用默认值直接生成 package.json（最常用） | `npm init -y` |
| `npm init -y --scope` | 指定包的作用域（用于组织私有包） | `npm init --scope=@myorg` |

package.json 核心字段：

| 字段 | 说明 | 示例 |
|---|---|---|
| `name` | 包名（必须小写，不能有空格） | `"my-awesome-project"` |
| `version` | 版本号（遵循 semver 语义化版本：MAJOR.MINOR.PATCH） | `"1.0.0"` |
| `description` | 项目描述 | `"A web application"` |
| `main` | 入口文件（不指定则默认 index.js） | `"src/index.js"` |
| `scripts` | 脚本命令（npm run 的命令集合） | `"start": "node server.js"` |
| `dependencies` | 生产依赖（打包上线后仍需要） | `"express": "^4.18.0"` |
| `devDependencies` | 开发依赖（只在开发时需要，如构建工具） | `"webpack": "^5.0.0"` |
| `peerDependencies` | 对等依赖（提示宿主项目应安装的包） | `"react": ">=16.0.0"` |
| `engines` | 指定 Node.js 和 npm 版本范围 | `"node": ">=16.0.0"` |
| `type` | 模块类型："module" 为 ESM，默认 "commonjs" 为 CJS | `"type": "module"` |

## 依赖安装

| 命令 | 说明 | 保存位置 | 示例 |
|---|---|---|---|
| `npm install 包名` | 安装到 dependencies（生产依赖） | 写入 package.json 的 dependencies | `npm install express` |
| `npm install 包名 --save-dev` | 安装到 devDependencies（开发依赖） | 写入 package.json 的 devDependencies | `npm install webpack --save-dev`（简写 `-D`） |
| `npm install 包名 -D` | 同上，简写 | 同上 | `npm install typescript -D` |
| `npm install 包名 -g` | 全局安装（在系统目录，所有项目可用） | 不写入 package.json，装到全局 node_modules | `npm install pm2 -g` |
| `npm install` | 不指定包名，根据 package.json 安装所有依赖 | 重建 node_modules | `npm install`（简写 `npm i`） |
| `npm install 包名@版本` | 安装指定版本 | 按版本写入依赖 | `npm install express@4.17.0` |
| `npm install 包名@latest` | 安装最新版 | 写入依赖 | `npm install lodash@latest` |

对应地，卸载依赖使用 `npm uninstall 包名`（简写 `npm rm 包名`），配合 `-D` 表示删除开发依赖、`-g` 表示卸载全局包；`npm ls`（或 `npm list`）可查看当前项目已安装的依赖树。

当项目带 lockfile、需要在干净环境里做一次“可复现安装”（如 CI）时，推荐用 `npm ci`：它完全依照 `package-lock.json` 安装并删除 node_modules，比 `npm install` 更快更严格，不会改动锁文件。

## 项目脚本 scripts

package.json 里的 `scripts` 字段集中定义项目常用命令，用 `npm run <脚本名>` 触发，是团队协作里统一命令入口的标准做法：

```json
{
  "name": "my-app",
  "scripts": {
    "dev": "vite",
    "build": "vite build",
    "preview": "vite preview",
    "test": "jest",
    "lint": "eslint . --fix"
  }
}
```

常用约定：

- `npm run dev` / `npm run build` / `npm test`（npm test、`npm start` 等少数几个脚本可省略 `run` 直接执行）。
- 在脚本中要把参数传给内部命令时，用 `--` 分隔，例如 `npm run lint -- --fix`。
- `npm run`（不带脚本名）会列出当前项目定义的全部可用脚本。

## 常用命令速查

| 命令 | 说明 | 示例 |
|---|---|---|
| `npm view 包名` | 查看包在 registry 上的元信息（版本、描述等） | `npm view express` |
| `npm outdated` | 列出有可用更新的依赖 | `npm outdated` |
| `npm update` | 按 semver 范围更新依赖 | `npm update` |
| `npm audit` | 检查依赖的安全漏洞 | `npm audit fix` |
| `npm config` | 查看/设置 npm 配置 | `npm config get registry`（查看当前镜像源） |
| `npm publish` | 发布自己的包到 registry | 在包目录下执行 `npm publish` |

# Prettier 代码格式化

Prettier 是一个非常棒的代码格式化工具，它的核心作用就是自动统一代码风格。把它装成开发依赖，配合编辑器保存时格式化，整个项目的代码风格就能保持一致。

安装（作为开发依赖）：

```bash
npm install --save-dev prettier
```

使用：

```bash
# 格式化单个文件
npx prettier --write src/index.js

# 格式化整个 src 目录下的所有文件
npx prettier --write src/

# 格式化项目里所有支持的文件（推荐）
npx prettier --write .
```

其中 `--write` 表示直接把格式化结果写回原文件；如果不加 `--write`，Prettier 只把格式化后的结果输出到终端，方便先预览。日常推荐把格式化动作挂进 npm 脚本（如 `"format": "prettier --write ."`），一条命令格式化全项目。
