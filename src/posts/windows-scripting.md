---
title: Windows 脚本与环境
date: 2026-04-10
category: 学习记录
tags: ["windows", "bat", "powershell", "winget", "chocolatey"]
excerpt: 批处理 bat、PowerShell、winget/Chocolatey 包管理与激活记录
---
# 批处理 bat

## 基本概念与核心定位

批处理（BAT）是 DOS/Windows 下的脚本文件，里面包含一系列 CMD 命令，按顺序自动执行，扩展名为 `.bat` 或 `.cmd`。CMD 是 Windows 命令提示符，是解释执行 BAT 文件的传统 Shell（对应 Linux 的 Bash）；PowerShell 是微软新一代 Shell，功能远超 CMD（支持面向对象、管道传对象），但 BAT 仍广泛用于简单自动化。

BAT 基于字符串处理、语法古老；PowerShell（PS1）基于 .NET 对象、语法现代。BAT 胜在零门槛、全 Windows 兼容、双击即跑。BAT 不需要 Shebang，Windows 通过扩展名 `.bat`/`.cmd` 关联到 cmd.exe 执行。

| 项目 | 说明 |
|---|---|
| 编码注意 | 强烈建议使用 ANSI(GBK) 编码保存，否则 echo 中文会乱码（在编辑器中另存为 ANSI） |
| 换行符 | 必须是 CRLF(\r\n)，如果是 LF(\n) 会导致执行异常 |
| 核心优势 | 无需安装任何环境、双击直接运行、系统运维和定时任务必备 |
| 常见用途 | 环境变量配置、文件批量重命名、自动打包备份、定时关机、启动多个程序 |

## 运行与基础设置

| 命令 | 说明 | 示例 |
|---|---|---|
| `@echo off` | 关闭命令回显（BAT 脚本第一行必须是这个，否则会打印每行代码本身） | `@echo off` |
| `echo` | 打印输出到屏幕 | `echo Hello World` |
| `echo.` | 打印一个空行 | `echo.` |
| `pause` | 暂停执行，提示“请按任意键继续...”（双击运行防闪退必备） | `pause` |
| `title` | 设置 CMD 窗口标题 | `title 我的自动化脚本` |
| `color` | 设置 CMD 窗口背景和字体颜色 | `color 0A`（黑底绿字） |
| `mode con` | 设置 CMD 窗口大小 | `mode con cols=80 lines=25` |
| `cls` | 清屏 | `cls` |
| `exit` | 退出当前 CMD 脚本 | `exit /b 0`（退出脚本并返回状态码 0） |
| `rem` | 注释行（不会被解释执行） | `rem 这是注释` |
| `::` | 另一种注释方式（推荐，更规范） | `:: 这是注释` |
| `%0` | 当前脚本自身的完整路径 | `echo %0` |

## 变量与特殊变量

| 操作 | 语法 | 说明 |
|---|---|---|
| 定义变量 | `set 变量名=值` | 等号两边不能有空格！例：`set name=Tom` |
| 使用变量 | `%变量名%` | `echo %name%` |
| 删除变量 | `set 变量名=` | `set name=`（赋空值即删除） |
| 局部变量 | `setlocal` | 启用局部变量环境（脚本结束时自动还原环境变量，防污染系统） |
| 结束局部 | `endlocal` | 与 setlocal 配对使用 |
| 延迟扩展 | `setlocal enabledelayedexpansion` | 解决 for 循环中变量不实时更新的问题，变量用 `!var!` 代替 `%var%` |
| 环境变量 | `set PATH=C:\new` | 永久或临时修改系统环境变量 |
| 查看所有环境变量 | `set` | 直接输入 set，列出当前所有环境变量 |

内置特殊变量：

| 变量 | 说明 | 示例 |
|---|---|---|
| `%CD%` | 当前所在目录 | `echo %CD%` |
| `%DATE%` | 当前日期（格式取决于系统） | `echo %DATE%` |
| `%TIME%` | 当前时间 | `echo %TIME%` |
| `%RANDOM%` | 生成 0 到 32767 之间的随机数 | `echo %RANDOM%` |
| `%ERRORLEVEL%` | 上一个命令的退出状态码（0 成功，非 0 失败） | `if %ERRORLEVEL% neq 0 echo failed` |
| `%1` ~ `%9` | 传递给脚本的第 1 到第 9 个参数 | `mybat.bat param1 param2`（脚本内用 `%1` 和 `%2` 接收） |
| `%*` | 传递给脚本的所有参数 | `echo %*` |
| `%~1` | 去除参数 1 的外围引号（防路径空格报错） | `cd /d "%~1"` |
| `%~dp0` | 当前脚本所在的绝对目录路径（极其常用！用于定位相对路径） | `cd /d "%~dp0"` |
| `%~n0` | 当前脚本的文件名（无扩展名） | `echo %~n0` |
| `%~x0` | 当前脚本的扩展名 | `echo %~x0` |

## 字符串处理

| 操作 | 语法 | 说明 |
|---|---|---|
| 截取字符串 | `%变量名:~起始索引,长度%` | 索引从 0 开始；例：`set str=abcdef` 后 `echo %str:~1,3%` 输出 `bcd` |
| 截取到末尾 | `%变量名:~起始索引%` | 例：`echo %str:~3%` 输出 `def` |
| 替换字符串 | `%变量名:旧=新%` | 例：`set str=abcabc` 后 `echo %str:a=x%` 输出 `xbcxbc` |
| 字符串拼接 | 直接写在一起 | `set full=%str1%%str2%` 无缝拼接 |
| 拼接加字符 | 拼接时插入字符 | `set full=%str1%-%str2%`，`echo %full%` 输出 `a-b` |
| 获取字符串长度 | 已弃用的复杂方法 | 建议用 PowerShell：`$str.Length` |
| 去掉前后空格 | 无原生命令 | 用 for 循环：`for /f "tokens=* delims= " %%a in ("%str%") do set str=%%a` |

## 条件判断（IF）

| 语法 | 说明 | 示例 |
|---|---|---|
| `IF [NOT] ERRORLEVEL number` | 如果上一个命令的退出码 >= number（NOT 取反） | `if %ERRORLEVEL% equ 0 echo success` |
| `IF [NOT] string1==string2` | 字符串相等比较（`if /i` 忽略大小写） | `if /i "%input%"=="Y" goto yes` |
| `IF [NOT] EXIST filename` | 判断文件或目录是否存在 | `if exist "C:\test.txt" (echo 存在) else (echo 不存在)` |
| `IF [NOT] DEFINED variable` | 判断变量是否已定义 | `if defined var (echo 已定义) else (echo 未定义)` |
| `()` 代码块 | 用括号将多行代码包起来作为一个整体执行 | `if exist file.txt (echo found) else (echo not found)` |

比较运算符：`EQU` / `NEQ`（等于/不等于，数字比较）、`LSS` / `LEQ`（小于/小于等于）、`GTR` / `GEQ`（大于/大于等于）、`==`（等于，字符串比较区分大小写）、`/i`（忽略大小写开关）。

## 循环（FOR）

