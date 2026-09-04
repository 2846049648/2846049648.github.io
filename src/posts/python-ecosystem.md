---
title: Python 生态：虚拟环境与常用库
date: 2026-06-04
category: 学习记录
tags: ["python", "venv", "conda", "poetry", "fastapi", "爬虫"]
excerpt: venv/conda/poetry 环境管理，FastAPI、NetworkX、bitarray 与爬虫库
---

# Python 环境管理

Python 项目一多，"依赖打架"就成了最头疼的问题。每个项目应该用独立的第三方包环境隔离开，这是标准做法。主流的隔离方案有三种：**venv**、**Conda** 与 **Poetry**。

- **venv**：Python 官方内置的轻量级虚拟环境，只管 Python 包；
- **Conda**：重量级环境管理器，能装 Python 解释器本身和非 Python 库（如 C/C++ 库、CUDA）；
- **Poetry**：项目依赖管理与打包工具，底层自动调用 venv 来创建环境，并锁定精确版本。

## venv：Python 官方虚拟环境

### 基本概念与定位

| 概念 | 说明 |
|---|---|
| venv | virtual environment（虚拟环境），Python 官方内置的标准库模块（Python 3.3+ 自带） |
| 核心作用 | 为每个项目创建独立的 Python 运行环境，隔离项目之间的第三方依赖（包） |
| 解决痛点 | 项目 A 用 Django 2.0，项目 B 用 Django 4.0，不隔离的话会互相冲突报错 |
| 与 virtualenv 区别 | virtualenv 是早期的第三方工具（需 pip install）；venv 是官方内置，直接用，性能更好 |
| 与 Conda 区别 | Conda 是重量级环境管理器（能装 Python 本身和非 Python 库如 C++ 库）；venv 是轻量级，只管 Python 包 |
| 与 Poetry 区别 | Poetry 是项目依赖管理和打包工具，底层自动调用 venv；venv 是底层基础 |
| 工作原理 | 在项目目录下生成一个文件夹（通常叫 venv），里面放一份当前 Python 解释器的软链接和 pip，安装的包都落在其中的 site-packages 里 |
| 核心原则 | 绝对不要把虚拟环境文件夹提交到 Git，必须加入 .gitignore |

### 为什么必须用虚拟环境

**反面教材（不用 venv）**：系统全局装了 requests 2.0。你接手一个老项目需要 requests 1.0，于是执行 `pip install requests==1.0`，全局的 requests 被降级了。结果：电脑上其他 10 个依赖 requests 2.0 的项目全部跑不起来。

**正面教材（用 venv）**：进入项目 A 目录创建 venv，里面装 requests 2.0；进入项目 B 目录创建 venv，里面装 requests 1.0。两个环境完全隔离，互不干扰，系统 Python 环境干干净净。

### 创建虚拟环境

| 命令 | 说明 | 示例 |
|---|---|---|
| `python -m venv venv` | 在当前目录下创建名为 "venv" 的虚拟环境文件夹 | `python -m venv venv` |
| `python -m venv .venv` | 以点开头的隐藏文件夹（推荐，更明显这是临时环境） | `python -m venv .venv` |
| `python -m venv env` | 自定义文件夹名字 | `python -m venv my_env` |
| `python3 -m venv venv` | Linux/Mac 上明确用 python3（防止调用到系统旧版 python2） | `python3 -m venv venv` |
| `python -m venv --clear venv` | 如果 venv 已存在，强制清除后重新创建 | `python -m venv --clear venv` |
| `python -m venv --prompt myproject venv` | 自定义激活后命令行前缀显示的名字（默认是文件夹名） | `python -m venv --prompt "myapp" venv` |

注意两点：创建时用的是哪个 Python 版本，虚拟环境内部就是哪个版本（用 python3.11 创建，里面就是 3.11）；虚拟环境文件夹不要放在需要 sudo 权限的目录（如 `/usr/local/`），放在用户目录或项目目录下即可。

### 激活虚拟环境

| 操作系统 | 激活命令 | 激活后效果 |
|---|---|---|
| Windows (CMD) | `venv\Scripts\activate.bat` | 命令行前面出现环境名 |
| Windows (PowerShell) | `venv\Scripts\Activate.ps1` | 命令行前面出现环境名 |
| Linux / macOS | `source venv/bin/activate` | 命令行前面出现环境名 |

验证是否激活成功：输入 `which python`（Linux/Mac）或 `where python`（Windows），路径应指向 venv 文件夹内部的 python，而不是系统全局路径。

忘记激活的后果：`pip install` 的包会全部装到系统全局环境，没有起到隔离作用。所以每次开终端必须先激活。

### Windows PowerShell 激活报错排错

报错内容：`venv\Scripts\Activate.ps1` 无法加载，因为在此系统上禁止运行脚本。

原因：Windows 默认执行策略是 Restricted，禁止运行任何 PS1 脚本。

| 方法 | 操作 |
|---|---|
| 方法 1（推荐） | 以管理员身份打开 PowerShell，执行 `Set-ExecutionPolicy RemoteSigned -Scope CurrentUser` |
| 方法 2（临时） | 在当前普通 PowerShell 执行 `Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass` |
| 方法 3（绕过） | 不用 PowerShell，直接用 CMD 激活：`venv\Scripts\activate.bat` |

验证策略修改：执行 `Get-ExecutionPolicy`，返回 `RemoteSigned` 即可。

### 退出与删除虚拟环境

| 操作 | 命令 | 说明 |
|---|---|---|
| 退出虚拟环境 | `deactivate` | 取消环境变量修改，回到系统全局 Python 环境（仅退出，不删除文件） |
| 删除虚拟环境 | 直接在文件管理器删除 venv 文件夹，或 `rm -rf venv` | 虚拟环境没有卸载命令，就是纯粹删文件夹 |
| 重新创建 | 删除后重新执行 `python -m venv venv` | 环境坏了或包装乱了，直接删了重建是最快的方法 |

### 在虚拟环境中安装包

| 命令 | 说明 | 示例 |
|---|---|---|
| `pip install 包名` | 在激活的 venv 中安装包（只会装到 venv 里） | `pip install requests` |
| `pip install -r requirements.txt` | 批量安装依赖文件中的所有包 | `pip install -r requirements.txt` |
| `pip list` | 查看当前 venv 中安装了哪些包 | `pip list` |
| `pip freeze` | 以标准格式输出已安装包及版本号（用于导出） | `pip freeze` |
| `pip freeze > requirements.txt` | 将当前环境依赖导出到文件（给别人用） | `pip freeze > requirements.txt` |
| `pip show 包名` | 查看某个包的详细安装路径和信息 | `pip show flask` |
| `pip uninstall 包名` | 卸载包 | `pip uninstall flask` |

注意：不要用 `sudo pip install`，也不要用 `python -m pip`；激活后直接用 `pip` 即可，否则 sudo 会把包装到系统 root 环境。

### 依赖管理：requirements.txt 工作流

完整工作流（团队协作必备）：

1. 你在本地 venv 开发，装了 flask、redis、requests；
2. 导出依赖：`pip freeze > requirements.txt`；
3. 把代码和 requirements.txt 一起提交到 Git；
4. 同事拉取代码；
5. 同事创建 venv：`python -m venv venv`；
6. 同事激活 venv：`source venv/bin/activate`；
7. 同事一键安装依赖：`pip install -r requirements.txt`；
8. 同事的环境和你完全一致，不会报版本错误。

### IDE 集成配置

| IDE | 配置方法 | 验证标志 |
|---|---|---|
| VS Code | 1. 按 `Ctrl+Shift+P`；2. 输入 "Python: Select Interpreter"；3. 选择 venv 文件夹里的 python.exe | 左下角状态栏显示 "Python 3.x.x ('venv': venv)" |
| PyCharm | 1. File -> Settings -> Project -> Python Interpreter；2. 点击齿轮 -> Add；3. 选 "Existing environment" -> 浏览选择 venv/bin/python；4. 勾选 "Make available to all projects"（可选） | 右下角或设置里看到路径指向 venv |
| Cursor | 同 VS Code（基于 VS Code 内核） | 同 VS Code |

PyCharm 小贴士：PyCharm 默认就采用 venv 虚拟环境对每个项目做环境隔离，各项目使用的虚拟环境和包互不干扰，只要在创建/配置项目时把解释器指向当前项目的 venv 即可。

另外两点注意：VS Code 有时会自动识别 venv 并提示你选择，点提示的按钮即可；如果 IDE 没识别到，代码里 import 第三方库会画红线报错但实际能运行，说明 IDE 用的解释器不对。

### 在虚拟环境中直接运行脚本

| 方式 | 命令 | 说明 |
|---|---|---|
| 激活后运行 | 先 activate，再 `python main.py` | 最常用，推荐 |
| 不激活直接调用 | `venv\Scripts\python.exe main.py`（Windows）<br>`venv/bin/python main.py`（Linux） | 适合写启动脚本或 CI/CD 流水线 |
| 不激活调用模块 | `venv\Scripts\python.exe -m http.server 8080` | 同理，直接指定 venv 里的解释器 |
| Windows 批处理 | start.bat：`call venv\Scripts\activate.bat` + `python main.py` + `pause` | 双击 bat 文件自动激活并运行 |
| Linux/Mac Shell 脚本 | run.sh：`#!/bin/bash` + `source venv/bin/activate` + `python main.py` | `./run.sh` 直接运行 |

注意：激活后不要用 `python3 main.py`（Linux 上 python3 可能指向系统 Python，没走 venv）；激活后 `python` 就是 venv 里的 python。

### venv 文件夹结构解析

```text
venv/
├── pyvenv.cfg              # 虚拟环境配置文件（记录原 Python 路径、是否包含系统包等）
├── Scripts/                # Windows 下的可执行文件目录
│   ├── activate.bat        # CMD 激活脚本
│   ├── Activate.ps1        # PowerShell 激活脚本
│   ├── python.exe          # Python 解释器软链接（指向系统 Python）
│   └── pip.exe             # pip 包管理器
├── bin/                    # Linux/Mac 下的可执行文件目录
│   ├── activate            # Shell 激活脚本
│   ├── python -> /usr/bin/python3.11   # 软链接
│   └── pip
├── Lib/                    # Windows 下第三方包安装位置
│   └── site-packages/      # 你 pip install 的包都在这里
└── lib/                    # Linux/Mac 下第三方包安装位置
    └── python3.11/
        └── site-packages/
```

### 高级进阶用法

| 命令/配置 | 说明 | 示例 |
|---|---|---|
| `--system-site-packages` | 创建时包含系统全局已安装的包（默认不包含） | `python -m venv --system-site-packages venv` |
| 修改 pyvenv.cfg | 直接编辑配置文件，手动改 `include-system-site-packages = true` | 不重建环境的情况下修改行为 |
| 升级 pip | 新创建的 venv 里的 pip 可能不是最新版，建议先升级 | `pip install --upgrade pip` |
| 换 Python 版本重建 | 如果项目需要 Python 3.10 但系统是 3.11，venv 无法降级 Python 本身 | 先安装 Python 3.10，再用 `py -3.10 -m venv venv` 创建 |
| uv 替代方案（前沿） | 用 Rust 写的极速 Python 包管理器，创建 venv 比官方快 10-100 倍 | `pip install uv && uv venv` |

### venv 与 Docker 的关系

| 场景 | 用不用 venv |
|---|---|
| 在 Docker 容器里开发 | 通常不用（容器本身就是隔离环境，直接装全局即可） |
| 在 Dockerfile 里 | 通常也不用（每层都是隔离的） |
| 本地开发然后打包 Docker | 必须用 venv（本地隔离依赖，Docker 里再装一遍） |
| 最佳实践 | Dockerfile 里写 `COPY requirements.txt . && pip install -r requirements.txt`（不用建 venv） |

### 常见问题与排错（FAQ）

