---
title: Gitee 与 Git 使用
date: 2026-05-11
category: 学习记录
tags: ["git", "gitee"]
excerpt: 建立 Gitee 仓库、SSH 配置与日常 Git 操作
---
# Gitee 平台简介

Gitee 即码云，是国内最大的代码托管平台（基于 Git），由开源中国 OSChina 团队开发。

| 概念 | 说明 |
|---|---|
| Gitee | 码云，国内最大的代码托管平台（基于 Git），由开源中国 OSChina 团队开发 |
| 与 GitHub 区别 | 服务器在国内，访问速度快；完全中文界面；本土化合规；免费提供私有仓库 |
| 核心功能 | 代码托管、版本控制、项目管理（看板）、CI/CD 流水线、代码审查、Wiki 文档、WebHook |
| 仓库 | 存放项目代码的空间，分为公开和私有 |
| 组织 | 企业或团队账号，可集中管理多个仓库和成员 |
| Web IDE | 网页端在线编辑代码，无需拉取到本地 |
| GVP | Gitee 最有价值开源项目（国内权威开源项目榜单） |

# 账号设置与安全（新手第一步）

## 绑定邮箱与贡献度统计

Gitee 强制要求提交者的邮箱必须是账号绑定的邮箱，否则提交会显示为灰色未知用户，不计入首页绿点统计。换电脑后必须重新配置本地 git 邮箱：

```bash
git config --global user.email "gitee绑定的邮箱@qq.com"
```

## 双因素认证与私人令牌

| 功能 | 路径 | 说明 |
|---|---|---|
| 开启双因素认证（2FA） | 设置 - 安全设置 - 双因素认证 | 提升账号安全性，防止被盗 |
| 私人令牌 | 设置 - 私人令牌 - 生成新令牌 | 替代密码进行 HTTPS 方式推送，防止密码泄露 |

# 安装并配置 Git

安装 Git：

```bash
sudo apt install git
```

配置用户名和邮箱（必须，需与 Gitee 账号一致），终端（Git Bash / cmd / Terminal）中输入：

```bash
git config --global user.name "你的Gitee用户名"
git config --global user.email "你的Gitee注册邮箱"
```

## 其他常用 Git 全局配置

| 命令 | 说明 |
|---|---|
| `git config --global user.name "你的名字"` | 设置全局用户名 |
| `git config --global user.email "gitee绑定邮箱"` | 设置全局邮箱（必须与 Gitee 绑定邮箱一致） |
| `git config --global init.defaultBranch main` | 设置默认分支名为 main |
| `git config --global core.autocrlf false` | Windows 下禁用自动换行符转换（防乱码） |
| `git config --global core.quotepath false` | 解决 Git 中文文件名显示为转义字符的问题 |
| `git config --global credential.helper store` | 保存 HTTPS 密码（不推荐，建议用 SSH） |
| `git config --global core.editor "vim"` | 设置默认提交信息编辑器 |

# 新建 Gitee 仓库

在 Gitee 网页上点击右上角“+”号，选择“新建仓库”，可选公开/私有，可选择初始化 README、添加 .gitignore。Gitee 还支持从 GitHub / GitLab 等 URL 直接导入已有仓库（新建仓库 - 导入已有仓库）。

创建好仓库后，你会得到仓库地址，本地需要用到。

# SSH 公钥配置

SSH 公钥必须配置，否则每次拉取/推送都要输密码。

## 生成 SSH 密钥

终端执行（一路回车即可）：

```bash
ssh-keygen -t ed25519 -C "你的邮箱"
```

在 `~/.ssh/id_ed25519.pub` 中获取公钥内容。

## 添加公钥到 Gitee

进入 头像 - 设置 - SSH 公钥 - 添加公钥，把上一步的公钥内容粘贴进去。

## 测试 SSH 连接

```bash
ssh -T git@gitee.com
```

返回 `Hi xxx! You've successfully authenticated` 即代表配置成功。

# 首次推送代码到远程仓库

在本地项目目录执行以下命令，把已有项目推到 Gitee（远程地址用上面新建仓库得到的地址，SSH 或 HTTPS 均可）：

```bash
# 1. 初始化本地仓库
git init

# 2. 添加所有文件到暂存区
git add .

# 3. 提交到本地仓库（-m 后面是提交说明，可自定义）
git commit -m "首次提交"

# 4. 添加远程仓库地址（替换为你自己的仓库地址）
git remote add origin git@gitee.com:你的用户名/项目名.git
# 或使用 HTTPS：git remote add origin https://gitee.com/你的用户名/项目名.git

# 5. 推送到 Gitee（-u 表示建立关联，之后可直接用 git push）
git push -u origin master
```

克隆已有仓库到本地：

```bash
git clone git@gitee.com:你的用户名/项目名.git
```

本地仓库与远程仓库的基础交互命令：