| 语法 | 说明 | 示例 |
|---|---|---|
| `FOR %%variable IN (set) DO command` | 遍历括号内的集合（.bat 中用 `%%v`，CMD 命令行中用 `%v`） | `for %%i in (1 2 3) do echo %%i` |
| `FOR /L %%i IN (start,step,end)` | 序列循环 | `for /l %%i in (1,1,10) do echo %%i`（1 到 10） |
| `FOR /D %%i IN (pattern)` | 遍历目录（不进入子目录） | `for /d %%i in (C:\*) do echo %%i` |
| `FOR /R [[drive:]path] %%i IN (set)` | 递归遍历目录树中的文件 | `for /r "C:\temp" %%i in (*.txt) do echo %%i` |
| `FOR /F "options" %%i IN (file-set)` | 解析文件内容（按行读取，极其强大） | `for /f "delims=" %%i in (data.txt) do echo %%i` |
| `FOR /F "tokens=" %%i IN ('command')` | 解析命令的输出结果（单引号包裹命令） | `for /f "tokens=2 delims=:" %%i in ('ipconfig ^\| find "IPv4"') do set ip=%%i` |
| `delims=` | 指定分隔符（默认空格和制表符） | `for /f "tokens=1,3 delims=," %%a in (a.csv) do echo %%a %%b`（%%b 是第二个 token） |
| `tokens=n,m` | 指定提取第几列（n,m 为列号，%%a 起始自动顺延为 %%b） | `for /f "tokens=1,2" %%a in (data.txt) do echo %%a %%b` |
| `skip=n` | 跳过前 n 行 | `for /f "skip=1 tokens=*" %%i in (data.csv) do echo %%i`（跳过表头） |
| `eol=` | 指定行注释符（遇到该字符开头的行则跳过） | `for /f "eol=#" %%i in (conf.txt) do echo %%i` |
| `usebackq` | 改变引号规则：`` ` ``反引号包裹命令、`"文件字符串"`、`'字符串'` | ``for /f "usebackq" %%i in (`dir /b`) do echo %%i`` |

## 流程控制（标签与跳转）

| 命令 | 说明 | 示例 |
|---|---|---|
| `:label` | 定义标签（相当于锚点） | `:start` |
| `goto label` | 无条件跳转到指定标签 | `goto start`（常用于构造循环） |
| `call :label` | 调用子程序（可传参，执行完会返回 call 的下一行） | `call :myfunc arg1` |
| `exit /b [code]` | 退出当前脚本（如果在 call 的子程序中，则返回调用处） | `exit /b 0` |
| `shift` | 参数左移（把 %2 变成 %1、%3 变成 %2，用于处理不定量参数） | 配合 `if "%1"=="" exit` 处理参数列表 |
| `CHOICE /C YN /M "确认吗"` | 等待用户输入指定选项，返回 ERRORLEVEL | `choice /c YN /M "继续?(Y/N)" & if errorlevel 2 exit` |

## 输入输出重定向

| 符号 | 说明 | 示例 |
|---|---|---|
| `>` | 将命令输出写入文件（覆盖原文件） | `echo hello > file.txt` |
| `>>` | 将命令输出追加到文件 | `echo world >> file.txt` |
| `2>` | 将标准错误输出重定向到文件 | `command 2> error.log` |
| `2>&1` | 将标准错误合并到标准输出 | `command > log.txt 2>&1` |
| `<` | 从文件中读取输入作为命令的输入 | `command < input.txt` |
| `\|` | 管道（将左边命令的输出作为右边命令的输入） | `dir /b \| find ".txt"` |

## 文件与目录操作进阶

| 命令 | 说明 | 示例 |
|---|---|---|
| `copy /Y` | 复制文件（/Y 覆盖不提示） | `copy /Y src.txt dst.txt` |
| `xcopy /E /I /Y` | 递归复制目录（含子目录和空目录） | `xcopy /E /I /Y dir1\ dir2\` |
| `robocopy` | 强大的文件复制工具（支持镜像、断点续传、多线程） | `robocopy src\ dst\ /MIR /MT:8` |
| `move /Y` | 移动或重命名文件 | `move /Y old.txt new.txt` |
| `del /F /Q /S` | 强制删除文件（/S 递归子目录） | `del /F /Q /S *.log` |
| `ren` / `rename` | 批量重命名文件（不支持路径，只能在当前目录） | `ren *.txt *.bak` |
| `md` / `mkdir` | 创建目录 | `md newdir` |
| `rd` / `rmdir /S /Q` | 删除目录（/S 递归，/Q 不提示） | `rd /S /Q olddir` |
| `attrib` | 查看或修改文件属性（只读/隐藏/系统） | `attrib +h file.txt`（隐藏文件） |
| `type` | 查看文件内容（类似 Linux 的 cat） | `type config.ini` |
| `more` | 分页查看文件内容 | `type huge.log \| more` |
| `findstr` | Windows 版的 grep（文本搜索） | `findstr "error" log.txt`；`findstr /S /I "pattern" *.txt` |
| `sort` | 排序 | `sort data.txt` |
| `assoc` | 查看或修改文件扩展名关联 | `assoc .txt=txtfile` |
| `ftype` | 查看或修改文件类型关联的打开程序 | `ftype txtfile=notepad.exe %1` |

## 网络相关命令

| 命令 | 说明 | 示例 |
|---|---|---|
| `ping` | 测试连通性 | `ping -n 4 baidu.com`（-n 指定次数） |
| `ipconfig` | 查看 IP 配置 | `ipconfig /all` |
| `netstat` | 查看端口和网络连接 | `netstat -ano \| findstr "8080"` |
| `netsh` | 网络配置工具（改 DNS/防火墙） | `netsh advfirewall firewall add rule name="Open80" dir=in action=allow protocol=TCP localport=80` |
| `tracert` | 路由追踪 | `tracert baidu.com` |
| `nslookup` | DNS 查询 | `nslookup baidu.com` |
| `curl` | 发起 HTTP 请求（Win10 1803+ 内置） | `curl -X POST -d "k=v" http://api.com` |
| `bitsadmin` | 文件下载（老旧但内置） | `bitsadmin /transfer job /download /priority normal http://url c:\file` |

## 注册表操作

| 命令 | 说明 | 示例 |
|---|---|---|
| `reg add` | 添加注册表项或值 | `reg add "HKLM\SOFTWARE\MyApp" /v Version /t REG_SZ /d "1.0" /f` |
| `reg delete` | 删除注册表项或值 | `reg delete "HKLM\SOFTWARE\MyApp" /v Version /f` |
| `reg query` | 查询注册表值 | `reg query "HKLM\SOFTWARE\MyApp" /v Version` |
| `reg export` | 导出注册表分支到文件 | `reg export "HKLM\SOFTWARE\MyApp" backup.reg` |
| `reg import` | 导入注册表文件 | `reg import backup.reg` |
| `HKLM` | 本地机器（全局配置），即 HKEY_LOCAL_MACHINE | - |
| `HKCU` | 当前用户（个人配置），即 HKEY_CURRENT_USER | - |
| `/t REG_SZ` | 字符串类型 | - |
| `/t REG_DWORD` | 32 位整数类型 | - |

## 服务与进程管理

| 命令 | 说明 | 示例 |
|---|---|---|
| `tasklist` | 列出所有进程 | `tasklist \| findstr "python"` |
| `taskkill /F /PID /T` | 强制杀死进程（/T 杀子进程树） | `taskkill /F /PID 1234`；`taskkill /F /IM notepad.exe`（按镜像名杀） |
| `sc query` | 查询服务状态 | `sc query MySQL80` |
| `sc start` | 启动服务 | `sc start MySQL80` |
| `sc stop` | 停止服务 | `sc stop MySQL80` |
| `sc config` | 配置服务（如设为自动启动） | `sc config MySQL80 start= auto` |
| `net start` | 启动服务（net 命令） | `net start MySQL80` |
| `net stop` | 停止服务（net 命令） | `net stop MySQL80` |
| `wmic` | 强大的 WMI 命令行工具（可做一切） | `wmic process where name="notepad.exe" delete` |
| `start` | 启动一个新窗口运行程序 | `start "" "C:\Program Files\App\app.exe"`（第一个参数是窗口标题，可留空） |

## 计划任务