| 问题 | 原因与解决 |
|---|---|
| `pip install` 报权限错误 | 没有激活虚拟环境，装到了系统 Python 目录（如 `C:\Program Files\`）。激活 venv 后再安装 |
| 激活后 python 版本不对 | 创建 venv 时用的 Python 版本不是你想要的。删除 venv，用指定版本的 Python 重新创建 |
| 导入模块报 ModuleNotFoundError | 1. 没激活 venv；2. 在 A 项目的 venv 里运行了 B 项目的代码。确认激活了对应项目的 venv，且装了依赖 |
| VS Code 代码画红线但能运行 | IDE 选择的 Python 解释器不是当前 venv 里的。`Ctrl+Shift+P` -> Python: Select Interpreter -> 选 venv 里的 |
| 删除 venv 后终端还显示环境名 | 只是没有执行 deactivate 退出环境。执行 `deactivate` 即可恢复 |
| Mac/Linux 提示 command not found: python | 系统没有 python 命令，只有 python3。用 `python3 -m venv venv` 创建，激活后用 `python3` 运行 |
| 装包极慢 | 默认 pip 源在国外，需换国内镜像：`pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple` |

### 实战：项目完整初始化标准流程

```bash
# 1. 创建项目目录
mkdir my_project && cd my_project

# 2. 初始化 Git
git init

# 3. 创建并写入 .gitignore（极其重要！）
echo "venv/" >> .gitignore
echo "__pycache__/" >> .gitignore
echo "*.pyc" >> .gitignore
echo ".env" >> .gitignore

# 4. 创建虚拟环境
python -m venv venv

# 5. 激活虚拟环境
# Windows PowerShell:
.\venv\Scripts\Activate.ps1
# Linux/Mac:
# source venv/bin/activate

# 6. 升级 pip
pip install --upgrade pip

# 7. 安装项目依赖
pip install flask requests

# 8. 导出依赖文件
pip freeze > requirements.txt

# 9. 开始写代码
touch app.py

# 10. 提交到 Git（venv 已被忽略，不会上传）
git add .
git commit -m "init project with venv"
```

## Conda：环境与包管理器

### 基本概念与核心定位

| 概念 | 说明 |
|---|---|
| Conda | 开源的跨平台环境与包管理器，由 Anaconda 公司开发（最初为 Python 数据科学生态而生） |
| Anaconda | 一个发行版 = Conda + Python + 180+ 预装科学计算包（Numpy/Pandas/Matplotlib 等），约 4-5GB |
| Miniconda | 轻量级发行版 = Conda + Python + 最少包（约 400MB），推荐！只装你需要的东西 |
| Mamba（补充） | 用 C++ 重写的 Conda 并行下载引擎，速度比 Conda 快 10 倍以上（`conda install mamba` 后可用） |
| 核心能力 | Conda 不只是 Python 包管理器！它能装 Python 解释器本身、R 语言包、Node.js、C/C++ 库（如 CUDA/cuDNN） |
| 与 venv 区别 | venv 只能管理 Python 第三方包（pip 装的），Conda 能管理 Python 版本本身 + 非 Python 库 |
| 工作原理 | Conda 有自己的包仓库，包格式为 .conda / .tar.bz2，存储在 envs/ 和 pkgs/ 目录中 |
| 使用场景 | 数据科学、机器学习、深度学习、科学计算（PyTorch/TensorFlow 依赖复杂的 C/CUDA 库，Conda 处理最好） |

### Anaconda vs Miniconda vs venv+pip 对比

| 对比维度 | Anaconda | Miniconda | venv + pip |
|---|---|---|---|
| 安装大小 | 4~5GB | ~400MB | 0（Python 自带） |
| 自带 Python | 自带 Python 3.x | 自带 Python 3.x | 使用系统已安装的 Python |
| 自带包 | 180+ 科学计算包 | 几乎没有包 | 没有 |
| 自带 Conda | 有 | 有 | 无 |
| 能否装 Python 版本本身 | 能（`conda create -n py310 python=3.10`） | 能 | 不能（venv 不能切换 Python 版本） |
| 能否装非 Python 库 | 能（如 CUDA/cuDNN/FFmpeg） | 能 | 不能 |
| 包数量 | conda-forge 仓库极大 | 同左 | PyPI 仓库最大（纯 Python 包最多） |
| 速度 | 中等 | 中等 | pip 通常更快（纯 Python 包） |
| 推荐人群 | 零基础小白/数据科学入门 | 所有有经验的开发者 | 纯 Web 开发/脚本开发 |
| 一句话总结 | 开箱即用但臃肿 | 轻量干净，按需安装 | Python 官方标准方案，最轻量 |

### 安装与配置

| 步骤 | 命令/操作 | 说明 |
|---|---|---|
| 1. 下载 Miniconda | https://docs.conda.io/en/latest/miniconda.html | 推荐装 Miniconda 而非 Anaconda |
| 2. Windows 安装 | 双击 exe，一路默认安装（建议路径 `C:\Users\xxx\miniconda3`） | 安装时建议勾选 "Add to PATH"（但不推荐，见下方） |
| 3. Linux 安装 | `bash Miniconda3-latest-Linux-x86_64.sh` | 一路回车 + yes |
| 4. Mac 安装 | `bash Miniconda3-latest-MacOSX-x86_64.sh` | 同理 |
| 5. 初始化 Shell | `conda init bash` / `conda init powershell` / `conda init zsh` | 将 conda 集成到你的 Shell 中 |
| 6. 重启终端 | 关闭并重新打开终端 | `conda activate` 命令才会生效 |
| 7. 验证安装 | `conda --version` / `conda info` | 显示版本号和路径信息 |

关于 PATH：强烈建议不要手动把 Conda 加到系统 PATH，用 `conda init` 自动管理，避免与其他 Python 环境冲突。卸载：直接删除 Miniconda 安装文件夹，再清理 `~/.condarc` 和 `.bashrc` 中的 conda 初始化行。

### 换国内镜像源（国内必做，否则极慢）

```bash
# 查看当前配置
conda config --show

# 清除默认配置（可选）
conda config --remove-key channels

# 添加清华镜像源（推荐）
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main/
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/conda-forge/

# 设置搜索时也显示镜像地址
conda config --set show_channel_urls yes
```

或直接编辑 `~/.condarc` 文件（推荐，最干净）：

```yaml
channels:
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/conda-forge/
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main/
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
  - defaults
show_channel_urls: true
```

pip 也要换镜像（在 Conda 环境里）：

```bash
pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple
```

恢复默认源：

```bash
conda config --remove-key channels
conda config --add channels defaults
```

### 基础环境管理命令

| 命令 | 说明 | 示例 |
|---|---|---|
| `conda create -n name` | 创建新的虚拟环境（默认用当前 Conda 的 Python 版本） | `conda create -n myenv` |
| `conda create -n name python=3.10` | 创建指定 Python 版本的环境 | `conda create -n py310 python=3.10` |
| `conda create -n name python=3.10 numpy pandas` | 创建环境的同时安装包 | `conda create -n ds python=3.11 numpy pandas matplotlib` |
| `conda create -n name --clone old_env` | 克隆已有环境（完全复制一份） | `conda create -n backup --clone myenv` |
| `conda env create -f env.yml` | 从 yml 文件创建环境（团队协作标准方式） | `conda env create -f environment.yml` |
| `conda activate name` | 激活环境（类似 venv 的 source activate） | `conda activate myenv` |
| `conda deactivate` | 退出当前环境 | `conda deactivate` |
| `conda remove -n name --all` | 删除指定环境（连同所有包一起删除） | `conda remove -n old_env --all` |
| `conda env list` / `conda info --envs` | 列出所有已创建的环境 | `conda env list` |
| `conda env export > env.yml` | 导出当前环境配置到 yml 文件 | `conda env export > environment.yml` |
| `conda env update -f env.yml` | 根据 yml 文件更新当前环境 | `conda env update -f environment.yml` |

注意：激活后命令行前缀会变成环境名。

### 包管理命令

| 命令 | 说明 | 示例 |
|---|---|---|
| `conda install 包名` | 在当前环境安装包 | `conda install numpy` |
| `conda install 包名=版本` | 安装指定版本的包 | `conda install numpy=1.24.0` |
| `conda install 包1 包2` | 同时安装多个包 | `conda install flask requests redis` |
| `conda install -c channel 包名` | 从指定 channel 安装包 | `conda install -c conda-forge opencv` |
| `conda update 包名` | 更新指定包 | `conda update numpy` |
| `conda update --all` | 更新当前环境所有包 | `conda update --all` |
| `conda remove 包名` | 卸载指定包 | `conda remove numpy` |
| `conda list` | 列出当前环境所有已安装的包 | `conda list` |
| `conda search 包名` | 搜索包（在所有已配置 channel 中搜索） | `conda search pytorch` |
| `conda search 包名=版本` | 搜索指定版本 | `conda search pytorch=2.0` |
| `conda clean --all` | 清理所有缓存（下载的包和解压文件，可节省大量磁盘） | `conda clean --all` |
| `conda clean -p` | 清理 tar 包缓存 | `conda clean -p` |
| `conda clean -t` | 清理临时缓存文件 | `conda clean -t` |

### 在 Conda 环境里使用 pip（混合使用的规则）

| 规则 | 说明 | 示例 |
|---|---|---|
| 能用 conda 装的优先用 conda | conda 能解决 C/CUDA 等底层依赖，pip 不能 | `conda install pytorch`（自带 CUDA 库） |
| conda 装不了的用 pip | 有些纯 Python 包只在 PyPI 上有，conda 仓库没有 | `pip install transformers` |
| 先装 conda 包再装 pip 包 | 确保 Conda 管理的底层库先就位 | pip 装的包可能依赖 conda 装的 C 库 |
| 不要用 pip 装 conda 已安装的包 | 会导致依赖冲突，conda list 里会显示 pip 标记 | 如果 numpy 是 conda 装的，别用 `pip install numpy` 覆盖 |
| conda 和 pip 查看冲突 | `conda list` 会显示用 pip 装的包后面带 pypi 标记 | `conda list \| grep pypi` |
| 最佳实践 | `pip install` 前先用 `pip install --upgrade pip` 确保 pip 最新 | - |

### environment.yml 配置文件（团队协作必备）

```yaml
# environment.yml 标准模板
name: my_project        # 环境名称
channels:               # 通道列表
  - conda-forge
  - defaults
dependencies:           # 依赖列表
  - python=3.11
  - numpy=1.24
  - pandas
  - matplotlib
  - scikit-learn
  - flask
  - pip                # 声明要使用 pip 子依赖
  - pip:               # pip 安装的包写在下面
    - transformers==4.30.0
    - requests>=2.28
    - jupyter
```

使用流程：

```bash
# 1. 创建环境
conda env create -f environment.yml
# 2. 激活环境
conda activate my_project
# 3. 修改后重新导出
conda env export > environment.yml
# 4. 更新环境
conda env update -f environment.yml
```

### Channel（通道/源）管理

| 概念 | 说明 | 示例 |
|---|---|---|
| defaults | Conda 默认通道（Anaconda 官方维护） | `conda install numpy`（默认从 defaults 拉取） |
| conda-forge | 社区维护的最大通道（包最新最全，推荐！很多包只有这里有） | `conda install -c conda-forge opencv` |
| bioconda | 生物信息学专用通道 | `conda install -c bioconda samtools` |
| pytorch | PyTorch 官方通道（含 CUDA 版 PyTorch） | `conda install pytorch torchvision -c pytorch` |
| 查看已配置的 channel | `conda config --show channels` | - |
| 添加 channel | `conda config --add channels conda-forge` | 永久添加，写入 ~/.condarc |
| 删除 channel | `conda config --remove channels conda-forge` | - |
| 设为默认搜索通道 | `conda config --set channel_priority strict` | strict = 只从第一个有包的 channel 安装 |
| channel 优先级 | 默认 flexible（弹性）；strict（严格，推荐，避免混合安装冲突） | `conda config --set channel_priority strict` |

### 深度学习环境搭建（Conda 最强场景）

```bash
# PyTorch + CUDA 环境（Conda 最擅长的场景）
# 创建环境并指定 Python 版本
conda create -n pytorch_env python=3.10
conda activate pytorch_env