| 命令 | 说明 | 示例 |
|---|---|---|
| `git clone` | 克隆远程仓库到本地 | `git clone git@gitee.com:用户名/仓库名.git` |
| `git remote add` | 关联本地已有仓库到 Gitee 远程仓库 | `git remote add origin git@gitee.com:xxx/xxx.git` |
| `git push` | 推送本地提交到远程仓库 | `git push -u origin master`（-u 设置上游，以后直接 `git push`） |
| `git pull` | 拉取远程仓库最新代码并合并 | `git pull origin master` |
| `git fetch` | 仅拉取远程更新，不自动合并（安全操作） | `git fetch origin` |
| `git remote -v` | 查看当前仓库绑定的远程地址 | `git remote -v` |

# 日常 Git 工作流

## 每日开工：先拉取最新代码

目的：确保本地代码和远程仓库同步，减少冲突。

```bash
# 拉取当前分支的最新代码（推荐）
git pull
```

## 日常提交：本地保存工作进度

目的：把你的修改保存到本地仓库，暂不推送到远程。

```bash
# 1. 查看哪些文件被修改了
git status

# 2. 添加指定文件到暂存区
git add 文件名        # 添加单个文件
git add src/          # 添加整个文件夹
git add .             # 添加所有修改的文件（常用）

# 3. 提交到本地仓库（写清楚改了什么）
git commit -m "修复了登录页面的样式问题"

# 组合命令：添加所有修改并一次性提交（仅对已跟踪文件有效）
git commit -am "修复了登录页面的样式问题"
```

## 推送到远程

目的：把本地提交上传到 Gitee，供其他人拉取。

```bash
# 推送到当前分支的远程仓库
git push
```

如果推送被拒绝（远程有新提交、本地落后），先拉取再推送：

```bash
git pull --rebase origin master
git push
```

# 分支管理

目的：多人协作时各自在独立分支上开发，互不干扰。

| 命令 / 操作 | 说明 |
|---|---|
| `git branch` | 查看本地分支（`*` 表示当前所在分支） |
| `git branch -r` | 查看远程分支 |
| `git branch -a` | 查看所有分支（本地 + 远程） |
| `git branch 分支名` | 创建新分支（只创建，不切换） |
| `git checkout -b 分支名` | 创建并切换到新分支（常用） |
| `git checkout 分支名` | 切换分支 |
| `git switch 分支名` | 切换分支（Git 2.23+ 新命令） |
| `git merge 分支名` | 合并指定分支到当前分支，例如 `git merge feature-login` |
| `git branch -d 分支名` | 删除本地分支（已合并），未合并时用 `git branch -D 分支名` 强制删除 |
| `git push origin 分支名` | 推送本地分支到远程，例如 `git push origin feature-login` |
| `git push origin --delete 分支名` | 删除远程分支 |
| 保护分支 | Gitee 网页：管理 - 仓库设置 - 分支与标签保护，可禁止强制推送、要求 PR 合并、要求 CI 通过 |

# 附录：常见问题与排错

| 问题 | 原因与解决 |
|---|---|
| Push 报错 403 | 权限不足：检查 SSH 公钥是否配对、是否有该仓库的写权限 |
| Push 报错 rejected (non-fast-forward) | 远程有新提交、本地落后了：先 `git pull --rebase origin master` 再 push |
| Pull 报错 fatal: refusing to merge unrelated histories | 两个仓库没有共同祖先，强制合并加参数：`git pull origin master --allow-unrelated-histories` |
| 提交不计入绿点统计 | 本地 git config 的邮箱没在 Gitee 上绑定，修改邮箱后重新 amend 提交 |
| Gitee Pages 访问 404 | 未实名认证；或部署目录下没有 index.html |
| 克隆速度慢 | 确保用的是 SSH 协议克隆（`git@gitee.com...`），而非 HTTPS；或者换网络 |
| 大文件推送失败 | 单个文件超过 100MB，需用 Git LFS (Large File Storage) 管理 |

# Gitee 协作与进阶

## Pull Request（PR）代码审查流程

1. Fork 仓库：点击 Gitee 页面右上角 Fork 按钮，将别人仓库复制到自己账号下。
2. Clone 下来：`git clone` 自己的 Fork 仓库。
3. 新建分支：`git checkout -b fix-bug`。
4. 修改提交：`git add . && git commit -m "修复bug" && git push origin fix-bug`。
5. 发起 PR：在自己 Fork 的 Gitee 页面，点击 “Pull Requests” - “+ 新建 Pull Request”。
6. 指定目标：选择源分支（fix-bug）到目标分支（原作者的 master）。
7. 描述 PR：填写修改说明、关联的 Issue。
8. 代码审查：原仓库维护者查看代码差异，提出评审意见。
9. 合并 PR：维护者确认无误后点击合并。
10. 同步更新：Fork 后如果原仓库更新，需在 Gitee 页面点 “同步上游” 或用命令行同步。

## Issue 问题追踪