| 命令 | 说明 | 示例 |
|---|---|---|
| `schtasks /create` | 创建计划任务 | `schtasks /create /tn "MyTask" /tr "C:\script.bat" /sc daily /st 00:00` |
| `schtasks /delete` | 删除计划任务 | `schtasks /delete /tn "MyTask" /f` |
| `schtasks /query` | 查看计划任务 | `schtasks /query /tn "MyTask" /v` |
| `/sc onlogon` | 用户登录时执行 | `/sc onlogon /ru "user"` |
| `/sc onstart` | 系统启动时执行 | `/sc onstart` |
| `/sc minute` | 每隔 N 分钟执行 | `/sc minute /mo 30`（每 30 分钟） |

## 实战：标准 BAT 脚本模板

以下是一个自动备份脚本的标准模板，演示了局部变量、错误判断、日志、清理等常规写法：

```bat
@echo off
:: ==========================================
:: 脚本名称: 自动备份脚本
:: 功能: 将源目录打包复制到备份目录，记录日志
:: ==========================================

:: 开启局部变量环境，防止污染系统
setlocal enabledelayedexpansion

:: 定义变量
set "SRC_DIR=C:\Projects\MyApp"
set "DST_DIR=D:\Backups\MyApp"
set "LOG_FILE=%DST_DIR%\backup.log"
set "DATE_STR=%DATE:~0,4%-%DATE:~5,2%-%DATE:~8,2%"
set "TIME_STR=%TIME:~0,2%-%TIME:~3,2%-%TIME:~6,2%"
set "BACKUP_NAME=backup_%DATE_STR%_%TIME_STR%"

:: 检查源目录是否存在
if not exist "%SRC_DIR%" (
  echo [%DATE% %TIME%] 错误: 源目录不存在 %SRC_DIR% >> "%LOG_FILE%"
  pause
  exit /b 1
)

:: 创建目标目录(如果不存在)
if not exist "%DST_DIR%" mkdir "%DST_DIR%"

:: 执行复制操作
echo [%DATE% %TIME%] 开始备份... >> "%LOG_FILE%"
xcopy "%SRC_DIR%" "%DST_DIR%\%BACKUP_NAME%\" /E /I /H /Y /C >> "%LOG_FILE%" 2>&1

:: 判断是否成功
if %ERRORLEVEL% equ 0 (
  echo [%DATE% %TIME%] 备份成功: %BACKUP_NAME% >> "%LOG_FILE%"
) else (
  echo [%DATE% %TIME%] 备份失败! 错误码: %ERRORLEVEL% >> "%LOG_FILE%"
)

:: 清理7天前的备份(高级用法，需forfiles命令)
forfiles /P "%DST_DIR%" /D -7 /C "cmd /c if @isdir==TRUE rd /S /Q @path" >> "%LOG_FILE%" 2>&1

echo 备份流程执行完毕，详细日志请查看: %LOG_FILE%
pause
endlocal
```

## 实战：常用单行 BAT 技巧

| 需求 | 命令 |
|---|---|
| 双击静默安装多个软件 | `start /wait msiexec /i app1.msi /quiet & start /wait app2.exe /S` |
| 批量修改文件后缀 | `ren *.jpg *.png` |
| 获取当前脚本所在路径 | `cd /d "%~dp0"` |
| 延时执行（无 sleep 命令） | `ping 127.0.0.1 -n 6 > nul`（延时 5 秒） |
| 根据日期创建文件夹 | `mkdir "%DATE:~0,4%_%DATE:~5,2%_%DATE:~8,2%"` |
| 一键清理系统垃圾 | `del /F /S /Q %temp%\* & del /F /S /Q C:\Windows\Temp\*` |
| 判断是否以管理员身份运行 | `net session >nul 2>&1 & if %errorLevel% == 0 (echo 是管理员) else (echo 不是管理员)` |
| 强制结束占用端口的进程（for 结合） | `for /f "tokens=5" %%a in ('netstat -aon ^\| findstr ":8080"') do taskkill /F /PID %%a` |
| 读取文本文件第一行 | `set /p first_line=< config.txt` |
| BAT 中调用 PowerShell 执行复杂逻辑 | `powershell -Command "(Get-Content file.txt) -replace 'old','new' \| Set-Content file.txt"` |

## 常见问题与排错（FAQ）

| 问题 | 原因与解决 |
|---|---|
| 中文乱码 | BAT 文件保存成了 UTF-8 编码；用记事本或 VS Code 另存为 ANSI(GBK) 编码 |
| 路径有空格报错 | 路径没有加双引号；所有路径变量必须加双引号：`cd "%PATH_VAR%"` |
| for 循环里变量不变 | for 循环默认在一开始就读取了 %var% 的值；在开头加 `setlocal enabledelayedexpansion`，并把 %var% 改成 !var! |
| 双击闪退 | 脚本执行完毕窗口自动关闭；在脚本最后一行加上 `pause` 命令 |
| set /a 算术报错 | 变量包含非数字字符；确保参与计算的变量是纯数字：`set /a sum=1+2` |
| findstr 太弱 | 不支持正则表达式的高级特性；复杂文本处理用 for /f 配合 PowerShell 替代 |
| 被杀毒软件拦截 | 使用了修改注册表、结束进程等敏感操作；添加白名单或用管理员权限运行 |
| 延迟扩展叹号问题 | 如果字符串本身包含 !，开启延迟扩展会被吞掉；先开启延迟扩展赋值，再关闭：`setlocal DisableDelayedExpansion` |

# PowerShell

## 基本概念与核心定位

PowerShell 是微软开发的跨平台任务自动化和配置管理框架（基于 .NET），包含脚本语言和 Shell。它的核心革命在于不再传递纯文本字符串，而是传递 .NET 对象——管道中可直接调用对象的方法和属性。

| 概念 | 说明 |
|---|---|
| `pwsh` | PowerShell 7+ 的可执行文件名（跨平台版本，支持 Linux/Mac） |
| `powershell.exe` | Windows 内置的 Windows PowerShell 5.1（基于 .NET Framework，仅 Windows） |
| 模块 | PowerShell 的功能扩展包（类似 Python 的包），通过 `Import-Module` 加载 |
| Cmdlet | PowerShell 内置的“动词-名词”命名法命令（如 `Get-Process`） |
| 别名 | PowerShell 为兼容旧习惯设置的简写（如 `ls` 实际是 `Get-ChildItem` 的别名） |
| PS 对象 | 任何命令输出的都不是纯文本，而是结构化的 .NET 对象（可直接 `.属性` 访问） |
| 与 Bash 区别 | Bash 字符串处理极强但类型弱；PS 有严格的类型系统且与 Windows 深度集成 |
| 适用场景 | Windows 系统运维、Active Directory 管理、Azure/Office365 管理、CI/CD 流水线、复杂自动化 |

## 安装与版本

| 版本 | 说明 | 环境 |
|---|---|---|
| Windows PowerShell 5.1 | 系统自带（不可轻易升级），基于 .NET Framework，仅限 Windows | 默认在 `C:\Windows\System32\WindowsPowerShell\v1.0\` |
| PowerShell 7.x（推荐） | 开源跨平台版本，基于 .NET 6/8，支持 Linux/Mac，功能最新 | 需手动下载安装 |

```bash
# 安装 PS7（Windows）
winget install Microsoft.PowerShell

# 安装 PS7（Linux - Debian/Ubuntu）
sudo apt install powershell