# 安装 PyTorch（Conda 自动处理 CUDA toolkit 依赖，不需要手动装！）
conda install pytorch torchvision torchaudio pytorch-cuda=11.8 -c pytorch -c nvidia
# 或指定 CUDA 12.1:
# conda install pytorch torchvision torchaudio pytorch-cuda=12.1 -c pytorch -c nvidia

# 验证 GPU 可用
python -c "import torch; print(torch.cuda.is_available())"

# TensorFlow + CUDA 环境
conda create -n tf_env python=3.10
conda activate tf_env
conda install tensorflow-gpu -c conda-forge
python -c "import tensorflow as tf; print(tf.config.list_physical_devices('GPU'))"
```

### Mamba 加速（Conda 的提速神器）

| 概念 | 说明 |
|---|---|
| Mamba | Conda 的 C++ 重写替代品，并行下载 + 并行解包，速度提升 5-20 倍 |
| 安装 | `conda install mamba -c conda-forge`（在 base 环境装一次即可） |
| 使用 | 所有 `conda install` 命令替换为 `mamba install` 即可：`mamba install numpy pandas matplotlib` |
| 创建环境 | `mamba create -n myenv python=3.11 numpy` |
| 更新环境 | `mamba update --all` |
| 移除包 | `mamba remove numpy` |
| Mambaforge | Miniconda 的 Mamba 版本（预装 Mamba，开箱即用）：https://github.com/conda-forge/miniforge/releases |

### Jupyter Notebook/Lab 集成

| 操作 | 命令/说明 |
|---|---|
| 在 Conda 环境里装 Jupyter | `conda install jupyter` / `conda install jupyterlab` |
| 安装 nb_conda 内核扩展 | `conda install nb_conda`（让 Jupyter 自动识别所有 Conda 环境），安装后 Jupyter 的"新建"里能看到所有 Conda 环境 |
| 手动注册内核 | `python -m ipykernel install --user --name=myenv --display-name="Python (myenv)"` |
| 切换内核 | Jupyter 界面：Kernel -> Change Kernel -> 选择对应环境 |
| Jupyter Lab | `conda install jupyterlab`（Jupyter 的下一代 IDE，更强大），`jupyter lab` |
| 在指定环境启动 | 先 `conda activate` 对应环境，再 `jupyter notebook` / `jupyter lab` |

### 常见问题与排错（FAQ）

| 问题 | 原因与解决 |
|---|---|
| conda: command not found | 安装后没执行 `conda init` / 没重启终端 / 没加到 PATH。执行 `conda init bash` 后重启终端 |
| 激活报错 ProxyConnectionError | 网络问题，需换国内镜像源。编辑 ~/.condarc 换清华源 |
| conda install 极慢 | 没换镜像 / 默认源在国外。换清华或阿里云镜像 |
| Solving environment 卡很久 | Conda 的依赖解析算法较慢（特别是包多时）。用 mamba 替代；或用 `conda install --no-deps` 只装不解析依赖 |
| ResolvePackageNotFound | 指定版本的包在当前 channel 中不存在。换 channel 试试：`conda install -c conda-forge 包名` |
| pip 和 conda 包冲突 | 同一个包被 conda 和 pip 各装了一份。卸载冲突的包，统一用一种方式安装 |
| conda 环境占磁盘太大 | pkgs/ 目录缓存了大量下载的包。执行 `conda clean --all` 清理缓存 |
| 删除环境报错 EnvironmentNotWritable | 有进程正在使用该环境中的文件。先 `conda deactivate` 再删除 |
| 创建环境指定 Python 版本失败 | 该版本在当前 channel 中不存在。换 channel：`conda create -n py39 python=3.9 -c conda-forge` |
| Conda 装的 PyTorch 检测不到 GPU | CUDA 版本不匹配或驱动太旧。用 `nvidia-smi` 看驱动版本，重新安装匹配的 pytorch-cuda=版本号 |

### Conda 与 venv 的选择建议

| 场景 | 推荐用 Conda | 推荐用 venv |
|---|---|---|
| 语言 | Python + 需要 C/CUDA 等底层库 | 纯 Python Web/脚本开发 |
| 框架 | PyTorch / TensorFlow / CUDA 相关 | Flask / Django / FastAPI / requests |
| 数据科学 | Numpy / Pandas / Scikit-learn / Matplotlib | 不需要科学计算 |
| Python 版本管理 | 需要多版本 Python 切换 | 只有一个 Python 版本 |
| 团队 | 数据科学团队 | Web 开发团队 |
| 部署 | 本地开发/实验（生产部署用 Docker） | 生产部署（Docker/venv） |

可以同时用吗？可以！Conda 环境里也能用 pip，venv 也能用 Conda 装的 Python 创建。

### 实战：数据科学项目完整初始化流程

```bash
# 1. 创建环境
conda create -n my_ds_project python=3.11

# 2. 激活环境
conda activate my_ds_project

# 3. 升级 pip
pip install --upgrade pip

# 4. 安装核心科学计算包（优先用 conda 装，自动处理底层依赖）
conda install numpy pandas matplotlib scikit-learn jupyter -c conda-forge

# 5. 安装深度学习框架（Conda 自动处理 CUDA）
conda install pytorch torchvision torchaudio pytorch-cuda=11.8 -c pytorch -c nvidia

# 6. 用 pip 装 conda 没有的纯 Python 包
pip install transformers datasets tqdm

# 7. 导出环境配置
conda env export > environment.yml

# 8. 提交到 Git
git add environment.yml
git commit -m "add conda environment config"
git push
```

同事复现流程：

```bash
# 1. 从配置文件创建环境
conda env create -f environment.yml
# 2. 激活环境
conda activate my_ds_project
# 3. 开始工作
jupyter notebook
```

## Poetry：项目依赖管理与虚拟环境

简单来说，Poetry 就是 Python 世界的"高级包管理器和环境管理器"，可以理解成 Python 版的 `npm`（Node.js）或 `yarn`。它专门解决 Python 项目中最让人头疼的两个问题：依赖管理，以及虚拟环境管理。

### 它解决的两个核心问题

**1. 依赖管理（这个项目需要装哪些第三方库？）**

普通 Python 项目会给你一个 `requirements.txt`，用 `pip install -r requirements.txt` 来安装。但 `requirements.txt` 有个大问题：它不锁定版本。今天你装的 `requests` 是 2.28 版，明天别人装的可能就是 2.31 版，万一新版改了接口，项目就跑不起来了。

Poetry 用一个 `pyproject.toml` 文件代替了 `requirements.txt`，并且会自动生成一个 `poetry.lock` 文件，精确锁定每一个依赖的版本号，确保所有人、所有环境下装的库一模一样。

**2. 虚拟环境管理（这个项目的库会不会和别的项目冲突？）**

假设项目 A 需要 Django 3.x，项目 B 需要 Django 4.x，如果都装在系统全局的 Python 里，必定打架。以前你得手动用 `virtualenv` 或 `conda` 创建隔离环境，Poetry 把这个操作自动化了：

- `poetry install`：自动创建虚拟环境 + 自动装依赖；
- `poetry shell`：自动进入这个隔离环境。

### Poetry vs 传统方式

| 操作 | 传统方式 | Poetry |
|---|---|---|
| 创建虚拟环境 | `python3 -m venv venv` | 自动完成 |
| 激活虚拟环境 | `source venv/bin/activate` | `poetry shell` |
| 安装依赖 | `pip install -r requirements.txt` | `poetry install` |
| 添加新依赖 | 手动改 requirements.txt | `poetry add requests` |
| 版本锁定 | 没有 | `poetry.lock` 自动锁定 |

### 常用命令速查

很多时候不需要深入学 Poetry，记住这三条命令就够了：

```bash
poetry install    # 一键装好所有依赖（自动创建虚拟环境 + 安装依赖）
poetry shell      # 进入项目环境（自动进入隔离环境）
poetry add <包名> # 添加新依赖，例如 poetry add requests
```

# Web 开发：FastAPI

FastAPI 是面向 API 设计的现代高性能 Python Web 框架，基于 Starlette（ASGI）与 Pydantic，由 Sebastian Ramirez 于 2018 年发布。它自带数据验证、自动生成 API 文档、原生异步支持，性能在 TechEmpower 基准测试中接近 Go 和 Node.js，比 Flask 快 3-5 倍。

## 基本概念与核心定位

| 概念 | 说明 |
|---|---|
| FastAPI | 现代、高性能的 Python Web 框架，专为构建 API 设计（由 Sebastian Ramirez 开发，2018 年发布） |
| 核心特点 | 极快（性能媲美 Node.js 和 Go，基于 Starlette + Pydantic）、自动生成 API 文档、原生异步支持、类型提示驱动 |
| 性能基准 | 基于 ASGI 的 Starlette，比 Flask 快 3-5 倍（TechEmpower 基准测试中接近 Go 和 Node.js） |
| Pydantic | 数据验证和序列化库（FastAPI 核心依赖），用 Python 类型注解自动验证请求体/参数 |
| Starlette | 轻量级 ASGI 框架（FastAPI 底层），提供路由、WebSocket、中间件等功能 |
| ASGI | Asynchronous Server Gateway Interface，异步服务器网关接口（类似 Flask 用的 WSGI） |
| 与 Flask 对比 | FastAPI 自带数据验证、自动文档、异步支持；Flask 更灵活简单，但需手动实现这些 |
| 与 Django 对比 | Django 是全栈框架（ORM+模板+Admin），FastAPI 专注于 API（不包含 ORM 和模板引擎） |
| 适用场景 | RESTful API、微服务、机器学习模型部署、实时 WebSocket 应用、BFF（Backend For Frontend） |

## 安装与环境准备

| 命令 | 说明 |
|---|---|
| `pip install fastapi` | 安装 FastAPI 核心 |
| `pip install "fastapi[standard]"` | 安装完整版（包含 uvicorn 服务器 + 标准依赖），推荐！ |
| `pip install uvicorn` | 安装 ASGI 服务器（必须，用于运行 FastAPI 应用） |
| `pip install "fastapi[all]"` | 安装所有可选依赖（不推荐，太多了） |
| 开发依赖 | `pip install python-multipart aiofiles jinja2 sqlalchemy`（文件上传、静态文件、模板、数据库） |
| 验证安装 | `python -c "import fastapi; print(fastapi.__version__)"` |

## 最小示例与运行

| 命令/操作 | 说明 | 示例 |
|---|---|---|
| `uvicorn main:app` | 运行 main.py 中的 app 对象 | `uvicorn main:app` |
| `--host` | 绑定 IP 地址（默认 127.0.0.1） | `uvicorn main:app --host 0.0.0.0` |
| `--port` | 绑定端口（默认 8000） | `uvicorn main:app --port 8080` |
| `--reload` | 代码修改后自动重启（仅开发用！） | `uvicorn main:app --reload` |
| `--workers` | 工作进程数（生产环境用，等于 CPU 核心数） | `uvicorn main:app --workers 4` |
| `--log-level` | 日志级别：info/debug/warning/error | `uvicorn main:app --log-level debug` |
| 访问 Swagger UI | 浏览器打开自动生成的交互式文档 | http://127.0.0.1:8000/docs |
| 访问 ReDoc | 浏览器打开另一种风格的文档 | http://127.0.0.1:8000/redoc |
| `fastapi dev` | （FastAPI 0.104+）新 CLI 开发命令 | `fastapi dev main.py` |
| `fastapi run` | （FastAPI 0.104+）新 CLI 生产命令 | `fastapi run main.py` |

## 请求方法与路由

| 装饰器 | HTTP 方法 | 说明 |
|---|---|---|
| `@app.get("/path")` | GET | 查询/获取资源 |
| `@app.post("/path")` | POST | 创建资源 |
| `@app.put("/path")` | PUT | 全量更新资源 |
| `@app.patch("/path")` | PATCH | 部分更新资源 |
| `@app.delete("/path")` | DELETE | 删除资源 |
| `@app.api_route` | 自定义方法 | 支持 methods 参数指定多个方法，如 `methods=["GET", "POST"]` |
| 路由标签与元数据 | `tags=["users"]` | 在 API 文档中给路由分组归类 |
| 路由分组 | `include_router` | 大型项目必备，将路由拆分到单独文件后挂载：`app.include_router(users.router, prefix="/api/v1")` |

## 路径参数与查询参数

| 参数类型 | 定义方式 | 说明 | 示例 |
|---|---|---|---|
| 路径参数 | 放在 URL 花括号中 | 必填，自动提取到函数参数 | `/items/{item_id}` |
| 查询参数 | 函数参数，不在花括号中 | 可选，跟在 ? 后面 | `/items/?skip=0&limit=10` |
| 类型声明 | `参数: type` | 自动类型转换与验证（如 str 转 int 失败报 422） | `item_id: int` |
| 默认值 | `参数 = value` | 不传则使用默认值，有默认值则变为可选参数 | `limit: int = 10` |
| 必填查询参数 | `参数: type` | 不设默认值，不传报错 | `needy: str` |
| 多值查询参数 | `参数: list[type] \| None = None` | 可接收同名参数多个值 | `?q=foo&q=bar` |
| 枚举参数 | 用 `enum.Enum` 类 | 限制参数只能为特定值 | `model_name: ModelName` |
| Path 验证 | `from fastapi import Path` | 专门用于路径参数的高级验证（ge/le/gt/lt） | `item_id: int = Path(ge=1, le=100)` |
| Query 验证 | `from fastapi import Query` | 专门用于查询参数的高级验证（长度/正则） | `q: str = Query(min_length=3, max_length=50)` |

## 请求体与 Pydantic 模型

| API/关键字 | 说明 | 示例 |
|---|---|---|
| BaseModel | 定义请求体结构的类，继承自 pydantic | `class Item(BaseModel): name: str; price: float` |
| Field | 定义模型字段的高级验证规则 | `price: float = Field(gt=0, description="价格大于0")` |
| 嵌套模型 | 模型字段可以是另一个模型或列表 | `class Order(BaseModel): items: list[Item]` |
| 多请求体 | 函数接收多个 Pydantic 对象 | `async def create(order: Order, user: User)` |
| Body | 显式标记请求体，可嵌入单个字段到 Body 中 | `priority: int = Body(default=1)` |
| model_dump() | 将 Pydantic 对象转为字典（FastAPI 0.98+，旧版用 dict()） | `return item.model_dump()` |
| model_dump_json() | 将 Pydantic 对象转为 JSON 字符串 | `return item.model_dump_json()` |
| model_validate() | 将字典转为 Pydantic 对象（旧版 parse_obj） | `Item.model_validate(data_dict)` |

## 响应处理

| 关键字/参数 | 说明 | 示例 |
|---|---|---|
| `response_model` | 指定返回数据的 Pydantic 模型，自动过滤未声明字段 | `@app.get("/", response_model=Item)` |
| `response_model_exclude_unset` | 仅返回客户端实际设置的字段（排除默认值） | `response_model_exclude_unset=True` |
| `status_code` | 指定 HTTP 响应状态码 | `status_code=status.HTTP_201_CREATED` |
| `status` 模块 | fastapi 内置的状态码常量集合 | `from fastapi import status; status.HTTP_200_OK` |
| `response_class` | 指定响应类型（如直接返回 HTML） | `from fastapi.responses import HTMLResponse` |
| `Response` | 直接返回原始 Response 对象 | `return Response(content=json_str, media_type="application/json")` |
| `JSONResponse` | 直接返回 JSON | `return JSONResponse(content={"msg": "ok"})` |
| `FileResponse` | 返回文件下载 | `return FileResponse(path="file.pdf", filename="report.pdf")` |

## 表单与文件上传

| 关键字/类 | 说明 | 示例 |
|---|---|---|
| `Form` | 声明表单数据字段（非 JSON，application/x-www-form-urlencoded） | `username: str = Form(...)` |
| `File` | 声明上传的文件对象 | `file: UploadFile = File(...)` |
| `UploadFile` | FastAPI 封装的文件对象，包含 filename/content_type | `file.filename` |
| `file.read()` | 异步读取文件全部内容到内存（注意大文件内存爆炸） | `contents = await file.read()` |
| 流式读取大文件 | 循环调用 read(chunk_size) 避免内存溢出 | 见下方示例 |
| 多文件上传 | 声明为列表类型 | `files: list[UploadFile] = File(...)` |
| python-multipart | 处理表单和文件必须安装的依赖 | `pip install python-multipart` |

流式读取大文件示例：

```python
while chunk := await file.read(1024 * 1024):
    f.write(chunk)
