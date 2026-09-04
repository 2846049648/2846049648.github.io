---
title: AI 工具与本地大模型
date: 2026-04-24
category: 学习记录
tags: ["ai", "ollama", "deepseek"]
excerpt: Ollama 本地模型、OpenClaw 工具与 Claude Code 接入 DeepSeek 配置
---
# Ollama 本地模型

Ollama 用于在本地跑大模型。以下是在 Linux（Ubuntu）上的部署记录。

## 部署步骤

1. 下载对应的解压工具：

```bash
apt update && apt install -y zstd
```

2. 使用官方安装脚本：

```bash
curl -fsSL https://ollama.com/install.sh | sh
```

3. 可用 AI 模型库网站：https://ollama.com/search

4. 启动服务：

```bash
ollama serve &
```

（`&` 表示后台运行。）

5. 在官网找到对应的 AI 模型，复制代码粘贴运行，例如：

```bash
ollama run qwen3.5:0.8b
```

## 常用命令

```bash
# 最常用命令
ollama run <模型名>    # 运行模型
ollama list            # 查看已安装模型
ollama pull <模型名>   # 下载模型
ollama rm <模型名>     # 删除模型
ollama serve           # 启动服务
```

## 快捷操作

```bash
ollama run llama2          # 交互对话
ollama run llama2 "Hello"  # 单次生成（带提示）
ollama cp llama2 my-model  # 创建别名
ollama show llama2         # 查看模型详情
```

# OpenClaw

OpenClaw（小龙虾）的使用记录。安装与操作基本都需要 root 权限。

## 下载安装

Linux 使用安装脚本：

```bash
curl -fsSL https://openclaw.ai/install.sh | bash
```

Windows 使用安装脚本：

```powershell
iwr -useb https://openclaw.ai/install.ps1 | iex
```

## 控制台与新手引导

```bash
openclaw dashboard            # 打开控制 UI
openclaw onboard --install-daemon   # 新手引导
```

## 设置与配置

```bash
openclaw gateway run   # 打开网关，启动服务
openclaw config        # 进行配置
openclaw models set AI名字   # 配置 AI
```

`openclaw models --help` 可查看具体命令。在 AI 聊天中设置了运行命令，例如 `/models AI名字`，可用来更换使用的 AI。

# Claude Code 接入 DeepSeek

利用 DeepSeek 提供的 Anthropic 兼容端点，把 Claude Code 指向 DeepSeek 的模型来使用。

## 安装 Claude Code 与 Node

Claude Code 通过 npm 全局安装：

```bash
npm install -g @anthropic-ai/claude-code
```

如果还没有安装 npm（Linux 下通过 NodeSource 脚本安装 Node.js，npm 会自动随之安装）：

```bash
# 1. 下载并执行 NodeSource 安装脚本（LTS 版本）
curl -fsSL https://deb.nodesource.com/setup_lts.x | sudo -E bash -

# 2. 安装 Node.js（npm 会自动随之安装）
sudo apt-get install -y nodejs
```

## 配置 settings.json

Claude Code 的配置文件位于 `C:\Users\你的用户名\.claude\settings.json`。把其中的 `env` 改为下面这样，即可通过 Anthropic 兼容端点（`ANTHROPIC_BASE_URL`）把请求转发到 DeepSeek，并在 `ANTHROPIC_MODEL` 及各 `ANTHROPIC_DEFAULT_*_MODEL` 中指定要用的 DeepSeek 模型。

使用推理模型（deepseek-reasoner）的配置：

```json
{
  "env": {
    "ANTHROPIC_AUTH_TOKEN": "sk-你的DeepSeek密钥",
    "ANTHROPIC_BASE_URL": "https://api.deepseek.com/anthropic",
    "ANTHROPIC_MODEL": "deepseek-reasoner",
    "ANTHROPIC_DEFAULT_OPUS_MODEL": "deepseek-reasoner",
    "ANTHROPIC_DEFAULT_SONNET_MODEL": "deepseek-reasoner",
    "ANTHROPIC_DEFAULT_HAIKU_MODEL": "deepseek-reasoner",
    "CLAUDE_CODE_SUBAGENT_MODEL": "deepseek-reasoner",
    "CLAUDE_CODE_MAX_OUTPUT_TOKENS": "32000"
  },
  "permissions": {
    "allow": [],
    "deny": []
  },
  "alwaysThinkingEnabled": false
}
```

使用 chat 模型（成本低）的配置：默认模型和各 Sonnet/Haiku 映射用 `deepseek-chat`，Opus 仍可保留 `deepseek-reasoner`：

```json
{
  "env": {
    "ANTHROPIC_AUTH_TOKEN": "sk-xxx",
    "ANTHROPIC_BASE_URL": "https://api.deepseek.com/anthropic",
    "ANTHROPIC_MODEL": "deepseek-chat",
    "ANTHROPIC_DEFAULT_OPUS_MODEL": "deepseek-reasoner",
    "ANTHROPIC_DEFAULT_SONNET_MODEL": "deepseek-chat",
    "ANTHROPIC_DEFAULT_HAIKU_MODEL": "deepseek-chat",
    "CLAUDE_CODE_SUBAGENT_MODEL": "deepseek-chat",
    "CLAUDE_CODE_MAX_OUTPUT_TOKENS": "32000"
  },
  "permissions": {
    "allow": [],
    "deny": []
  },
  "alwaysThinkingEnabled": false
}
```

配置完成后重启 Claude Code，即可用 DeepSeek 作为底层模型进行对话与编码。