# 安装 PS7（Mac - Homebrew）
brew install powershell
```

查看版本用 `$PSVersionTable`，会显示详细的 PS 版本、OS、.NET CLR 版本信息。执行策略控制脚本运行权限，默认 Restricted（禁止运行）；推荐修改为：

```powershell
Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
```

（该策略允许本地脚本，远程脚本需签名。）

## 基础语法与核心约定

| 约定 | 说明 | 示例 |
|---|---|---|
| 动词-名词 | Cmdlet 必须使用此命名法，动词如 Get/Set/New/Remove | `Get-Process`, `New-Item` |
| 大小写不敏感 | 命令、变量、属性都不区分大小写 | `get-process` 等同于 `Get-Process` |
| 单引号 `'` | 字面量字符串，不解析变量 | `'Hello $name'` 输出 `Hello $name` |
| 双引号 `"` | 可解析变量和转义符 | `"Hello $name"` 输出 `Hello Tom` |
| 反引号 `` ` `` | 转义符（类似 Bash 的反斜杠） | `` "Hello `n World" ``（换行） |
| 分号 `;` | 同一行写多条命令 | `$a=1; $b=2; $c=$a+$b` |
| 反斜杠 `\` | 路径分隔符（Linux/Mac/PS7 也支持 `/`） | `C:\Users\` |
| 管道 `\|` | 将前一个命令的对象传递给后一个命令 | `Get-Process \| Where-Object {$_.CPU -gt 10}` |
| 注释 `#` | 单行注释 | `# 这是注释` |
| `<# #>` | 多行注释（块注释） | `<# 注释1 注释2 #>` |
| 强类型 | 变量可指定 .NET 类型 | `[int]$age = 25` |

## 变量与数据类型

| 操作 | 语法 | 说明 |
|---|---|---|
| 定义变量 | `$变量名 = 值` | 必须以 `$` 开头，等号两边可以有空格 |
| 使用变量 | `$变量名` | `echo $name` |
| 数据类型 | `[string]`、`[int]`、`[double]`、`[bool]`、`[array]`、`[hashtable]`、`[pscustomobject]` | 强类型声明：`[string]$str = "abc"` |
| 字符串拼接 | `"Hello " + $name` 或 `"Hello $name"` | 双引号内直接插入变量 |
| 数组 | `$arr = 1, 2, 3` 或 `$arr = @(1, 2, 3)` | `@()` 确保即使只有一个元素也是数组 |
| 哈希表/字典 | `$hash = @{Name="Tom"; Age=20}` | 键值对集合，用分号分隔 |
| 自定义对象 | `$obj = [PSCustomObject]@{Name="Tom"; Age=20}` | 极其重要！用于构造结构化数据输出 |
| 获取类型 | `$var.GetType().Name` | - |
| 判断类型 | `$var -is [string]` | 返回 True/False |
| 作用域 | `$global:var`、`$script:var`、`$local:var`、`$private:var` | 默认是 Local 作用域 |
| 强制删除变量 | `Remove-Variable var` 或 `rv var` | - |
| 特殊变量 | `$_`（管道当前对象）、`$Args`（脚本参数）、`$PSScriptRoot`（脚本所在目录） | - |

## 运算符

| 分类 | 运算符 | 说明 | 示例 |
|---|---|---|---|
| 算术 | `+ - * / %` | 加减乘除取模 | `5 % 2`（结果 1） |
| 赋值 | `= += -= *= /=` | 赋值与复合赋值 | `$a += 1` |
| 比较（等于） | `-eq` | 等于（不区分大小写） | `$a -eq "hello"` |
| 比较（不等于） | `-ne` | 不等于 | `$a -ne "hello"` |
| 比较（大小） | `-gt -lt -ge -le` | 大于/小于/大于等于/小于等于 | `$a -gt 10` |
| 比较（区分大小写） | `-ceq -cne -igt` | 加 c 前缀表示 Case-sensitive | `"ABC" -ceq "abc"`（False） |
| 通配符匹配 | `-like -notlike` | 支持 `*` 和 `?` 通配符 | `"file.txt" -like "*.txt"` |
| 正则匹配 | `-match -notmatch` | 使用正则表达式 | `"2023" -match "^\d+$"` |
| 替换 | `-replace` | 正则替换（默认不区分大小写，加 c 区分） | `"abc" -replace "a", "x"`（结果 xbc） |
| 包含 | `-contains -in` | 判断数组/集合是否包含元素 | `1,2,3 -contains 2` |
| 类型匹配 | `-is` | 判断对象类型 | `$a -is [int]` |
| 逻辑 | `-and -or -not` / `!` | 逻辑与或非 | `($a -gt 0) -and ($b -lt 10)` |

## 条件判断

| 语法 | 说明 | 示例 |
|---|---|---|
| `if () {} elseif () {} else {}` | 标准多条件判断 | `if ($age -gt 18) { "成年" } elseif ($age -gt 12) { "少年" } else { "儿童" }` |
| `switch (值) { 条件 {} }` | 多值匹配（类似其他语言的 case） | `switch ($color) { "red" { "红" } "blue" { "蓝" } default { "未知" } }` |
| `switch -regex` | 使用正则匹配 | `switch ($str) { "^A" { "以A开头" } }` |
| `switch -wildcard` | 使用通配符匹配 | `switch ($file) { "*.txt" { "文本文件" } }` |
| 三元运算符 | `条件 ? "真值" : "假值"` | `($age -gt 18) ? "是" : "否"` |
| 管道中的 If | `Where-Object`（或简写 `?`） | `Get-Process \| ? {$_.Name -like "note*"}` |

## 循环结构

| 循环类型 | 语法 | 说明 |
|---|---|---|
| `for` | `for ($i=0; $i -lt 10; $i++) { $i }` | 经典 C 风格循环 |
| `foreach` | `foreach ($item in $array) { $item }` | 遍历集合 |
| `foreach-object` | `$array \| ForEach-Object { $_ }` | 管道遍历（简写 `%`）：`1..5 \| % { $_ * 2 }` |
| `while` | `while ($i -lt 10) { $i++ }` | 先判断后执行 |
| `do-while` | `do { $i++ } while ($i -lt 10)` | 先执行后判断（至少执行一次） |
| `do-until` | `do { $i++ } until ($i -ge 10)` | 执行直到条件为真 |
| 流程控制 break | `break` | 跳出整个循环 |
| 流程控制 continue | `continue` | 跳过本次循环 |
| 标签跳出 | `:outer for (...) { for (...) { break outer } }` | 跳出外层循环 |

## 字符串处理

| 操作 | 语法 | 说明 |
|---|---|---|
| 多行字符串 | `@"` `"@` | Here-String，必须单独占一行 |
| 格式化 | `-string -f "格式"` | 类似 C 语言的 printf：`"{0} is {1} years old." -f "Tom", 25` |
| 拼接 | `+` 或双引号插值 | `"Hello " + "World"` |
| 截取 | `.Substring(起始索引, 长度)` | `"abcdef".Substring(1, 3)`（输出 bcd） |
| 分割 | `.Split("分隔符")` | `"a,b,c".Split(",")`（返回数组） |
| 替换 | `.Replace("旧", "新")` | `"abc".Replace("a", "x")` |
| 查找索引 | `.IndexOf("子串")` | `"abcdef".IndexOf("cde")`（返回 2） |
| 去空格 | `.Trim()` / `.TrimStart()` / `.TrimEnd()` | `" hello ".Trim()` |
| 判空 | `[string]::IsNullOrEmpty($var)` | 判断是否为空或 null |
| 判空白 | `[string]::IsNullOrWhiteSpace($var)` | 判断是否为空、null 或纯空格 |
| 转大写/小写 | `.ToUpper()` / `.ToLower()` | `"abc".ToUpper()` |

## 数组与集合操作