```

## Cookie、Header 与异常

| 类/函数 | 说明 | 示例 |
|---|---|---|
| `Cookie` | 获取请求中的 Cookie 值 | `token: str \| None = Cookie(default=None)` |
| `Header` | 获取请求头中的值 | `user_agent: str \| None = Header(default=None)` |
| Header 转换 | 自动将下划线转为横线并不区分大小写 | `strange_header: str \| None = Header(default=None, convert_underscores=False)` |
| `HTTPException` | 抛出 HTTP 错误，中断请求返回标准错误响应 | `raise HTTPException(status_code=404, detail="Item not found")` |
| `status_code` | 异常配合的状态码 | `HTTPException(status_code=403, detail="No permission")` |
| `detail` | 错误详情字段，会出现在自动生成的 JSON 错误响应中 | `detail="Not found"` |
| `headers` | 异常中可附带自定义响应头 | `raise HTTPException(status_code=400, headers={"X-Error": "Invalid"})` |
| 自定义异常处理器 | `@app.exception_handler(CustomException)` | 捕获特定异常并返回自定义响应格式 |

## 依赖注入

| 函数/类 | 说明 | 示例 |
|---|---|---|
| `Depends` | 声明依赖项的关键字，FastAPI 自动解析并注入 | `async def read_items(db: Session = Depends(get_db))` |
| 依赖是函数 | 普通函数作为依赖，执行业务逻辑或返回值 | `def get_query(q: str \| None = None): return q` |
| 依赖是类 | 类的实例作为依赖，FastAPI 自动实例化 | `class CommonQueryParams: def __init__(self, q: str): ...` |
| 子依赖 | 依赖项本身也可以有自己的依赖项（嵌套注入） | `Depends(get_current_active_user)` 依赖 `Depends(get_current_user)` |
| 全局依赖 | 整个应用或路由组都生效 | `app = FastAPI(dependencies=[Depends(verify_token)])` |
| yield 依赖 | 执行完主逻辑后执行清理工作（类似 try/finally） | `def get_db(): db=Session(); yield db; db.close()` |
| 依赖复用 | 同一请求中多次使用同一依赖，只会执行一次（缓存机制） | - |

## 中间件

| API/类 | 说明 | 示例 |
|---|---|---|
| `@app.middleware("http")` | 定义基于 HTTP 的中间件函数 | `async def add_custom_header(request, call_next): ...` |
| `call_next` | 将请求传递给下一个中间件或路由处理函数 | `response = await call_next(request)` |
| 请求前逻辑 | call_next 之前的代码 | 记录开始时间、验证特定 Header |
| 请求后逻辑 | call_next 之后的代码 | 记录处理时间、添加响应头 |
| CORS 中间件 | 处理跨域资源共享（前后端分离必备） | `from fastapi.middleware.cors import CORSMiddleware` |
| `allow_origins` | 允许的源列表（* 表示允许所有，生产慎用） | `allow_origins=["http://localhost:3000"]` |
| `allow_methods` | 允许的 HTTP 方法 | `allow_methods=["*"]` |
| `allow_headers` | 允许携带的请求头 | `allow_headers=["*"]` |
| `allow_credentials` | 允许携带 Cookie | `allow_credentials=True` |

## 数据库集成（以 SQLAlchemy 为例）

| 操作/概念 | 说明 | 示例 |
|---|---|---|
| 创建引擎 | `from sqlalchemy import create_engine` | `engine = create_engine("sqlite:///./test.db")` |
| 创建会话工厂 | `from sqlalchemy.orm import sessionmaker` | `SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)` |
| 定义模型类 | 继承 SQLAlchemy 的 Base | `class User(Base): __tablename__ = "users"; id = Column(Integer, primary_key=True)` |
| Pydantic 模型 | 用于 API 数据验证（与 ORM 模型分离） | `class UserCreate(BaseModel): name: str` |
| 依赖注入 DB | 用 yield 提供数据库会话，请求结束自动关闭 | `def get_db(): db = SessionLocal(); yield db; db.close()` |
| CRUD 操作 | 在依赖中获取 db 进行增删改查 | `db.add(user); db.commit(); db.refresh(user); db.query(User).filter().first()` |
| 分页查询 | 结合查询参数 skip 和 limit 实现 | `db.query(User).offset(skip).limit(limit).all()` |

## 安全认证

| 概念/类 | 说明 | 示例 |
|---|---|---|
| OAuth2PasswordBearer | 提取 Header 中的 Bearer Token | `token: str = Depends(OAuth2PasswordBearer(tokenUrl="login"))` |
| 密码哈希 | `from passlib.context import CryptContext` | `CryptContext(schemes=["bcrypt"], deprecated="auto")` |
| `hash()` | 对明文密码进行哈希 | `hashed_password = pwd_context.hash(plain_password)` |
| `verify()` | 验证明文密码与哈希是否匹配 | `pwd_context.verify(plain_password, hashed_password)` |
| JWT 令牌 | 生成和验证无状态 Token | `from jose import JWTError, jwt` |
| create_access_token | 生成 JWT 字符串 | `access_token = jwt.encode(data, SECRET_KEY, algorithm=ALGORITHM)` |
| decode_access_token | 解析 JWT 字符串获取 payload | `jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])` |
| get_current_user | 依赖注入：解析 Token 并查询数据库返回用户对象 | `async def get_current_user(token: str = Depends(oauth2_scheme))` |

## 后台任务与 WebSocket

| 功能 | 说明 | 示例 |
|---|---|---|
| BackgroundTasks | 在返回响应后执行后台任务（发邮件、写日志） | `def send_email(email: str): ...` |
| 添加任务 | background_tasks.add_task(send_email, user.email) | 在路径操作函数参数中接收：`tasks: BackgroundTasks` |
| WebSocket | 支持全双工通信 | `@app.websocket("/ws")` |
| websocket.accept() | 接受 WebSocket 连接 | `await websocket.accept()` |
| websocket.receive_json() | 接收客户端 JSON 数据 | `data = await websocket.receive_json()` |
| websocket.send_json() | 发送 JSON 数据给客户端 | `await websocket.send_json({"msg": "ok"})` |
| websocket.close() | 关闭连接 | `await websocket.close()` |
| WebSocket 断开处理 | 捕获 WebSocketDisconnect 异常 | `try: ... except WebSocketDisconnect: ...` |

## 项目目录结构规范

| 结构 | 说明 |
|---|---|
| main.py | 应用入口，创建 FastAPI 实例，挂载路由和中间件 |
| api/ | 按版本或模块划分的路由层（如 api/v1/endpoints/users.py） |
| crud.py | 数据库 CRUD 操作的具体 SQL 逻辑 |
| models.py | SQLAlchemy 等 ORM 的数据库表模型定义 |
| schemas.py | 所有 Pydantic 数据验证模型（请求体/响应体） |
| database.py | 数据库引擎、会话工厂配置 |
| dependencies.py | 公共的依赖注入函数（如 get_db、get_current_user） |
| core/ | 核心配置（如 config.py 读取环境变量，security.py 安全逻辑） |
| services/ | 复杂业务逻辑层（可选） |

## 部署与生产环境

| 方式/工具 | 说明 | 示例 |
|---|---|---|
| Uvicorn Workers | 利用多核 CPU，启动多个工作进程 | `uvicorn main:app --host 0.0.0.0 --port 80 --workers 4` |
| Gunicorn + Uvicorn | 用 Gunicorn 管理 Uvicorn 进程（生产推荐） | `gunicorn main:app -w 4 -k uvicorn.workers.UvicornWorker` |
| Docker | 容器化部署（最标准做法） | 见下方 Dockerfile |
| Nginx | 反向代理，处理 HTTPS、静态文件、负载均衡 | `proxy_pass http://127.0.0.1:8000;` |
| 关闭 Swagger 文档 | 生产环境关闭自动生成的文档，减少攻击面 | `app = FastAPI(docs_url=None, redoc_url=None)` |
| 环境变量 | 使用 pydantic-settings 管理配置，不硬编码密码 | `from pydantic_settings import BaseSettings` |

