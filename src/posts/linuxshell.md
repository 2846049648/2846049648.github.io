---
title: linux的shell脚本学习
date: 2026-09-03
category: 学习记录
tags: ["linux", "shell"]
excerpt: linux的shell脚本的粗略学习
---
# 文本操作
## （1）grep：搜索的大师

它用于在文件或输出中查找符合特定模式（支持正则表达式）的行。

常用场景：

grep "error" server.log：在日志文件中搜索包含 “error” 的行。

grep -i "warning" app.log：忽略大小写，搜索 “warning”。

grep -r "TODO" ./src/：递归搜索 src 目录下所有文件，找出包含 “TODO” 的行。

## （2）sed：非交互式的编辑器
它可以自动执行查找替换、删除、插入等编辑操作，非常适合在脚本中批量修改文件。

常用场景：

sed 's/old/new/g' file.txt：将 file.txt 中所有的 “old” 替换为 “new”，并输出结果。

sed -i 's/127.0.0.1/0.0.0.0/g' config.conf：直接修改 config.conf 文件，将其中的 IP 地址替换掉（-i 表示直接修改原文件）。

## （3）tr：字符转换器
它从标准输入读取数据，然后进行字符映射、删除或压缩。

常用场景：

echo "Hello" | tr 'a-z' 'A-Z'：将 “Hello” 转换为大写 “HELLO”。

echo $PATH | tr ':' '\n'：将 PATH 变量中的冒号 : 替换为换行符，让每个路径独占一行。

## （4）echo 输出命令，终端显示文本或变量。

常用场景：

echo "Hello, World!"：直接输出一段文本。

echo "当前用户是：$USER"：输出变量的值，$USER 会被替换为当前用户名。

echo -n "不换行输出"：输出文本后不换行，方便在同一行继续输出。

echo -e "第一行\n第二行"：启用转义字符，\n 表示换行。
# 程序操作