| 操作 | 语法 | 说明 |
|---|---|---|
| 创建数组 | `$arr = 1, 2, 3` 或 `$arr = @(1..10)` | `@()` 强制数组类型 |
| 空数组 | `$arr = @()` | - |
| 访问元素 | `$arr[0]` | 索引从 0 开始 |
| 最后一项 | `$arr[-1]` | 支持负数索引 |
| 添加元素 | `$arr += 4` | 实际上是创建新数组（性能差），大数组用 ArrayList |
| 删除元素 | 无法直接删除，需过滤 | `$arr = $arr \| Where-Object { $_ -ne "要删的值" }` |
| 数组长度 | `$arr.Count` 或 `$arr.Length` | - |
| 判断包含 | `$arr -contains "值"` | - |
| 排序 | `$arr \| Sort-Object` | - |
| 切片 | `$arr[1..3]` | 取出索引 1 到 3 的元素 |
| 遍历索引 | `for ($i=0; $i -lt $arr.Count; $i++) { $arr[$i] }` | - |
| 强类型数组 | `[int[]]$arr = 1, 2, "3"` | `"3"` 会被自动转为整数 3，如果写 `"abc"` 会报错 |

## 哈希表与自定义对象

| 操作 | 语法 | 说明 |
|---|---|---|
| 创建哈希表 | `$hash = @{"Key1"="Val1"; "Key2"="Val2"}` | - |
| 访问值 | `$hash["Key1"]` 或 `$hash.Key1` | - |
| 添加/修改 | `$hash["Key3"] = "Val3"` | - |
| 删除 | `$hash.Remove("Key1")` | - |
| 遍历 | `$hash.GetEnumerator() \| ForEach-Object { $_.Key }` | - |
| 创建 PSCustomObject | `$obj = [PSCustomObject]@{Name="Tom"; Age=20}` | 极其重要！ |
| 访问对象属性 | `$obj.Name` | - |
| 添加属性 | `Add-Member -NotePropertyName "City" -NotePropertyValue "BJ" -InputObject $obj` | 动态给对象加属性 |
| 对象转哈希表 | `$hash = $obj \| ConvertTo-Hashtable` | 需自定义函数或遍历属性 |
| 对象数组 | `$users = @([PSCustomObject]@{N="A"}, [PSCustomObject]@{N="B"})` | - |
| 导出对象 | `$obj \| Export-Csv -NoTypeInformation -Encoding UTF8 users.csv` | 导出为 CSV |
| 导入对象 | `$users = Import-Csv users.csv` | 从 CSV 导入为 PSCustomObject 数组 |

## 管道

| 概念 | 说明 | 示例 |
|---|---|---|
| 原理 | 将左边命令输出的 .NET 对象直接作为输入传递给右边命令 | `Get-Process \| Where-Object {$_.CPU -gt 10} \| Sort-Object CPU -Descending` |
| 管道变量 | `$_` 代表管道中当前正在处理的对象 | `Get-Service \| Where-Object {$_.Status -eq "Running"}` |
| 简写符号 | `?` 等同于 Where-Object，`%` 等同于 ForEach-Object | `Get-Process \| ? Name -like "note*" \| % { $_.Kill() }` |
| Select-Object | 选择对象的特定属性（投影） | `Get-Process \| Select-Object Name, ID, CPU`（简写 `select`） |
| Format-Table | 将对象格式化为表格输出（只改变显示，不改变对象类型） | `Get-Process \| Format-Table Name, CPU -AutoSize`（简写 `ft`） |
| Format-List | 将对象格式化为列表输出 | `Get-Process \| Format-List *`（简写 `fl`） |
| Out-File | 将管道输出保存到文件（默认 Unicode） | `Get-Process \| Out-File proc.txt -Encoding UTF8` |
| Tee-Object | 输出到屏幕的同时保存到文件 | `Get-Process \| Tee-Object proc.txt` |
| Measure-Object | 对对象属性进行统计（求和/平均/最大最小/计数） | `Get-ChildItem \| Measure-Object Length -Sum -Average` |
| 对比 CMD 管道 | CMD 传的是纯文本，需用字符串分割提取；PS 直接传对象、直接点属性 | PS 的核心优势 |

## 文件与目录操作

| Cmdlet | 说明 | 示例 |
|---|---|---|
| `Get-Item` | 获取单个文件/目录信息 | `Get-Item C:\test.txt` |
| `Get-ChildItem` | 获取目录内容（支持递归，等同 dir/ls） | `Get-ChildItem C:\temp -Recurse -Filter "*.log"`（简写 `gci`, `dir`, `ls`） |
| `New-Item` | 创建文件或目录 | `New-Item -Path "C:\newdir" -ItemType Directory`（简写 `ni`） |
| `Remove-Item` | 删除文件或目录（删目录加 -Recurse） | `Remove-Item "C:\old" -Recurse -Force`（简写 `rm`, `del`） |
| `Copy-Item` | 复制文件或目录 | `Copy-Item src.txt dst.txt`（简写 `cp`, `copy`） |
| `Move-Item` | 移动或重命名 | `Move-Item old.txt new.txt`（简写 `mv`, `move`） |
| `Rename-Item` | 重命名 | `Rename-Item old.txt new.txt`（简写 `ren`） |
| `Set-Content` | 将内容写入文件（覆盖） | `"hello" \| Set-Content out.txt`（简写 `sc`） |
| `Add-Content` | 追加内容到文件 | `"world" \| Add-Content out.txt`（简写 `ac`） |
| `Get-Content` | 读取文件内容（返回数组，每行一个元素） | `Get-Content log.txt`（简写 `gc`, `cat`） |
| `Clear-Content` | 清空文件内容（不删文件） | `Clear-Content log.txt` |
| `Test-Path` | 判断路径是否存在 | `Test-Path "C:\test.txt"`，返回 True/False |
| `Resolve-Path` | 获取绝对路径（解析 `.` 和 `..`） | `Resolve-Path .\file.txt` |
| `Split-Path` | 获取路径的父目录或文件名 | `Split-Path "C:\a\b.txt" -Parent`（输出 C:\a） |
| `Join-Path` | 拼接路径（自动处理斜杠） | `Join-Path "C:\temp" "file.txt"` |

## 高级文件处理

| 操作 | 语法 | 说明 |
|---|---|---|
| 按行读取处理 | `Get-Content file.txt \| ForEach-Object { $_ }` | 大文件避免一次性读入内存 |
| 流式读取 | `[System.IO.StreamReader]::new("file.txt")` | 极高性能读取超大文件 |
| 读取特定行 | `(Get-Content file.txt)[5]` | 读取第 6 行（索引从 0 开始） |
| 读取前 N 行 | `Get-Content file.txt -Head 5` | - |
| 读取尾 N 行 | `Get-Content file.txt -Tail 10` | - |
| 监视文件变化 | `Get-Content file.txt -Wait -Tail 10` | 类似 Linux 的 tail -f |
| 正则替换内容 | `(Get-Content in.txt) -replace 'old', 'new' \| Set-Content out.txt` | - |
| 处理 CSV 文件 | `Import-Csv` / `Export-Csv` | 处理表格数据的标准方式 |
| 处理 JSON 文件 | `Get-Content json.txt \| ConvertFrom-Json` | 转为 PSCustomObject |
| 导出 JSON | `$obj \| ConvertTo-Json -Depth 10 \| Set-Content out.json` | -Depth 控制嵌套深度 |
| 处理 XML 文件 | `[xml]$x = Get-Content config.xml` | 直接作为 XML 文档对象操作 |
| 处理 INI 文件 | `Get-Content ini.txt \| ForEach-Object { ... }` | 需手写解析逻辑 |
| 文件编码 | `-Encoding UTF8 / UTF8BOM / Default (ANSI)` | Get-Content 时指定避免乱码 |

## 注册表操作