```dockerfile
FROM python:3.11
COPY . .
RUN pip install -r requirements.txt
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "80"]
```

# 图论与网络分析：NetworkX

NetworkX 是 Python 的图论与复杂网络分析库，用于创建、操作和研究复杂网络的结构、动态和功能。导入惯例是 `import networkx as nx`，下面所有示例都用 `nx` 前缀。

## 基本概念与核心定位

| 概念 | 说明 |
|---|---|
| NetworkX | Python 的图论与复杂网络分析库，用于创建、操作和研究复杂网络的结构、动态和功能 |
| 图 | 由节点和边组成的数学结构，用于表示实体之间的关系 |
| 节点 | 图中的基本元素，可以代表任何人、物体、概念等；NetworkX 中节点可以是任意可哈希对象（字符串/数字/元组等） |
| 边 | 连接两个节点的线，表示节点之间的关系，可以附加权重和属性 |
| 有向图 | 边有方向性的图（如 A→B 不等于 B→A），如网页链接、关注关系、单向道路 |
| 无向图 | 边没有方向性的图（如 A-B 等于 B-A），如好友关系、合作网络 |
| 多重图 | 允许两个节点之间存在多条边的图（如两个城市之间有多条航线） |
| 加权图 | 边上带有权重值的图（如道路距离、网络延迟、关系强度） |
| 二分图 | 节点可以分为两个互不相交的集合，边只在不同集合之间存在的图（如用户-商品） |
| 核心能力 | 图创建、图遍历算法、最短路径、网络流、社区发现、中心性分析、图可视化 |

## 安装与环境准备

| 命令 | 说明 |
|---|---|
| `pip install networkx` | 安装 NetworkX |
| `pip install networkx[all]` | 安装所有可选依赖 |
| `pip install matplotlib` | 安装绘图依赖（可视化必备） |
| `pip install numpy scipy` | 安装科学计算依赖 |
| `pip install pygraphviz` | 安装 Graphviz 后端（更美观的布局） |
| 验证安装 | `python -c "import networkx as nx; print(nx.__version__)"` |
| 导入惯例 | `import networkx as nx`（下文中使用 nx 前缀） |

## 四种图类型

| 类型 | 类名 | 说明 |
|---|---|---|
| 无向图 | `nx.Graph()` | 最基本的图类型，边无方向，不允许多重边 |
| 有向图 | `nx.DiGraph()` | 边有方向，A→B 和 B→A 是两条不同的边 |
| 无向多重图 | `nx.MultiGraph()` | 无向图但允许两个节点间存在多条边 |
| 有向多重图 | `nx.MultiDiGraph()` | 有向图且允许两个节点间存在多条相同方向的边 |
| 转换 | `G.to_undirected()` | 有向图转无向图 |
| 转换 | `G.to_directed()` | 无向图转有向图 |
| 检查类型 | `type(G)` / `isinstance(G, nx.Graph)` | 查看图的类型 |
| 判断有向 | `G.is_directed()` | 判断是否为有向图（返回 True/False） |
| 判断多重 | `G.is_multigraph()` | 判断是否为多重图 |

## 创建图

| 方式 | 代码 | 说明 |
|---|---|---|
| 空图 | `G = nx.Graph()` | 创建一个没有任何节点和边的空图 |
| 带节点创建 | `G = nx.Graph([(1,2),(2,3),(3,4)])` | 创建图的同时添加边（节点自动添加） |
| 从边列表创建 | `G = nx.from_edgelist([(1,2),(2,3)])` | 从 Python 列表创建 |
| 从字典创建 | `G = nx.from_dict_of_dicts({1:{2:{}}, 2:{3:{}}})` | 嵌套字典表示邻接关系 |
| 从 numpy 数组创建 | `G = nx.from_numpy_array(arr)` | 邻接矩阵（numpy 二维数组） |
| 从 scipy 稀疏矩阵创建 | `G = nx.from_scipy_sparse_matrix(sp_matrix)` | 大规模图（节省内存） |
| 从 pandas DataFrame 创建 | `G = nx.from_pandas_edgelist(df, source='src', target='dst')` | 从数据框的列创建 |
| 网络自带图 | `G = nx.karate_club_graph()` | 经典的空手道俱乐部图（34 节点） |
| 网络自带图 | `G = nx.davis_southern_women_graph()` | Davis 南方女性社交网络 |
| 网络自带图 | `G = nx.florentine_families_graph()` | 佛罗伦萨家族联姻图 |
| 完全图 | `G = nx.complete_graph(5)` | 5 个节点的完全图（每两个节点之间都有边） |
| 环形图 | `G = nx.cycle_graph(10)` | 10 个节点的环形图 |
| 路径图 | `G = nx.path_graph(5)` | 5 个节点的链状路径 |
| 星形图 | `G = nx.star_graph(5)` | 1 个中心节点连接 5 个外围节点 |
| 二分图 | `G = nx.complete_bipartite_graph(3, 5)` | 3+5 个节点的完全二分图 |
| 树 | `G = nx.balanced_tree(2, 3)` | 分支因子为 2，深度为 3 的平衡树 |
| 随机图 | `G = nx.erdos_renyi_graph(100, 0.05)` | ER 随机图（100 节点，边概率 5%） |
| 随机图 | `G = nx.barabasi_albert_graph(100, 3)` | BA 无标度网络（100 节点，每次加 3 条边，幂律分布） |
| 随机图 | `G = nx.watts_strogatz_graph(100, 4, 0.1)` | WS 小世界网络（100 节点，每个连 4 个邻居，重连概率 10%） |

## 节点操作

| 操作 | 代码 | 说明 |
|---|---|---|
| 添加单个节点 | `G.add_node(1)` | 添加节点 1 |
| 添加带属性的节点 | `G.add_node(1, name='Alice', age=25)` | 属性以关键字参数传入 |
| 添加多个节点 | `G.add_nodes_from([1, 2, 3])` | 从列表批量添加 |
| 添加带属性的多个节点 | `G.add_nodes_from([(1,{'color':'red'}),(2,{'color':'blue'})])` | 列表中每个元素为（节点，属性字典）元组 |
| 查看所有节点 | `G.nodes()` | 返回所有节点的视图（NodeView） |
| 转为列表 | `G.nodes` / `list(G.nodes)` | 转为列表 |
| 查看节点数量 | `G.number_of_nodes()` / `len(G)` | 返回节点数 |
| 查看节点属性 | `G.nodes[1]` | 返回节点 1 的属性字典 |
| 查看指定属性 | `G.nodes[1]['name']` | 返回节点 1 的 name 属性 |
| 修改节点属性 | `G.nodes[1]['age'] = 26` | 修改已有属性 |
| 批量更新属性 | `G.nodes[1].update({'age':26, 'city':'Beijing'})` | 批量更新属性 |
| 删除单个节点 | `G.remove_node(1)` | 删除节点 1 及其所有关联边 |
| 删除多个节点 | `G.remove_nodes_from([1, 2, 3])` | 批量删除 |
| 判断节点是否存在 | `1 in G` / `G.has_node(1)` | 返回 True/False |
| 遍历节点 | `for node in G.nodes(): print(node)` | 迭代所有节点 |
| 遍历节点及属性 | `for node, data in G.nodes(data=True): print(node, data)` | 同时获取节点属性 |

## 边操作

| 操作 | 代码 | 说明 |
|---|---|---|
| 添加单条边 | `G.add_edge(1, 2)` | 添加边 1-2（节点自动创建） |
| 添加带权重/属性的边 | `G.add_edge(1, 2, weight=3.5, relation='friend')` | 属性以关键字参数传入 |
| 添加多条边 | `G.add_edges_from([(1,2),(2,3),(3,4)])` | 从列表批量添加 |
| 添加带属性的批量边 | `G.add_edges_from([(1,2,{'weight':1.0}),(2,3,{'weight':2.0})])` | 三元组形式 |
| 查看所有边 | `G.edges()` | 返回所有边的视图 |
| 查看边的数量 | `G.number_of_edges()` / `G.size()` | 返回边数 |
| 查看边属性 | `G.edges[1, 2]` | 返回边 (1,2) 的属性字典 |
| 另一种访问方式 | `G[1][2]` / `G.adj[1][2]` | 另一种访问边属性的方式 |
| 修改边属性 | `G.edges[1, 2]['weight'] = 5.0` | 修改权重 |
| 另一种写法 | `G[1][2]['weight'] = 5.0` | 另一种写法 |
| 删除单条边 | `G.remove_edge(1, 2)` | 删除边 1-2 |
| 删除多条边 | `G.remove_edges_from([(1,2),(2,3)])` | 批量删除 |
| 判断边是否存在 | `G.has_edge(1, 2)` / `(1,2) in G.edges` | 返回 True/False |
| 有向图添加边 | `D.add_edge(1, 2)` | 添加有向边 1→2 |
| 遍历所有边 | `for u, v in G.edges(): print(u, v)` | 迭代所有边（两个端点） |
| 遍历边及属性 | `for u, v, data in G.edges(data=True): print(u, v, data)` | 同时获取边属性 |
| 查看邻居节点 | `G.neighbors(1)` / `list(G.adj[1])` | 返回节点 1 的所有邻居 |
| 查看度 | `G.degree(1)` | 返回节点 1 的度（连接的边数） |
| 查看所有节点的度 | `G.degree()` | 返回所有节点的度的视图 |

## 图属性与基本信息

| 属性/方法 | 说明 | 示例 |
|---|---|---|
| `G.number_of_nodes()` | 节点数 | `len(G)` |
| `G.number_of_edges()` | 边数 | `G.size()` |
| `G.nodes()` | 所有节点 | - |
| `G.edges()` | 所有边 | - |
| `G.degree()` | 所有节点的度 | - |
| `G.is_directed()` | 是否有向 | - |
| `G.is_multigraph()` | 是否多重图 | - |
| `G.is_connected()` | 是否连通图（仅无向图） | - |
| `G.is_strongly_connected()` | 是否强连通（仅无向图） | - |
| `nx.is_connected(G)` | 判断无向图是否连通（独立函数） | - |
| `nx.is_bipartite(G)` | 判断是否为二分图 | - |
| `nx.is_tree(G)` | 判断是否为树（连通且无环） | - |
| `G.subgraph([1,2,3])` | 返回由指定节点构成的子图 | - |
| `G.copy()` | 浅拷贝图 | - |
| `G.clear()` | 清空所有节点和边 | - |
| `nx.info(G)` | 图的摘要信息（旧版） | - |
| `G.graph['name'] = 'MyGraph'` | 图的元数据属性（不属于任何节点/边） | - |

## 图的可视化