| 功能 | 说明 | 示例 |
|---|---|---|
| 新建 Issue | 仓库页面 - 问题 - 新建问题 | 描述 Bug 或新需求 |
| 模板 | 仓库设置中可配置 Issue 模板（Bug 报告、需求模板） | 创建 Issue 时自动加载模板 |
| 标签 | 给 Issue 打标签分类（Bug、文档、功能请求） | 红色标签：紧急 Bug |
| 指派人 | 将 Issue 分配给具体开发者处理 | 指派给 @张三 |
| 里程碑 | 将 Issue 归类到某个版本节点 | v1.0 版本、v2.0 版本 |
| 关联提交 | 在 commit 信息或 PR 中写 “Fix #1” 自动关闭对应 Issue | `git commit -m "Fix #1 修复登录问题"` |
| 关闭 Issue | 问题解决后手动关闭，或通过 PR 自动关闭 | 状态改为“已关闭” |

## 代码审查

| 操作 | 说明 |
|---|---|
| 审查变更 | 在 PR 页面查看文件差异 |
| 行内评论 | 在某一行代码上点击 “+” 添加评论，指出具体代码问题 |
| 总体评论 | 在 PR 最下方发表总体评审意见，针对设计层面的问题 |
| 请求修改 | 审查后打回，要求提交者修改后再审查，状态设为“有待改进” |
| 通过审查 | 确认代码无误，点击“审查通过”，随后可以合并 PR |

## 仓库管理与设置

| 设置路径 | 说明 | 注意事项 |
|---|---|---|
| 仓库转移 | 管理 - 仓库设置 - 仓库转移 | 转移给其他用户或组织（需对方同意） |
| 仓库删除 | 管理 - 仓库设置 - 删除本仓库 | 删除后不可恢复！ |
| 仓库成员 | 管理 - 仓库成员管理 - 添加仓库成员 | 可设置权限：开发者、管理员、只读 |
| Star 收藏 | 点击仓库页面上的 Star 按钮 | 类似点赞，表示支持 |
| Watch 关注 | 点击 Watch 按钮 | 该仓库有更新时会收到邮件通知 |
| Fork | 点击 Fork 按钮复制仓库到自己账号下 | 用于 PR 贡献代码 |
| WebHooks | 管理 - WebHooks 设置 - 添加 | 代码推送时触发 URL 回调（如触发 Jenkins、钉钉机器人） |
| Gitee 同步 | 仓库页面 - 同步按钮 | 将 GitHub 仓库的更新一键强制同步到 Gitee（解决两边代码不一致） |

## 高级协作与安全

| 功能 | 说明 |
|---|---|
| 分支保护 | 设置指定分支（如 master）禁止直接 push，必须走 PR 合并 |
| 签名提交 | Git 配置 GPG 密钥，提交带已验证标识，防止身份伪造 |
| 锁定文件 | 锁定重要文件（如 package.json）防止被其他人修改，防止依赖被恶意篡改 |
| 操作日志 | 管理 - 操作日志，记录谁在什么时间做了什么操作（审计） |
| IP 白名单 | 企业版功能，限制只有特定 IP 能通过 SSH / HTTPS 访问，安全防护 |
| 二次验证 | 企业版支持强制所有成员开启 2FA，企业安全合规要求 |

## Gitee Pages（免费静态网页托管）

| 操作 | 说明 |
|---|---|
| 开启服务 | 仓库 - 服务 - Gitee Pages - 启动 |
| 部署目录 | 选择要部署的分支和目录（通常是 master 分支的 `/` 或 `/docs`），用于部署 VuePress、Hexo、Vite 等静态站 |
| 访问域名 | 默认为 `https://用户名.gitee.io/仓库名`，需实名认证才能使用 |
| 自定义域名 | 在 Pages 设置中绑定自己的域名，并去 DNS 解析添加 CNAME 记录，如 `www.myblog.com` |
| 更新网站 | 往部署分支 push 代码，Gitee 自动重新构建（有几分钟延迟）：`git push origin master` |
| 与 GitHub Pages 区别 | Gitee Pages 需实名认证、有访问限制；国内访问极快；不支持 Jekyll |

## Gitee Go（CI/CD 持续集成/持续部署）

| 概念 | 说明 | 示例 |
|---|---|---|
| 流水线 | 自动化构建、测试、部署的流程定义 | 代码提交 - 自动跑单元测试 - 自动打包 - 自动部署到服务器 |
| 构建环境 | Gitee 提供的云端 Linux / Windows / Docker 运行环境 | 不需要自己配服务器 |
| .gitee-ci.yml | 在仓库根目录创建此文件定义流水线（语法类似 GitLab CI） | 见下方模板 |
| 触发条件 | 支持 Push、PR、定时任务触发 | 每次 push 到 master 自动触发 |
| 构建缓存 | 缓存 node_modules 等依赖，加速下次构建 | `cache: paths: - node_modules/` |
| 制品管理 | 构建产生的 jar 包、zip 包可以归档 | `artifacts: paths: - dist/` |

`.gitee-ci.yml` 模板（Node.js 前端项目构建）：

```yaml
name: 前端构建部署
stages:
  - install
  - build
  - deploy

install-job:
  stage: install
  image: node:16
  script:
    - npm install

build-job:
  stage: build
  image: node:16
  script:
    - npm run build
  artifacts:
    paths:
      - dist/

deploy-job:
  stage: deploy
  image: alpine:latest
  script:
    - echo "部署逻辑（如通过scp传到服务器）"
  only:
    - master
```