| Cmdlet | 说明 | 示例 |
|---|---|---|
| `Get-ItemProperty` | 读取注册表值 | `Get-ItemProperty -Path "HKLM:\SOFTWARE\MyApp" -Name "Version"` |
| `New-Item -Path` | 创建注册表项（键） | `New-Item -Path "HKCU:\SOFTWARE\MyApp" -Force` |
| `New-ItemProperty` | 创建注册表值 | `New-ItemProperty -Path "HKCU:\SOFTWARE\MyApp" -Name "Port" -Value 8080 -Type DWord` |
| `Set-ItemProperty` | 修改注册表值 | `Set-ItemProperty -Path "HKCU:\SOFTWARE\MyApp" -Name "Port" -Value 9090` |
| `Remove-ItemProperty` | 删除注册表值 | `Remove-ItemProperty -Path "HKCU:\SOFTWARE\MyApp" -Name "Port"` |
| `HKLM:\` | 本地机器根节点（HKEY_LOCAL_MACHINE） | - |
| `HKCU:\` | 当前用户根节点（HKEY_CURRENT_USER） | - |
| 注册表类型 | String、ExpandString、DWord、QWord、Binary、MultiString | -Type 参数指定 |

## 服务与进程管理

| Cmdlet | 说明 | 示例 |
|---|---|---|
| `Get-Process` | 获取进程列表 | `Get-Process \| Where-Object {$_.WorkingSet64 -gt 100MB}` |
| `Stop-Process` | 杀死进程（按 ID 或名称） | `Stop-Process -Name "notepad" -Force`（简写 `kill`） |
| `Start-Process` | 启动新进程 | `Start-Process "notepad.exe" -ArgumentList "file.txt" -NoNewWindow` |
| `Wait-Process` | 等待进程结束 | `Wait-Process -Id 1234` |
| `Get-Service` | 获取服务列表 | `Get-Service \| Where-Object {$_.Status -eq "Running"}` |
| `Start-Service` | 启动服务 | `Start-Service -Name "wuauserv"` |
| `Stop-Service` | 停止服务 | `Stop-Service -Name "wuauserv" -Force` |
| `Set-Service` | 修改服务属性（如启动类型） | `Set-Service -Name "wuauserv" -StartupType Manual` |
| 重启服务 | `Restart-Service -Name "spooler"` | - |
| WMI 查询 | `Get-CimInstance Win32_Process` | 更底层的系统管理接口 |

## 网络操作

| Cmdlet | 说明 | 示例 |
|---|---|---|
| `Test-Connection` | 测试网络连通（类似 ping） | `Test-Connection -ComputerName baidu.com -Count 2` |
| `Resolve-DnsName` | DNS 解析（替代 nslookup） | `Resolve-DnsName baidu.com` |
| `Get-NetIPAddress` | 查看本机 IP 地址 | `Get-NetIPAddress -AddressFamily IPv4` |
| `Get-NetRoute` | 查看路由表 | - |
| `Test-NetConnection` | 综合网络测试（端口+路由+DNS） | `Test-NetConnection -ComputerName baidu.com -Port 443` |
| `Invoke-WebRequest` | 发起 HTTP 请求（类似 curl） | `$r = Invoke-WebRequest -Uri "http://api.com" ; $r.Content` |
| `Invoke-RestMethod` | 发起 REST 请求并自动解析 JSON（极其常用） | `$data = Invoke-RestMethod -Uri "http://api.com/data"` |
| NetSSH 模块 | SSH 连接（Linux） | `Install-Module Posh-SSH; Enter-PSSession -HostName ip -UserName root` |

## 模块管理

| Cmdlet | 说明 | 示例 |
|---|---|---|
| `Get-Module -ListAvailable` | 查看已安装的所有模块 | `Get-Module -ListAvailable` |
| `Import-Module` | 导入模块 | `Import-Module Az`（导入 Azure 模块） |
| `Remove-Module` | 卸载模块 | `Remove-Module Az` |
| `Find-Module` | 在在线库（PSGallery）搜索模块 | `Find-Module -Name "PowerShellGet"` |
| `Install-Module` | 在线安装模块（需管理员） | `Install-Module -Name "PowerShellGet" -Force -Scope CurrentUser` |
| `Update-Module` | 更新已安装的模块 | `Update-Module -Name "PowerShellGet"` |
| `Uninstall-Module` | 卸载已安装的模块 | `Uninstall-Module -Name "PowerShellGet"` |
| `$env:PSModulePath` | 模块搜索路径 | 模块必须放在此路径下才能自动发现 |
| PSGallery | PowerShell 官方模块仓库（类似 npm） | https://www.powershellgallery.com/ |

## 错误处理与调试

| 机制 | 语法 | 说明 |
|---|---|---|
| 非终止错误 | 命令报错但脚本继续执行 | 默认大部分错误 |
| 终止错误 | 命令报错且脚本立即停止 | 如语法错误、找不到命令 |
| `$ErrorActionPreference` | 全局错误处理偏好 | `$ErrorActionPreference = "Stop"`；值：Stop/SilentlyContinue/Inquire/Continue |
| `-ErrorAction` (EA) | 单条命令的错误处理 | `Get-Item "不存在.txt" -ErrorAction SilentlyContinue` |
| try-catch-finally | `try { } catch { } finally { }` | 捕获终止错误 |
| `$_.Exception` | 在 catch 块中获取异常对象详情 | `catch { Write-Host $_.Exception.Message }` |
| `Write-Error` | 写一条错误到错误流 | `Write-Error "自定义错误"` |
| `Write-Warning` | 写一条警告到警告流 | `Write-Warning "磁盘快满了"` |
| `Write-Debug` | 调试输出（需 `$DebugPreference="Continue"`） | `Write-Debug "变量值: $val"` |
| `Write-Verbose` | 详细输出（需 -Verbose 开关） | `Write-Verbose "开始处理..."` |
| `Set-PSDebug -Trace` | 开启脚本逐行跟踪 | `Set-PSDebug -Trace 1` |

## 实战：标准 PS1 脚本模板

```powershell
param(
  [string]$Name = "World",
  [int]$Count = 1,
  [switch]$Force
)

# 开启严格模式（变量未定义就报错，极其推荐！）
Set-StrictMode -Version Latest

# 错误处理偏好（遇到错误直接进 catch）
$ErrorActionPreference = "Stop"

# 日志函数
function Write-Log {
  param([string]$Message, [string]$Level = "INFO")
  $time = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
  $logStr = "[$time] [$Level] $Message"
  Write-Host $logStr
  # $logStr | Out-File "script.log" -Append -Encoding UTF8
}