| 方法 | 代码 | 说明 |
|---|---|---|
| 基础绘制 | `nx.draw(G)` | 最简单的绘制（弹窗显示） |
| 指定布局 | `pos = nx.spring_layout(G)` | 力导向布局（最常用） |
| 指定布局 | `pos = nx.circular_layout(G)` | 环形布局 |
| 指定布局 | `pos = nx.random_layout(G)` | 随机布局 |
| 指定布局 | `pos = nx.shell_layout(G)` | 同心圆布局 |
| 指定布局 | `pos = nx.kamada_kawai_layout(G)` | 路径长度导向布局 |
| 指定布局 | `pos = nx.spectral_layout(G)` | 基于拉普拉斯特征向量 |
| 带参数绘制 | `nx.draw(G, pos, with_labels=True, node_color='lightblue', node_size=500, edge_color='gray', font_size=12, width=1.5)` | 完整参数 |
| 带标签边绘制 | `nx.draw_networkx_edge_labels(G, pos, edge_labels=nx.get_edge_attributes(G, 'weight'))` | 显示边的权重标签 |
| 绘制子图 | `nx.draw(G.subgraph(nodes), pos)` | 只画部分节点 |
| 完整绘图模板 | `import matplotlib.pyplot as plt; plt.figure(figsize=(10,8)); pos = nx.spring_layout(G); nx.draw(G, pos, with_labels=True, node_size=300); plt.show()` | 标准流程 |
| 保存图片 | `plt.savefig('graph.png', dpi=300, bbox_inches='tight')` | 保存为高分辨率图片 |
| draw_networkx | `nx.draw_networkx(G, pos)` | 与 nx.draw 类似但返回更多控制对象 |
| draw_networkx_nodes | 单独绘制节点（可单独控制） | `nx.draw_networkx_nodes(G, pos, nodelist=[1,2,3], node_color='red')` |
| draw_networkx_edges | 单独绘制边 | `nx.draw_networkx_edges(G, pos, edgelist=[(1,2)], edge_color='blue')` |
| 节点颜色映射 | `nx.draw(G, pos, node_color=[G.degree(n) for n in G.nodes()], cmap=plt.cm.Blues)` | 按度大小着色 |
| 节点大小映射 | `nx.draw(G, pos, node_size=[G.degree(n)*100 for n in G.nodes()])` | 按度大小调整节点大小 |

## 图算法：中心性分析

| 算法 | 函数 | 说明 | 用途 |
|---|---|---|---|
| 度中心性 | `nx.degree_centrality(G)` | 节点的度/(n-1)，衡量直接连接数量 | 找出最活跃的节点 |
| 接近中心性 | `nx.closeness_centrality(G)` | 到其他所有节点的平均最短距离的倒数 | 找出最"中心"的节点 |
| 介数中心性 | `nx.betweenness_centrality(G)` | 节点在最短路径中出现的比例 | 找出"桥梁"/"中间人"节点 |
| 特征向量中心性 | `nx.eigenvector_centrality(G)` | 与高中心性节点相连越多，自己中心性越高 | 找出最有影响力的节点 |
| PageRank | `nx.pagerank(G)` | Google 的网页排名算法（有向图） | 网页排名、社交影响力 |
| Katz 中心性 | `nx.katz_centrality(G)` | 特征向量中心性的变体 | - |
| Hub 和 Authority | `nx.hits(G)` | HITS 算法，区分 hub（指向好的）和 authority（被好的指向） | 网页分析 |
| 获取 TopN | 找出中心性最高的 N 个节点 | `sorted(nx.degree_centrality(G).items(), key=lambda x: x[1], reverse=True)[:10]` | - |

## 图算法：最短路径

| 算法 | 函数 | 说明 | 时间复杂度 |
|---|---|---|---|
| Dijkstra | `nx.dijkstra_path(G, source, target)` | 单源最短路径（非负权重） | O((V+E)logV) |
| Dijkstra | `nx.dijkstra_path_length(G, source, target)` | 最短路径长度 | - |
| Dijkstra | `nx.single_source_dijkstra(G, source)` | 到所有节点的最短路径和长度 | - |
| Dijkstra | `nx.single_source_dijkstra_path(G, source)` | 到所有节点的最短路径 | - |
| BFS 无权最短路径 | `nx.shortest_path(G, source, target)` | 无权图的最短路径（BFS） | O(V+E) |
| BFS | `nx.shortest_path_length(G, source, target)` | 最短路径长度 | - |
| BFS | `nx.all_pairs_shortest_path(G)` | 所有节点对之间的最短路径 | - |
| Floyd-Warshall | `nx.floyd_warshall_numpy(G)` | 所有节点对最短路径（矩阵形式） | O(V^3) |
| Bellman-Ford | `nx.bellman_ford_path(G, source, target)` | 支持负权重边的最短路径 | O(VE) |
| A* | `nx.astar_path(G, source, target, heuristic)` | 带启发式函数的最短路径 | 通常比 Dijkstra 快 |

有向图注意：有向图的最短路径有方向性，G 中 u→v 存在，不一定 v→u 存在。

## 图算法：连通性与遍历

| 算法 | 函数 | 说明 |
|---|---|---|
| 连通分量 | `nx.connected_components(G)` | 无向图的所有连通分量（返回节点集合的迭代器） |
| 连通分量 | `nx.number_connected_components(G)` | 连通分量的数量 |
| 连通分量 | `nx.node_connected_component(G, node)` | 包含指定节点的连通分量 |
| 强连通分量 | `nx.strongly_connected_components(DG)` | 有向图的强连通分量 |
| 强连通分量 | `nx.strongly_connected_component_recursive(DG)` | 递归版本（大图可能栈溢出） |
| 弱连通分量 | `nx.weakly_connected_components(DG)` | 有向图忽略方向后的连通分量 |
| BFS 遍历 | `nx.bfs_tree(G, source)` | 从 source 开始的 BFS 生成树 |
| BFS | `nx.bfs_edges(G, source)` | BFS 遍历经过的边 |
| BFS | `nx.bfs_predecessors(G, source)` | BFS 前驱节点 |
| DFS 遍历 | `nx.dfs_tree(G, source)` | 从 source 开始的 DFS 生成树 |
| DFS | `nx.dfs_edges(G, source)` | DFS 遍历经过的边 |
| DFS | `nx.dfs_predecessors(G, source)` | DFS 前驱节点 |
| DFS | `nx.dfs_postorder_nodes(G, source)` | DFS 后序遍历节点 |
| 拓扑排序 | `nx.topological_sort(DG)` | 有向无环图（DAG）的拓扑排序 |
| 拓扑排序 | `nx.is_directed_acyclic_graph(DG)` | 判断是否为有向无环图 |
| 最短路径树 | `nx.dijkstra_tree(G, source)` | Dijkstra 算法生成的最短路径树 |

## 图算法：社区发现

| 算法 | 函数 | 说明 |
|---|---|---|
| Girvan-Newman | `nx.community.girvan_newman(G)` | 基于边介数的层次聚类社区发现 |
| 模块度最大化解 | `nx.community.greedy_modularity_communities(G)` | 贪心算法最大化模块度 |
| 标签传播 | `nx.community.label_propagation_communities(G)` | 基于标签传播（速度快） |
| Louvain | `nx.community.louvain_communities(G)` | Louvain 算法（速度快，适合大图，推荐！） |
| K-Clique | `nx.community.k_clique_communities(G, k)` | K-团渗透算法 |
| 模块度 | `nx.community.modularity(G, partition)` | 计算社区划分的模块度值（0~1，越大越好） |
| 获取最佳划分 | 使用 Louvain 算法得到社区划分 | `communities = nx.community.louvain_communities(G)` |
| 可视化社区 | 不同社区用不同颜色绘制 | - |

## 图算法：图的匹配与覆盖

| 算法 | 函数 | 说明 |
|---|---|---|
| 最大匹配 | `nx.max_weight_matching(G)` | 最大权匹配（每条边最多用一次） |
| 极大匹配 | `nx.maximal_matching(G)` | 极大匹配（贪心，不一定最大） |
| 最大团 | `nx.find_cliques(G)` | 找出所有极大团（完全子图） |
| 最大团 | `nx.graph_clique_number(G)` | 最大团的节点数 |
| 顶点覆盖 | `nx.min_weighted_vertex_cover(G)` | 最小权顶点覆盖 |
| 独立集 | `nx.maximal_independent_set(G)` | 极大独立集 |
| 图着色 | `nx.greedy_color(G)` | 贪心着色（返回节点到颜色的映射） |
| 图同构 | `nx.is_isomorphic(G1, G2)` | 判断两个图是否同构（结构相同） |
| 子图同构 | `nx.isomorphism.GraphMatcher(G1, G2).subgraph_is_monomorphic()` | 判断 G2 是否为 G1 的子图 |

## 图算法：网络流

| 算法 | 函数 | 说明 |
|---|---|---|
| 最大流 | `nx.maximum_flow(G, source, sink)` | 从 source 到 sink 的最大流（返回流值和流分布） |
| 最大流 | `nx.maximum_flow_value(G, source, sink)` | 只返回最大流的值 |
| 最小割 | `nx.minimum_cut(G, source, sink)` | 最小割（返回割值和两个集合） |
| 容量设置 | 边的属性 key 为 capacity | `G.add_edge('s', 'a', capacity=10)` |
| 最短增广路 | `nx.shortest_augmenting_path(G, source, sink)` | Edmonds-Karp 算法 |
| 流网络要求 | 必须是有向图，边必须有 capacity 属性 | - |

## 图算法：二分图

| 函数 | 说明 | 示例 |
|---|---|---|
| `nx.is_bipartite(G)` | 判断是否为二分图 | - |
| `nx.bipartite.sets(G)` | 返回二分图的两个节点集合 | - |
| `nx.bipartite.maximum_matching(G)` | 二分图最大匹配 | - |
| `nx.bipartite.minimum_weight_full_matching(G)` | 最小权完全匹配（匈牙利算法） | 分配问题 |
| `nx.bipartite.projected_graph(G, nodes)` | 二分图投影（保留指定集合内的连接） | - |
| 创建二分图 | 用 bipartite 属性标记集合 | `B = nx.Graph(); B.add_nodes_from([1,2,3], bipartite=0); B.add_nodes_from(['a','b'], bipartite=1); B.add_edges_from([(1,'a'),(2,'b')])` |

## 图算法：树与 DAG

| 函数 | 说明 | 示例 |
|---|---|---|
| `nx.is_tree(G)` | 判断是否为树 | - |
| `nx.is_forest(G)` | 判断是否为森林 | - |
| `nx.minimum_spanning_tree(G)` | 最小生成树（Kruskal/Prim） | - |
| `nx.minimum_spanning_edges(G)` | 最小生成树的边 | - |
| `nx.is_directed_acyclic_graph(DG)` | 判断是否为 DAG | - |
| `nx.topological_sort(DG)` | 拓扑排序（任务调度） | - |
| `nx.dag_longest_path(DG)` | DAG 中的最长路径（关键路径） | - |
| `nx.transitive_closure(DG)` | 传递闭包（可达性矩阵） | - |
| `nx.ancestors(DG, node)` | 某节点的所有祖先 | - |
| `nx.descendants(DG, node)` | 某节点的所有后代 | - |

## 图的 IO 读写

| 格式 | 写入函数 | 读取函数 | 说明 |
|---|---|---|---|
| GraphML | `nx.write_graphml(G, 'graph.graphml')` | `nx.read_graphml('graph.graphml')` | 最通用的图格式（推荐） |
| GEXF | `nx.write_gexf(G, 'graph.gexf')` | `nx.read_gexf('graph.gexf')` | Gephi 可视化软件的格式 |
| GML | `nx.write_gml(G, 'graph.gml')` | `nx.read_gml('graph.gml')` | 经典图格式 |
| Pajek | `nx.write_pajek(G, 'graph.net')` | `nx.read_pajek('graph.net')` | Pajek 软件格式 |
| Edge List | `nx.write_edgelist(G, 'edges.txt')` | `nx.read_edgelist('edges.txt')` | 简单的边列表（节点为字符串） |
| Adjlist | `nx.write_adjlist(G, 'adj.txt')` | `nx.read_adjlist('adj.txt')` | 邻接表格式 |
| JSON | `nx.node_link_data(G)` / `nx.node_link_graph(data)` | 转 JSON（适合前后端传输） | dict 格式 |
| Pandas | `nx.to_pandas_edgelist(G)` | `nx.from_pandas_edgelist(df)` | 与 DataFrame 互转 |
| NumPy | `nx.to_numpy_array(G)` | `nx.from_numpy_array(arr)` | 与邻接矩阵互转 |
| Scipy Sparse | `nx.to_scipy_sparse_array(G)` | `nx.from_scipy_sparse_array(sp)` | 大规模图（稀疏矩阵） |

## 图生成模型

| 模型 | 函数 | 说明 |
|---|---|---|
| 随机图（ER） | `nx.erdos_renyi_graph(n, p)` | n 个节点，每对节点之间以概率 p 连边 |
| BA 无标度网络 | `nx.barabasi_albert_graph(n, m)` | 优先连接机制，产生幂律度分布 |
| WS 小世界网络 | `nx.watts_strogatz_graph(n, k, p)` | 规则环网 + 随机重连，高聚类 + 短路径 |
| 规则图 | `nx.random_regular_graph(d, n)` | 每个节点度数都为 d 的随机正则图 |
| 随机 Lobster | `nx.random_lobster(n, p1, p2)` | 树 + 额外边的随机图 |
| 扩展 BA | `nx.powerlaw_cluster_graph(n, m, p)` | BA 模型 + 三角形形成 |
| 随机几何图 | `nx.random_geometric_graph(n, radius)` | 节点随机分布在平面上，距离小于 radius 则连边 |
| 复制模型 | `nx.duplication_divergence_graph(n, p)` | 基因网络复制模型 |
| 树 | `nx.random_tree(n)` | 随机生成树 |
| DAG | `nx.gnp_random_graph(n, p, directed=True)` | 随机有向图（可能有环） |

## 动态图（时序图/网络演化）

| 类 | 说明 |
|---|---|
| 用 `nx.DiGraph()` 模拟动态 | 通过逐步添加/删除节点和边模拟时序演化，每步代表一个时间点 |
| `nx.SnapshotGraph` | 在 networkx-dynamic 扩展包中（需额外安装）：`pip install networkx-dynamic` |
| 模拟方法 | 在循环中按时间步添加/移除边：`G.add_edge(u, v, time=t); G.remove_edge(old_u, old_v)` |
| 时序属性 | 边上添加时间戳属性：`G.add_edge(1, 2, timestamp='2024-01-01')` |
| 时序分析 | 分析不同时间快照的图指标变化 |

## 实战：网络分析完整流程模板

```python
import networkx as nx
import matplotlib.pyplot as plt
import numpy as np

# ===== 1. 创建/加载图 =====
G = nx.karate_club_graph()               # 使用自带数据集
# G = nx.read_graphml('my_graph.graphml') # 或从文件加载

# ===== 2. 基本信息统计 =====
print(f"节点数: {G.number_of_nodes()}")
print(f"边数: {G.number_of_edges()}")
print(f"平均度: {sum(dict(G.degree()).values()) / G.number_of_nodes():.2f}")
print(f"密度: {nx.density(G):.4f}")
print(f"连通分量数: {nx.number_connected_components(G)}")
print(f"平均聚类系数: {nx.average_clustering(G):.4f}")

# ===== 3. 中心性分析 =====
degree_cen = nx.degree_centrality(G)
betweenness_cen = nx.betweenness_centrality(G)
pagerank = nx.pagerank(G)

# Top5 关键节点
print("度中心性Top5:", sorted(degree_cen.items(), key=lambda x: x[1], reverse=True)[:5])
print("PageRank Top5:", sorted(pagerank.items(), key=lambda x: x[1], reverse=True)[:5])

# ===== 4. 社区发现 =====
communities = nx.community.louvain_communities(G)
print(f"社区数量: {len(communities)}")
modularity = nx.community.modularity(G, communities)
print(f"模块度: {modularity:.4f}")

# ===== 5. 路径分析 =====
shortest_path = nx.shortest_path(G, source=0, target=33)
avg_path_length = nx.average_shortest_path_length(G)
print(f"节点0到33的最短路径: {shortest_path}")
print(f"平均最短路径长度: {avg_path_length:.4f}")

# ===== 6. 可视化 =====
fig, axes = plt.subplots(1, 3, figsize=(20, 6))

# 图1: 按度大小着色
pos = nx.spring_layout(G, seed=42)
node_colors = [degree_cen[n] for n in G.nodes()]
nx.draw(G, pos, ax=axes[0], with_labels=True, node_color=node_colors,
        cmap=plt.cm.Blues, node_size=300, font_size=8)
axes[0].set_title("Degree Centrality")

# 图2: 按社区着色
color_map = []
for node in G.nodes():
    for i, community in enumerate(communities):
        if node in community:
            color_map.append(i)
            break
nx.draw(G, pos, ax=axes[1], with_labels=True, node_color=color_map,
        cmap=plt.cm.Set1, node_size=300, font_size=8)
axes[1].set_title(f"Communities (Modularity={modularity:.2f})")

# 图3: PageRank大小
node_sizes = [pagerank[n] * 5000 for n in G.nodes()]
nx.draw(G, pos, ax=axes[2], with_labels=True, node_size=node_sizes,
        node_color='lightcoral', font_size=8)
axes[2].set_title("PageRank")

plt.tight_layout()
plt.savefig('network_analysis.png', dpi=300, bbox_inches='tight')
plt.show()

# ===== 7. 保存结果 =====
nx.write_graphml(G, 'analyzed_graph.graphml')
```

# 位级操作：bitarray

bitarray 是 Python 的第三方库，提供高效的位数组数据结构，以 bit 为单位存储和操作数据。相比用 `list[int]` 存 0/1，内存节省约 8 倍（1bit vs 8bit），位操作更快（C 语言实现）。

## 基本概念与核心定位

| 概念 | 说明 |
|---|---|
| bitarray | Python 第三方库，提供高效的位数组数据结构，以 bit 为单位存储和操作数据 |
| 核心优势 | 相比 list[int] 存储 0/1：内存节省约 8 倍（1bit vs 8bit），位操作更快（C 语言实现） |
| 安装 | `pip install bitarray` |
| 导入 | `from bitarray import bitarray` |
| bitarray 对象 | 每个元素占 1bit，值为 0 或 1，支持索引/切片/拼接/位运算 |
| 与 list[int] 对比 | list[int] 存储 8 个 0/1 占 64 字节；bitarray 存 8 个 bit 只占 1 字节，大数据量下内存优势巨大 |
| 与 bytes 对比 | bytes 按字节操作，bitarray 可精确到单个 bit，二进制协议解析必备 |
| 适用场景 | 二进制协议解析、位图/布隆过滤器、压缩编码、DNA 序列分析、大规模布尔集合运算 |
| 与 Python 内置替代 | bitarray vs bytearray（字节级）vs int（位运算但不直观）vs list[int]（最慢最占内存），bitarray 在 bit 级别操作上是最优解 |

## 安装与环境

| 命令 | 说明 |
|---|---|
| `pip install bitarray` | 标准安装（含 C 扩展加速） |
| `pip install bitarray --upgrade` | 升级到最新版 |
| `pip install bitarray2` | 替代方案（另一个独立库，API 不同，谨慎选择） |
| 验证安装 | `python -c "from bitarray import bitarray; print('OK')"`，不报错即成功 |
| C 扩展 | bitarray 自动编译 C 扩展，无需额外操作；如果编译失败会回退到纯 Python（慢） |
| `from bitarray.util import *` | 导入实用工具函数（整数转 bitarray、十六进制转换等） |

## 创建 bitarray

| 方法 | 代码 | 说明 | 输出示例 |
|---|---|---|---|
| 空位数组 | `ba = bitarray()` | 创建空的 bitarray | `bitarray('')` |
| 指定长度初始化 | `ba = bitarray(8)` | 创建指定长度全 0 的 bitarray | `bitarray('00000000')` |
| 从二进制字符串创建 | `ba = bitarray('10110')` | 0 和 1 组成的字符串 | `bitarray('10110')` |
| 从字节创建 | `ba = bitarray(b'\x05')` | 0x05 = 0b00000101 | `bitarray('00000101')` |
| 从字节创建（指定长度） | `ba = bitarray(b'\x05', length=4)` | 只取前 4 位（bitarray 的 MSB 优先） | `bitarray('0000')` |
| 从整数创建 | 从 bitarray.util 导入：`ba = int2ba(42, length=8)` | 42 = 0b00101010 | `bitarray('00101010')` |
| 从整数创建（不指定长度） | `ba = int2ba(42)` | 自动计算最小长度（去掉前导 0） | `bitarray('101010')` |
| 从十六进制字符串 | `ba = bitarray().frombytes(bytes.fromhex('05'))` | 0x05 = 0b00000101 | `bitarray('00000101')` |
| 从另一个 ba 拷贝 | `ba2 = ba.copy()` | 深拷贝 | - |

## 基础信息查看

| 属性/方法 | 说明 | 示例 |
|---|---|---|
| `len(ba)` | 位数组长度（位数） | `len(bitarray('10110'))` -> 5 |
| `ba.length()` | 同 len(ba)，位数 | `ba.length()` -> 5 |
| `ba.nbytes()` | 占用的字节数（向上取整） | `bitarray('10110').nbytes()` -> 1（5 位 = 1 字节） |
| `ba.buffer_info()` | 返回（内存地址，字节数） | 底层调试用 |
| `ba.isempty()` | 是否为空 | - |
| `type(ba)` | 类型 | `<class 'bitarray.bitarray'>` |
| `ba == bitarray('10110')` | 判等（逐位比较） | True/False |
| `repr(ba)` / `str(ba)` | 字符串表示（二进制形式） | `"bitarray('10110')"` |

## 索引、切片与赋值

| 操作 | 说明 | 示例 |
|---|---|---|
| `ba[i]` | 读取第 i 位（返回 int：0 或 1） | `ba[0]`, `ba[-1]` |
| `ba[i] = v` | 设置第 i 位（0 或 1） | `ba[0] = 1` |
| `ba[start:stop]` | 切片（返回新 bitarray） | `ba[2:5]` |
| `ba[start:stop:step]` | 带步长切片 | `ba[::2]`（每隔一位） |
| `ba[start:stop] = new_ba` | 切片赋值（替换指定范围） | `ba[0:3] = bitarray('111')` |
| `ba[start:stop:step] = iter` | 带步长赋值（可迭代对象） | `ba[::2] = bitarray('11')` |
| `del ba[i]` | 删除第 i 位 | `del ba[0]` |
| `del ba[start:stop]` | 删除切片范围 | `del ba[2:5]` |
| `ba.count(1)` | 统计 1 的个数 | `ba.count(1)` -> 3 |
| `ba.count(0)` | 统计 0 的个数 | `ba.count(0)` -> 2 |
| `ba.index(value)` | 查找第一个值为 value 的索引 | `ba.index(1)` -> 0（第一个 1 的位置） |

## 修改操作

| 方法 | 说明 | 示例 |
|---|---|---|
| `ba.append(bit)` | 末尾追加一位（0 或 1） | `ba.append(1)` |
| `ba.extend(iterable)` | 末尾追加多位（可迭代对象） | `ba.extend([1,0,1])` |
| `ba.insert(i, bit)` | 在位置 i 插入一位 | `ba.insert(0, 1)` |
| `ba.pop()` | 弹出最后一位并返回（默认） | `ba.pop()` -> 1 |
| `ba.pop(i)` | 弹出指定位置的一位 | `ba.pop(0)` |
| `ba.remove(bit)` | 删除第一个值为 bit 的位 | `ba.remove(0)` |
| `ba.reverse()` | 原地翻转所有位 | `ba.reverse()` |
| `ba.clear()` | 清空所有位 | `ba.clear()` |
| `ba.fill(bit)` | 用 bit 填充所有位 | `ba.fill(0)` |
| `ba.setall(bit)` | 同 fill，设置所有位为指定值 | `ba.setall(1)` |
| `ba.bytereverse()` | 按字节反转位序（每个字节内部 8 位翻转） | `ba.bytereverse()` |