try {
  Write-Log "脚本开始执行"
  Write-Log "参数: Name=$Name, Count=$Count, Force=$Force"

  # 业务逻辑
  for ($i=1; $i -le $Count; $i++) {
    Write-Log "正在处理第 $i 次..."
    # 模拟操作
    Start-Sleep -Seconds 1
  }

  Write-Log "脚本执行成功"
  exit 0
}
catch {
  Write-Log "发生严重错误: $($_.Exception.Message)" -Level "ERROR"
  exit 1
}
finally {
  Write-Log "清理资源..."
}
```

## 实战：常用单行技巧与对比

| 需求 | Windows CMD 写法 | PowerShell 写法（推荐） |
|---|---|---|
| 查找当前目录下所有大于 10MB 的文件 | 极难实现，需 for 循环 | `Get-ChildItem -Recurse \| Where-Object {$_.Length -gt 10MB} \| Select-Object Name, @{N="Size(MB)";E={[math]::Round($_.Length/1MB,2)}}` |
| 提取文本中所有的 IP 地址 | findstr + 极限正则 | `"192.168.1.1 test 10.0.0.1" -match "\d{1,3}(\.\d{1,3}){3}" ; $Matches[0]` |
| 批量重命名文件（加前缀） | ren 命令无法遍历子目录 | `Get-ChildItem -Recurse -Filter "*.jpg" \| Rename-Object -NewName {"IMG_" + $_.Name}` |
| 下载文件 | bitsadmin 或 curl | `curl.exe -O url` 或 `Invoke-WebRequest url -OutFile file` |
| 解析 JSON | 几乎不可能 | `$data = Get-Content json.txt \| ConvertFrom-Json ; $data.property` |
| 杀掉占用端口的进程 | for 循环+netstat+taskkill | `Get-NetTCPConnection -LocalPort 8080 -ErrorAction SilentlyContinue \| ForEach-Object { Stop-Process -Id $_.OwningProcess -Force }` |
| 静默安装 msi | `msiexec /i app.msi /quiet` | `Start-Process msiexec.exe -ArgumentList "/i app.msi /quiet" -Wait` |
| 获取本机公网 IP | `curl ifconfig.me` | `(Invoke-RestMethod http://ifconfig.me).ToString()` |

# 包管理：winget 与 Chocolatey

## 基本概念与核心定位

包管理器是自动化软件安装、更新、卸载的命令行工具（类似 Linux 的 apt/yum）。Windows 长久以来只能去网页下载 exe/msi，winget 和 choco 填补了 `apt install` 的空白。

| 概念 | 说明 |
|---|---|
| winget | Windows Package Manager，微软官方推出的命令行包管理器（Win10/11 内置） |
| Chocolatey | 第三方开源包管理器，生态最成熟，社区包最丰富（需手动安装） |
| Scoop | （补充）另一款流行的第三方包管理器，特点是不需要管理员权限、不污染环境变量 |
| 核心优势 | 告别网页点击“下一步”、自动处理依赖、一键静默安装、方便写脚本批量部署环境 |

## winget 与 Chocolatey 核心对比

| 对比维度 | winget（微软官方） | Chocolatey（第三方社区） |
|---|---|---|
| 安装方式 | Win10/11 系统内置（或通过应用商店更新） | 需用 PowerShell 脚本手动安装 |
| 管理员权限 | 安装大部分系统级软件需要管理员权限 | 默认需要管理员权限（Scoop 不需要） |
| 包仓库来源 | 微软官方仓库 + 各软件官方仓库（如 GitHub、Node.js 仓库） | 社区维护的中心仓库 |
| 包数量 | 相对较少（几千个），但都是主流软件 | 极其丰富（近万个），基本涵盖所有常用软件 |
| 安装原理 | 调用软件官方的 exe/msi 安装包进行静默安装 | 绝大多数是自己解压 nupkg 文件（干净无残留） |
| 更新速度 | 依赖软件自己的仓库更新机制 | 依赖社区作者提交更新（有时会滞后） |
| 搜索体验 | 支持模糊搜索，结果带版本号和来源说明 | 搜索结果较杂乱，包含各种版本和模块 |
| 卸载干净度 | 调用官方卸载程序（可能留注册表） | 直接删除文件夹，非常干净（但配置文件需手动备份） |
| 适用场景 | 装主流大厂软件、快速搭建基础环境 | 装小众工具、开发者工具、各种 CLI 命令行工具 |
| 一句话总结 | “Windows 界的 App Store 命令行版” | “Windows 界的 apt-get” |

## 安装与环境准备

| 工具 | 安装命令 | 验证命令 |
|---|---|---|
| winget | Win10(1709+) 内置；若没有，在微软商店搜索“应用安装程序”更新 | `winget -v` |
| Chocolatey | 以管理员身份运行 PowerShell（见下） | `choco -v` |

Chocolatey 的安装命令（管理员 PowerShell 中执行）：

```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

注意：Chocolatey 安装命令极长，建议直接去官网 chocolatey.org 首页复制；安装后需重启终端才能生效。两者不冲突，推荐同时安装——大软件用 winget，小工具用 choco。

## 通用基础命令对比

| 操作 | winget 语法 | Chocolatey 语法 |
|---|---|---|
| 搜索软件 | `winget search <关键词>` | `choco search <关键词>` |
| 安装软件 | `winget install <软件名/ID>` | `choco install <软件名>`（简写 `cinst`） |
| 卸载软件 | `winget uninstall <软件名/ID>` | `choco uninstall <软件名>`（简写 `cuninst`） |
| 更新指定软件 | `winget upgrade <软件名/ID>` | `choco upgrade <软件名>`（简写 `cup`） |
| 更新所有软件 | `winget upgrade` | `choco upgrade all`（简写 `cup all -y`） |
| 列出已安装 | `winget list` | `choco list --local-only` |
| 查看软件详情 | `winget show <软件名/ID>` | `choco info <软件名>` |
| 清理缓存 | `winget cache clean` | `choco cache clear` |
| 帮助 | `winget --help` | `choco -h` |

## 搜索命令详解

| 场景 | winget | Chocolatey |
|---|---|---|
| 基础搜索 | `winget search vscode` | `choco search vscode` |
| 精确搜索（包名） | `winget search "Visual Studio Code"` | `choco search vscode --exact` |
| 过滤来源 | `winget search vscode --source winget` | `choco search vscode` |
| 过滤架构 | `winget search python --architecture x64` | 不支持（安装时指定） |
| 搜索并分页 | `winget search python --count 10` | 不支持（直接全部列出） |

注意：winget 返回结果包含 ID 和 Name，推荐用 ID 安装；choco 返回结果较杂，可能包含依赖包，找带有 `[Approved]` 标志的。

## 安装命令详解

| 场景 | winget | Chocolatey |
|---|---|---|
| 基础安装 | `winget install Git.Git` | `choco install git` |
| 静默安装 | winget 默认基本静默（部分需加 `--silent --accept-package-agreements --accept-source-agreements`） | `choco install git -y`（-y 自动确认所有提示） |
| 指定安装路径 | `winget install Git.Git --location "D:\Software"` | `choco install git --install-directory "D:\Software"` |
| 指定架构（32/64 位） | `winget install Python.Python.3.11 --architecture x64` | `choco install python --x64`（或 `--forcex86`） |
| 跳过哈希校验 | `winget install <ID> --ignore-security-hash` | `choco install <name> --ignore-checksums` |
| 覆盖已安装 | `winget install <ID> --overwrite` | `choco install <name> --force` |
| 便携版安装 | `winget install <ID> --scope machine`（部分支持 portable 参数） | `choco install <name> --params "'/Portable'"`（看具体包说明） |

## 卸载与更新命令详解

| 场景 | winget | Chocolatey |
|---|---|---|
| 卸载 | `winget uninstall Git.Git` | `choco uninstall git` |
| 卸载并清理 | - | `choco uninstall git --purge` |
| 更新单个 | `winget upgrade Git.Git` | `choco upgrade git` |
| 更新全部 | `winget upgrade`（会有交互确认） | `choco upgrade all -y`（全自动无脑升级） |
| 列出可更新 | `winget upgrade`（直接列出可更新的列表） | `choco outdated` |
| 忽略某个版本更新 | `winget pin add Git.Git`（固定当前版本） | `choco pin add -n=git --version=2.40.0` |
| 取消忽略 | `winget pin remove Git.Git` | `choco pin remove -n=git` |

## 已安装软件管理

| 场景 | winget | Chocolatey |
|---|---|---|
| 查看全部已装 | `winget list` | `choco list --local-only` |
| 模糊查找已装 | `winget list python` | `choco list python` |
| 查看已装版本 | `winget list Git.Git` | `choco list git -v` |
| 导出已装列表 | `winget export -o packages.json` | `choco list --local-only -r > installed.txt` |
| 从列表批量安装 | `winget import -o packages.json` | （需配合其他脚本处理 txt） |

## winget 进阶特性

| 特性 | 说明 | 示例 |
|---|---|---|
| 包 ID 机制 | winget 使用“作者.软件名”格式的唯一 ID，避免重名 | `winget install Google.Chrome` |
| 多源管理 | 可以添加第三方源 | `winget source add name` |
| `--interactive` | 交互式安装（软件弹出 GUI 界面时用） | `winget install <ID> --interactive` |
| `--silent` | 强制静默安装（不弹窗） | `winget install <ID> --silent` |
| `--accept-package-agreements` | 自动接受软件许可协议 | `winget install <ID> --accept-package-agreements` |
| `winget settings` | 打开 JSON 配置文件修改默认行为 | `winget settings` |
| 配置默认安装路径 | 在 settings.json 中设置 installBehaviordefaults | `winget settings` |

## Chocolatey 进阶特性

| 特性 | 说明 | 示例 |
|---|---|---|
| 包参数 | 很多 choco 包支持自定义安装参数 | `choco install nodejs --params "'/InstallDir:D:\Node'"` |
| nupkg 文件本质 | 本质是 zip 包，包含 nuspec 元数据和解压后的文件 | 可直接解压提取绿色版 |
| 自定义包 | `choco new <name>` 可以自己打包内部软件分发 | `choco new mytool` |
| 私有源 | `choco push` 可以搭建内部 NuGet 服务器做私有源 | `choco push mytool.1.0.nupkg --source="http://internal-server/"` |
| 环境变量刷新 | choco 安装后自动刷新当前终端的环境变量（极其好用） | 装完 git 直接就能用 git 命令 |
| 安装后脚本 | 包可以自带 PowerShell 安装后脚本，用于做环境配置 | 很多包会自动配 Path |
| `--skip-virus-checks` | 跳过杀毒软件检测（公司电脑常被拦截） | `choco install <name> --skip-virus-checks` |

## 开发者必备工具安装速查表

| 类别 | 软件 | winget 命令 | Chocolatey 命令 |
|---|---|---|---|
| 版本控制 | Git | `winget install Git.Git` | `choco install git` |
| 编辑器 | VS Code | `winget install Microsoft.VisualStudioCode` | `choco install vscode` |
| 编辑器 | Vim | `winget install vim.vim` | `choco install vim` |
| 终端 | Windows Terminal | `winget install Microsoft.WindowsTerminal` | `choco install microsoft-windows-terminal` |
| 语言 | Python | `winget install Python.Python.3.11` | `choco install python` |
| 语言 | Node.js | `winget install OpenJS.NodeJS.LTS` | `choco install nodejs-lts` |
| 语言 | Go | `winget install GoLang.Go` | `choco install golang` |
| 语言 | Rust | rustup（建议用官方脚本） | `choco install rust` |
| 语言 | Java JDK | `winget install EclipseAdoptium.Temurin.17.JDK` | `choco install temurin17` |
| 容器 | Docker Desktop | `winget install Docker.DockerDesktop` | `choco install docker-desktop` |
| 数据库 | MySQL | `winget install Oracle.MySQL` | `choco install mysql` |
| 数据库 | Redis | （需通过 WSL 或第三方） | `choco install redis-64` |
| 数据库 | Navicat | `winget install PremiumSoft.NavicatPremium` | `choco install navicat` |
| 云 CLI | AWS CLI | `winget install Amazon.AWSCLI` | `choco install awscli` |
| 云 CLI | kubectl | `winget install Kubernetes.kubectl` | `choco install kubernetes-cli` |
| 基础工具 | 7-Zip | `winget install 7zip.7zip` | `choco install 7zip` |
| 基础工具 | Curl | `winget install curl.curl` | `choco install curl` |
| 基础工具 | Wget | `winget install wget` | wget（choco 里叫 wget） |
| 网络工具 | Clash（代理） | （需手动或第三方源） | `choco install clash` |

## 常见问题与排错（FAQ）

| 问题 | winget 表现及解决 | Chocolatey 表现及解决 |
|---|---|---|
| 提示找不到命令 | Win10 老版本未预装，去微软商店更新“应用安装程序” | 重启 PowerShell；检查环境变量 Path 中是否有 `C:\ProgramData\chocolatey\bin` |
| 安装报权限不够 | 右键开始菜单 -> 终端(管理员) 或 Windows Terminal(管理员) | 必须以管理员身份运行 PowerShell |
| 安装失败（哈希不匹配） | 网络问题或官方包更新了，用 `--ignore-security-hash` 跳过 | `choco install <name> --ignore-checksums` |
| 安装被杀毒拦截 | Windows Defender 偶尔误报，去安全中心放行 | `choco install <name> --skip-virus-checks` |
| choco 安装卡住不动 | - | 公司网络可能走了代理，设置 proxy：`choco config set proxy http://ip:port` |
| 装完找不到软件 | winget 有时没加 Path，需重启终端或注销 | choco 默认会自动刷新 Path |
| 如何完全卸载 choco | - | 删除 `C:\ProgramData\chocolatey` 文件夹，清理环境变量即可 |

## 实战：一键配置新电脑环境的脚本示例

Winget 批量安装（保存为 `winget-setup.ps1`，管理员运行）：

```powershell
Write-Host "===== 开始使用 Winget 安装基础软件 =====" -ForegroundColor Green
winget install Git.Git --silent --accept-package-agreements --accept-source-agreements
winget install Microsoft.VisualStudioCode --silent --accept-package-agreements
winget install 7zip.7zip --silent --accept-package-agreements
winget install Python.Python.3.11 --silent --accept-package-agreements
winget install OpenJS.NodeJS.LTS --silent --accept-package-agreements
winget install Microsoft.WindowsTerminal --silent --accept-package-agreements
winget install Docker.DockerDesktop --silent --accept-package-agreements
Write-Host "===== Winget 安装完成，建议重启电脑 =====" -ForegroundColor Green
```

Chocolatey 批量安装（保存为 `choco-setup.ps1`，管理员运行）：

```powershell
Write-Host "===== 开始使用 Chocolatey 安装开发者工具 =====" -ForegroundColor Green
choco install git -y
choco install vscode -y
choco install python -y
choco install nodejs-lts -y
choco install golang -y
choco install curl -y
choco install wget -y
choco install fzf -y        # 模糊搜索工具
choco install ripgrep -y    # 文本搜索神器
choco install jq -y         # JSON处理工具
choco install everything -y # 文件搜索神器
Write-Host "===== Chocolatey 安装完成 =====" -ForegroundColor Green
```

## 终极建议：什么时候用哪个？

| 场景 | 推荐工具 | 原因 |
|---|---|---|
| 安装 VS Code、Chrome、Office 等大厂主流软件 | winget | 调用官方原版安装包，最标准，后续更新走官方通道 |
| 安装各种命令行小工具（jq, fzf, curl, tree） | Chocolatey | 直接解压到 bin 目录，干净利落，无需安装向导 |
| 安装开发语言环境 | choco 或官方脚本 | choco 安装后自动配环境变量，无需手动改 Path |
| 在公司内网受限电脑 | 看哪个能连上外网 | 有些公司封了 choco.org 域名，但放行了微软的 CDN，反之亦然 |
| 写自动化部署脚本 | winget（稳定性）+ choco（丰富度）结合 | 脚本里先尝试 winget，失败了 fallback 到 choco |
| 卸载流氓软件/彻底清理 | Chocolatey | 卸载非常干净，不留注册表垃圾 |
| 需要便携版/绿色版 | Chocolatey | 很多包支持 `--params="'/Portable'"` |

# 附：Windows 激活记录

Windows 11/10 专业版激活密钥备忘：`PXTPF-QPNXG-BW8K8-TG88C-3YH26`

激活步骤：

1. 按下 `Win + I` 打开“设置”，在左侧选择“系统”，然后点击右侧的“激活”，找到并点击“更改产品密钥”，输入密钥后点击“下一步”验证。
2. 或者用命令行激活：右键开始图标，选择“Windows 终端（管理员）”，分开三次执行以下命令：

```bat
slmgr -ipk W269N-WFGWX-YVC9B-4J6C9-T83GX
slmgr -skms kms.0t.net.cn
slmgr -ato
```

等待命令执行完毕，出现“已成功激活产品”的提示即表示激活完成。