## 拼接与重复

| 操作 | 说明 | 示例 |
|---|---|---|
| `ba1 + ba2` | 拼接两个 bitarray（返回新对象） | `bitarray('101') + bitarray('010')` -> `bitarray('101010')` |
| `ba1 += ba2` | 就地拼接 | `ba1 += ba2` |
| `ba1 * n` | 重复 n 次（返回新对象） | `bitarray('101') * 3` -> `bitarray('101101101')` |
| `ba1 *= n` | 就地重复 | `ba1 *= 3` |
| `''.join(map(str, ba))` | 转成二进制字符串 | `'10110010'` |

## 位运算（bitarray 自带）

| 方法 | 等价运算 | 说明 |
|---|---|---|
| `ba1 & ba2` | 位与（要求同长度或 ba2 可广播） | 逐位 AND，返回新 bitarray |
| `ba1 \|= ba2` | 就地位或 | 逐位 OR |
| `ba1 ^= ba2` | 就地位异或 | 逐位 XOR |
| `ba1 &= ba2` | 就地位与 | 逐位 AND |
| `~ba` | 按位取反（返回新 bitarray） | 0 变 1，1 变 0 |
| `ba << n` | 左移 n 位（返回新 bitarray，右边补 0） | `bitarray('10110') << 2` -> `bitarray('1011000')` |
| `ba >> n` | 右移 n 位（返回新 bitarray，高位补 0） | `bitarray('10110') >> 2` -> `bitarray('101')` |
| `ba <<= n` | 就地左移 | `ba <<= 3` |
| `ba >>= n` | 就地右移 | `ba >>= 3` |
| `ba1 == ba2` | 逐位比较是否相等 | - |
| `ba.any()` | 是否有任何一位为 1 | 等价于 `any(ba)` |
| `ba.all()` | 是否所有位都为 1 | 等价于 `all(ba)` |

## 搜索与查找

| 方法 | 说明 | 示例 |
|---|---|---|
| `ba.count(val)` | 统计 val（0 或 1）出现的次数 | `ba.count(1)` -> 5 |
| `ba.index(val)` | 第一个 val 的索引 | `ba.index(1)` -> 2 |
| `ba.index(val, start)` | 从 start 开始查找 | `ba.index(1, 5)` |
| `ba.find(sub_ba)` | 查找子位数组第一次出现的位置 | 返回起始索引，未找到返回 -1 |
| `ba.rfind(sub_ba)` | 从右向左查找子位数组 | `ba.rfind(bitarray('10'))` |
| `ba.startswith(sub_ba)` | 是否以子位数组开头 | `ba.startswith(bitarray('101'))` |
| `ba.endswith(sub_ba)` | 是否以子位数组结尾 | `ba.endswith(bitarray('010'))` |
| `ba.itersearch(sub_ba)` | 迭代搜索所有匹配位置（返回索引生成器） | `for pos in ba.itersearch(pattern): print(pos)` |

## 集合运算（位数组作为集合）

当元素范围确定且不大时（如 0~999999），可用 bitarray 做超高效的位集合。

| 方法 | 说明 | 示例 |
|---|---|---|
| 集合交集 | `ba1 & ba2` | 两个集合都包含的元素（AND） |
| 集合并集 | `ba1 \| ba2` | 任一集合包含的元素（OR） |
| 集合差集 | `ba1 & ~ba2` | 在 ba1 中但不在 ba2 中 |
| 集合对称差 | `ba1 ^ ba2` | 只在其中一个集合中的元素 |
| `ba.count()` | 集合大小（元素个数） | `ba.count(1)` |
| `ba[i] = 1` | 添加元素 i 到集合 | - |
| `ba[i] = 0` | 从集合中移除元素 i | - |
| `ba[i]` | 判断元素 i 是否在集合中（0 或 1） | - |

性能对比：set.add() vs `bitarray[i]=1`，bitarray 快 10-100 倍（set 需哈希计算），百万级元素集合操作 bitarray 碾压 set。

## 序列方法

| 方法 | 说明 | 示例 |
|---|---|---|
| `ba.tolist()` | 转为 Python list[int] | `[1, 0, 1, 1, 0]` |
| `ba.copy()` | 深拷贝 | - |
| `ba.encode(encoding)` | 按指定编码将位数组转为字符串 | `ba.encode('utf-8')`（位数组长度必须是 8 的倍数） |
| `ba.decode(encoding)` | 将字节解码为字符串 | - |
| `ba.tobytes()` | 转为 bytes（不足 8 位高位补 0） | `ba.tobytes()` |
| `ba.frombytes(b)` | 从 bytes 填充位数组 | `ba.frombytes(b'\x05')` |
| `ba.unpack(zip=)` | 解包为字符串（'01' 字符串或二进制字节） | `ba.unpack(zero=b'0', one=b'1')` |

## 序列化（I/O 持久化）

| 方法 | 说明 | 示例 |
|---|---|---|
| `ba.tofile(f)` | 将位数组写入二进制文件 | `ba.tofile(open('data.bin', 'wb'))` |
| `bitarray.fromfile(f, n)` | 从二进制文件读取 n 个字节构造 bitarray | `ba = bitarray.fromfile(open('data.bin', 'rb'), 10)` |
| `pickle.dump(ba, f)` | 用 pickle 序列化（通用） | `pickle.dump(ba, open('data.pkl', 'wb'))` |
| `pickle.load(f)` | 用 pickle 反序列化 | `ba = pickle.load(open('data.pkl', 'rb'))` |
| `ba.to01()` | 转为 '0' 和 '1' 组成的字符串 | `'10110010'` |
| `bitarray(s).from01(s)` | 从 '01' 字符串构造 | - |
| `ba.tosep()` | 按分隔符连接（用于可视化） | `'1-0-1-1-0-1-0'` |

## bitarray.util 工具函数

| 函数 | 说明 | 示例 |
|---|---|---|
| `int2ba(n, length=N)` | 整数转 bitarray | `int2ba(42, length=8)` -> `bitarray('00101010')` |
| `ba2int(ba)` | bitarray 转整数 | `ba2int(bitarray('00101010'))` -> 42 |
| `hex2ba(hex_str)` | 十六进制字符串转 bitarray | `hex2ba('ff')` -> `bitarray('11111111')` |
| `ba2hex(ba)` | bitarray 转十六进制字符串 | `ba2hex(bitarray('11111111'))` -> 'FF' |
| `ba2base(n, ba)` | bitarray 转 base-n 字符串（2/4/8/16/32/64） | `ba2base(64, ba)` |
| `base2ba(n, s)` | base-n 字符串转 bitarray | `base2ba(64, s)` |
| `zeros(n)` | 创建长度为 n 的全 0 bitarray | `zeros(8)` -> `bitarray('00000000')` |
| `ones(n)` | 创建长度为 n 的全 1 bitarray | `ones(8)` -> `bitarray('11111111')` |
| `urandom(n, ...)` | 从 OS 安全随机源生成 n 位随机 bitarray | `urandom(128)`（密码学安全） |
| `make_endian(ba, fmt)` | 转换大小端（字节序） | `make_endian(ba, 'big')` |
| `count_n(a, n, ...)` | 高效统计子模式出现次数（指定对齐方式） | `count_n(ba, bitarray('101'))` |
| `parity(ba)` | 计算奇偶校验位（所有位的 XOR） | `parity(ba)` -> 0 或 1 |

## 进阶：冻结位数组与回调

| 特性 | 说明 | 示例 |
|---|---|---|
| frozenbitarray | 不可变的 bitarray（类似 tuple vs list） | `from bitarray import frozenbitarray` |
| frozenbitarray(ba) | 从 bitarray 创建冻结版本 | `fb = frozenbitarray(bitarray('10110'))` |
| 用途 | 可作为字典的 key（可哈希） | `d = {frozenbitarray('101'): 'A', frozenbitarray('010'): 'B'}` |
| `ba.check()` | 检查内部一致性（调试用） | `ba.check()` -> True |
| `ba.cycles()` | 返回循环移位的最小步数 | - |
| 回调检查 | `ba.setcheck(callback_func)` | 设置回调函数，每次修改时调用，用于调试或自动验证 |

## bitarray vs Python 内置方案对比

| 维度 | bitarray | list[int(0,1)] | bytearray | int（位运算） | set |
|---|---|---|---|---|---|
| 存储单位 | 1 bit | 8 bits | 8 bits | 任意 | 多变 |
| 内存效率 | 极高 | 极低（8 倍差距） | 中等 | 中等 | 低（哈希表开销） |
| 位操作速度 | 极快（C 实现） | 慢（Python 循环） | 有限 | 快但不直观 | 无 |
| 修改能力 | 可增删改查 | 可增删改查 | 字节级修改 | 不可变（不可修改某一位） | 可增删 |
| 切片 | 支持 | 支持 | 支持 | 不支持 | 不支持 |
| 集合运算 | 极快（位运算） | 极慢（遍历） | 不支持 | 快但 API 差 | 支持（但慢） |
| 序列化 | tofile/tobytes | 手动处理 | 内置 | 内置 | 内置 |
| 密码学安全随机 | urandom() | 需额外处理 | os.urandom + 转换 | - | - |
| 可哈希（作 dict key） | 需 frozenbitarray | 否 | 否 | 是（int 可哈希） | 否 |
| 推荐场景 | 大规模 bit 操作、位图、集合运算 | 少量 bit，需要灵活性 | 字节级操作 | 少量 bit 运算 | 传统集合（非 bit） |

## 常见问题与排错（FAQ）

| 问题 | 原因与解决 |
|---|---|
| ModuleNotFoundError: No module named 'bitarray' | 未安装：`pip install bitarray` |
| TypeError: 'int' object is not iterable | 创建时传了整数而非字符串或可迭代对象。用 `bitarray('101')` 而不是 `bitarray(101)` |
| ValueError: mismatch in length | 位运算时两个 bitarray 长度不同。& 运算要求同长度，先 pad 或 resize |
| tobytes() 长度不对 | 位数不是 8 的倍数时会自动高位补 0 再转 bytes。注意补 0 对数据的影响 |
| 索引返回值类型 | `ba[i]` 返回 int（0 或 1），不是 bitarray；赋值时 `ba[i] = 1`（int） |
| 大端序 vs 小端序 | bitarray 默认 MSB 在前（大端），与协议要求的位序可能不同。用 make_endian() 或手动处理 |
| 性能问题 | 循环中逐位操作 `ba[i]` 很慢（Python 层面）。尽量用批量操作（切片赋值、位运算、内置方法） |
| 内存泄漏 | 超大 bitarray 不释放：`ba = None; import gc; gc.collect()` |
| pickle 兼容性 | pickle 序列化的 bitarray 可能跨版本不兼容。跨版本传输推荐用 tobytes() |
| 与 numpy 互转 | 不能直接互转。先 tobytes() 再 np.frombuffer()，或用 np.packbits()/unpackbits() |
| 与 struct 配合 | 先 tobytes() 再用 struct 解包：`data = struct.unpack('>I', ba.tobytes()[:4])` |

# 网络爬虫：requests 与 BeautifulSoup

写爬虫最常用的一对组合是 **requests** 负责发 HTTP 请求拿网页，**BeautifulSoup** 负责解析 HTML、提取需要的数据。

## requests：发送 HTTP 请求

requests 用于发送 HTTP 请求获取网页内容，是爬虫的第一步。

## BeautifulSoup：解析 HTML

BeautifulSoup 用于解析 HTML，提取我们需要的数据，导入方式为：

```python
from bs4 import BeautifulSoup
```

典型配合：先用 `requests` 拿到网页源码，再交给 `BeautifulSoup` 解析，通过标签/选择器定位并抽取目标内容。
