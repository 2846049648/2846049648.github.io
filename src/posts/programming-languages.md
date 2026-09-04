---
title: 编程语言语法：Python · C++ · JavaScript
date: 2026-04-11
category: 学习记录
tags: ["python", "c++", "javascript", "位运算", "makefile"]
excerpt: Python、C++、JavaScript 语法速查，附 Makefile 与位运算
---

> 本文把几份语言类语法笔记合并为一份综合语法速查：`# Python 语法`、`# C++ 语法`、`# Makefile 构建`、`# JavaScript 语法`，末尾附一份 `# 位运算`（Python 与 C++ 对照）。各语言章节内部按知识点拆成 `##` 小节，表格用 GFM、示例代码用标注了语言的代码块，方便按目录快速检索。

# Python 语法

Python 用**缩进**表示代码块边界，变量无需声明类型。这一节按“基本语法 → 数据类型 → 字符串/运算符 → 条件循环 → 容器 → 函数与类 → 文件/异常 → 常用内置函数与模块 → 推导式 → 包管理”的顺序整理，适合当作日常速查表。

## 基本语法与变量

Python 里每一行语句就是一个语句块成员，同一代码块内的行必须有相同的缩进（常用 4 个空格）。变量赋值不需要声明类型，解释器会根据右侧值推断类型。

| 关键字/写法 | 说明 | 示例 |
|---|---|---|
| `#` | 单行注释 | `# 这是注释` |
| `""" ... """` | 多行注释（三个双引号或三个单引号均可） | `""" 这是注释 """` |
| 变量赋值 | 变量无需声明类型，直接赋值即可 | `name = "植物大战僵尸"` |
| `type()` | 查看变量的数据类型 | `print(type(3.14))  # <class 'float'>` |
| `input()` | 接收用户输入（返回字符串类型） | `age = input("请输入年龄：")` |
| `print()` | 输出内容到控制台 | `print("血量：", hp)` |
| `,` | 一行定义多个变量 | `a, b, c = 1, 2, 3` |
| PEP8 命名规范 | 变量名用小写字母 + 下划线 | `player_name = "玩家1"` |

## 数据类型

Python 常见内置类型如下：数字分整数与浮点，布尔首字母大写，容器有四类（列表/元组/字典/集合），另有表示“无”的 `None`。

| 类型 | 说明 | 示例 |
|---|---|---|
| `int` | 整数 | `integer = 100; negative = -5` |
| `float` | 浮点数（小数） | `price = 9.99; pi = 3.14159` |
| `bool` | 布尔值（首字母大写） | `is_alive = True; is_dead = False` |
| `str` | 字符串（单引号、双引号、三引号） | `s = '你好'; s2 = """多行字符串"""` |
| `list` | 列表（有序可变，用 `[]`） | `arr = [1, 2, "豌豆", True]` |
| `tuple` | 元组（有序不可变，用 `()`） | `t = (1, 2, 3)` |
| `dict` | 字典（键值对，用 `{}`） | `player = {"name": "玩家1", "hp": 100}` |
| `set` | 集合（无序不重复，用 `{}`） | `tags = {"游戏", "开发", "Python"}` |
| `None` | 空值（类似 JS 的 null） | `data = None` |

## 字符串操作

字符串是不可变序列，支持 `+` 拼接、`*` 重复、下标取值与切片，还提供一系列返回新字符串的方法。格式化推荐使用 f-string（性能最好）。

| 符号/方法 | 说明 | 示例 |
|---|---|---|
| `+` | 字符串拼接 | `s = "你好" + "世界"` |
| `*` | 字符串重复 | `s = "哈" * 3  # "哈哈哈"` |
| `[]` | 按索引取字符（从 0 开始，负数倒序） | `s = "hello"; s[0]  # 'h'; s[-1]  # 'o'` |
| `[:]` | 切片（左闭右开） | `s = "abcdef"; s[1:4]  # "bcd"; s[::2]  # "ace"` |
| `len()` | 获取字符串长度 | `len("hello")  # 5` |
| `.upper()` | 全部转大写 | `s.upper()` |
| `.lower()` | 全部转小写 | `s.lower()` |
| `.strip()` | 去除首尾空白字符 | `s.strip()` |
| `.split()` | 按分隔符拆分为列表 | `"a,b,c".split(",")  # ['a', 'b', 'c']` |
| `.join()` | 将列表合并为字符串 | `",".join(['a', 'b', 'c'])  # "a,b,c"` |
| `.replace()` | 替换子字符串 | `"hello".replace("l", "r")  # "herro"` |
| f-string | 格式化字符串（推荐，性能最好） | `f"血量：{hp} / {max_hp}"` |
| `.format()` | 格式化字符串（旧写法） | `"血量：{}/{}".format(hp, max_hp)` |
| `in` | 判断子字符串是否存在 | `"植物" in "植物大战僵尸"  # True` |

## 运算符

算术、比较、逻辑、身份与成员运算符汇总如下。Python 没有 `++`/`--`，自增自减写作 `x += 1`。按位运算符 `& | ~ ^ << >>` 在文末「# 位运算」中统一详述，这里不重复展开。

| 符号 | 说明 | 示例 |
|---|---|---|
| `+ - * /` | 加减乘除 | `5 + 3; 10 / 3  # 3.333...` |
| `//` | 整除（地板除） | `10 // 3  # 3` |
| `%` | 取余（求模） | `10 % 3  # 1` |
| `**` | 幂运算 | `2 ** 3  # 8` |
| `==` | 判断相等 | `1 == 1  # True` |
| `!=` | 判断不相等 | `1 != 2  # True` |
| `> < >= <=` | 大小比较 | `3 > 1  # True` |
| `and or not` | 逻辑运算符 | `True and False  # False` |
| `is` | 判断是否为同一个对象（身份比较） | `a is None` |
| `in / not in` | 成员运算符 | `3 in [1, 2, 3]  # True` |

## 条件判断与循环

条件判断用 `if / elif / else`，循环有 `for`（遍历可迭代对象）与 `while`。注意 Python 有独特的 `for...else`：循环正常结束（没有被 `break` 打断）时才执行 `else` 分支。

| 语法 | 说明 | 示例 |
|---|---|---|
| `if` | 条件判断 | `if hp <= 0: print("死亡")` |
| `elif` | 否则如果 | `elif hp < 30: print("危险")` |
| `else` | 否则 | `else: print("安全")` |
| 三元表达式 | 简写 if（值A if 条件 else 值B） | `status = "存活" if hp > 0 else "死亡"` |
| `for` | 遍历循环 | `for item in [1, 2, 3]: print(item)` |
| `range()` | 生成数字序列 | `range(5)  # 0,1,2,3,4`<br>`range(1, 10, 2)  # 1,3,5,7,9` |
| `while` | 条件循环（满足条件就一直执行） | `while hp > 0: hp -= 10` |
| `break` | 跳出整个循环 | `break` |
| `continue` | 跳过本次循环，进入下一次 | `continue` |
| `for...else` | 循环正常结束（没被 break）时执行 else | `for i in range(5): ...`<br>`else: print("完成")` |
| `pass` | 空占位语句（什么都不做，防止语法报错） | `if True: pass` |

## 列表操作

列表是 Python 最常用的可变序列，增删改查方法很齐全。绝大多数方法都是**原地修改**（如 `append`、`sort`），返回 `None`；需要新列表时用切片、`.copy()` 或 `sorted()`。

| 方法 | 说明 | 示例 |
|---|---|---|
| `.append()` | 末尾添加一个元素 | `arr.append("新元素")` |
| `.insert()` | 在指定索引处插入元素 | `arr.insert(0, "第一个")` |
| `.extend()` | 将另一个列表的所有元素追加进来 | `arr.extend([4, 5])` |
| `.pop()` | 删除末尾元素并返回该元素 | `arr.pop()` |
| `.pop(i)` | 删除指定索引的元素并返回 | `arr.pop(0)` |
| `.remove()` | 删除第一个匹配的值的元素 | `arr.remove("豌豆")` |
| `del` | 删除指定索引或整个变量 | `del arr[0]; del arr` |
| `.clear()` | 清空整个列表 | `arr.clear()` |
| `.index()` | 查找元素第一次出现的索引 | `arr.index("豌豆")` |
| `.count()` | 统计元素出现次数 | `arr.count("豌豆")` |
| `.sort()` | 原地排序（默认升序） | `arr.sort()` |
| `reverse=True` | 降序排序 | `arr.sort(reverse=True)` |
| `.reverse()` | 反转列表 | `arr.reverse()` |
| `len()` | 获取列表长度 | `len(arr)` |
| `[:]` | 列表切片（左闭右开） | `arr[1:3]  # 取索引 1 和 2` |
| `arr[start:stop:step]` | 带步长的切片 | `arr[::2]  # 隔一个取一个` |
| `.copy()` | 复制列表（浅拷贝） | `new_arr = arr.copy()` |
| `in` | 判断元素是否在列表中 | `"豌豆" in arr  # True` |

## 字典操作

字典以键值对存储，键必须可哈希。用 `[]` 取值时键不存在会抛 `KeyError`，用 `.get()` 则返回默认值更安全。遍历键值对推荐用 `.items()`。

| 方法 | 说明 | 示例 |
|---|---|---|
| `{key: value}` | 创建字典 | `player = {"name": "玩家1", "hp": 100}` |
| `d[key]` | 通过键取值（不存在会报错） | `player["name"]` |
| `d.get(key)` | 通过键取值（不存在返回 None/默认值，不报错） | `player.get("score", 0)` |
| `d[key] = value` | 添加或修改键值对 | `player["score"] = 999` |
| `del d[key]` | 删除指定键值对 | `del player["score"]` |
| `.pop(key)` | 删除并返回指定键的值 | `player.pop("hp")` |
| `.keys()` | 获取所有键 | `player.keys()  # dict_keys(['name', 'hp'])` |
| `.values()` | 获取所有值 | `player.values()  # dict_values(['玩家1', 100])` |
| `.items()` | 获取所有键值对（常配合 for 遍历） | `for k, v in player.items(): print(k, v)` |
| `.update()` | 合并另一个字典进来 | `player.update({"level": 5, "gold": 500})` |
| `in` | 判断键是否存在 | `"name" in player  # True` |
| `len()` | 获取键值对数量 | `len(player)` |

## 函数

用 `def` 定义函数，支持默认参数、`*args` 可变位置参数、`**kwargs` 可变关键字参数、`lambda` 匿名函数与 `->` 类型标注。函数内要给全局变量赋值需先用 `global` 声明。

| 语法 | 说明 | 示例 |
|---|---|---|
| `def` | 定义函数 | `def shoot(damage): print(f"造成{damage}伤害")` |
| `return` | 函数返回值（不写默认返回 None） | `def add(a, b): return a + b` |
| 默认参数 | 参数给默认值，调用时可省略 | `def plant(name, cost=50): ...` |
| `*args` | 可变位置参数（打包成元组） | `def fn(*args): print(args)` |
| `**kwargs` | 可变关键字参数（打包成字典） | `def fn(**kwargs): print(kwargs)` |
| `lambda` | 匿名函数（一行搞定简单逻辑） | `add = lambda a, b: a + b` |
| `global` | 在函数内部声明使用全局变量 | `global score; score += 10` |
| `->` | 类型标注（Python 3.5+）返回值类型提示 | `def add(a: int, b: int) -> int: return a + b` |

## 类与面向对象

Python 用 `class` 定义类，`__init__` 是构造方法，`self` 代表实例自身（类似 JS 的 this）。`super()` 调用父类方法，括号里写父类即为继承。`@property` 把方法变属性，`@staticmethod` 定义与实例无关的静态方法。

| 语法 | 说明 | 示例 |
|---|---|---|
| `class` | 定义类 | `class Plant: pass` |
| `__init__()` | 构造方法（初始化实例属性） | `def __init__(self, name, hp): self.name = name; self.hp = hp` |
| `self` | 实例自身（类似 JS 的 this） | `self.name = name` |
| `__str__()` | 定义 print 对象时的显示字符串 | `def __str__(self): return f"植物：{self.name}"` |
| 实例化对象 | 创建类的实例 | `sunflower = Plant("向日葵", 100)` |
| `.属性` | 访问实例属性 | `sunflower.name` |
| `.方法()` | 调用实例方法 | `sunflower.attack()` |
| `super()` | 调用父类的方法 | `super().__init__(name, hp)` |
| `class Sub(Parent)` | 继承（子类继承父类属性和方法） | `class Sunflower(Plant): pass` |
| `@property` | 把方法变成属性调用（类似 getter） | `@property def is_dead(self): return self.hp <= 0` |
| `@staticmethod` | 静态方法（不需要 self，和类无关） | `@staticmethod def game_info(): print("植物大战僵尸")` |
| `__name__` | 模块名判断（主程序入口） | `if __name__ == "__main__": main()` |

一个完整的示例：

```python
class Plant:
    def __init__(self, name, hp):
        self.name = name
        self.hp = hp

    def attack(self):
        print(f"{self.name} 发起攻击")

    @property
    def is_dead(self):
        return self.hp <= 0

sunflower = Plant("向日葵", 100)
print(sunflower.is_dead)  # False
```

## 文件操作

`open()` 返回文件对象，用完必须 `.close()`（否则占用资源）；更推荐用 `with open() as f` 上下文管理器，离开代码块自动关闭。文本模式默认按系统编码读写，`rb`/`wb` 用于图片、视频等二进制文件。

| 模式/方法 | 说明 | 示例 |
|---|---|---|
| `"r"` | 只读模式（默认） | `f = open("file.txt", "r")` |
| `"w"` | 写入模式（会覆盖原文件） | `f = open("file.txt", "w")` |
| `"a"` | 追加模式（在文件末尾添加） | `f = open("file.txt", "a")` |
| `"rb"` / `"wb"` | 二进制读写模式（图片、视频等） | `f = open("img.png", "rb")` |
| `.read()` | 读取全部内容 | `f.read()` |
| `.readline()` | 读取一行 | `f.readline()` |
| `.readlines()` | 读取所有行返回列表 | `f.readlines()` |
| `.write()` | 写入字符串 | `f.write("hello")` |
| `.writelines()` | 写入列表中的多个字符串 | `f.writelines(["a\n", "b\n"])` |
| `.close()` | 关闭文件（不关会占内存） | `f.close()` |
| `with open() as` | 上下文管理器（自动关闭文件，推荐） | `with open("file.txt", "r") as f: data = f.read()` |

## 异常处理

用 `try...except` 捕获异常防止程序崩溃；`except Exception as e` 兜底捕获所有异常；`finally` 无论是否出错都会执行，适合关闭资源；`raise` 主动抛出异常，`assert` 做断言。

| 语法 | 说明 | 示例 |
|---|---|---|
| `try...except` | 捕获异常（防代码崩溃） | `try: x = 1 / 0`<br>`except ZeroDivisionError: print("不能除以零")` |
| `Exception` | 捕获所有异常 | `except Exception as e: print(f"出错：{e}")` |
| `finally` | 无论是否出错都会执行（用于关闭资源） | `finally: f.close()` |
| `raise` | 主动抛出异常 | `raise ValueError("血量不能为负数")` |
| `assert` | 断言（条件不成立则抛 AssertionError） | `assert hp > 0, "血量必须大于0"` |

## 常用内置函数

不需要 import 就能直接用的高频内置函数，处理序列、数值、类型判断非常顺手。

| 函数 | 说明 | 示例 |
|---|---|---|
| `len()` | 获取长度 | `len([1, 2, 3])  # 3` |
| `range()` | 生成整数序列 | `list(range(5))  # [0, 1, 2, 3, 4]` |
| `enumerate()` | 遍历时同时获取索引和值 | `for i, v in enumerate(arr): print(i, v)` |
| `zip()` | 将多个列表打包成元组列表 | `for a, b in zip([1, 2], ['a', 'b']): print(a, b)` |
| `map()` | 对每个元素执行函数（返回迭代器） | `list(map(lambda x: x * 2, [1, 2, 3]))  # [2, 4, 6]` |
| `filter()` | 过滤满足条件的元素（返回迭代器） | `list(filter(lambda x: x > 2, [1, 2, 3, 4]))  # [3, 4]` |
| `sorted()` | 排序（返回新列表，不改变原列表） | `sorted([3, 1, 2])  # [1, 2, 3]` |
| `reversed()` | 反转（返回迭代器） | `list(reversed([1, 2, 3]))  # [3, 2, 1]` |
| `isinstance()` | 判断变量是否为某类型 | `isinstance(x, int)  # True` |
| `max()` / `min()` | 最大值 / 最小值 | `max([1, 2, 3])  # 3` |
| `sum()` | 求和 | `sum([1, 2, 3])  # 6` |
| `round()` | 四舍五入 | `round(3.14159, 2)  # 3.14` |
| `abs()` | 绝对值 | `abs(-5)  # 5` |
| `type()` | 查看数据类型 | `type("hello")  # <class 'str'>` |
| `id()` | 查看内存地址 | `id(x)` |

## 常用标准库模块

标准库覆盖面很广，以下是日常最常用的一批模块及其典型用法。

| 模块 | 说明 | 常用功能示例 |
|---|---|---|
| `import os` | 操作系统接口 | `os.path.exists("file.txt")`、`os.listdir(".")`、`os.remove("file.txt")`、`os.makedirs("dir")` |
| `import sys` | 系统相关参数 | `sys.argv`、`sys.exit()` |
| `import json` | JSON 数据处理 | `json.dumps(obj)` 对象转字符串；`json.loads(s)` 字符串转对象；`json.load(f)` 从文件读；`json.dump(obj, f)` 写入文件 |
| `import random` | 随机数模块 | `random.randint(1, 100)` 随机整数；`random.choice(arr)` 随机选一个；`random.shuffle(arr)` 打乱顺序 |
| `import time` | 时间模块 | `time.sleep(2)` 暂停 2 秒；`time.time()` 当前时间戳；`time.strftime("%Y-%m-%d")` 格式化时间 |
| `import math` | 数学模块 | `math.sqrt(16)  # 4.0`、`math.pi`、`math.ceil(3.2)  # 4`、`math.floor(3.8)  # 3` |
| `import re` | 正则表达式模块 | `re.findall(r"\d+", "abc123def")  # ['123']`；`re.sub(r"\s+", "", text)` 去除空格 |
| `import datetime` | 日期时间模块 | `datetime.datetime.now()`、`datetime.timedelta(days=7)` |
| `import collections` | 高级集合容器 | `collections.Counter([1, 1, 2])  # Counter({1: 2, 2: 1})`；`collections.defaultdict(list)` |
| `import shutil` | 文件/目录高级操作 | `shutil.copy("a.txt", "b.txt")`；`shutil.rmtree("dir")` 递归删除目录 |
| `import hashlib` | 哈希/加密模块 | `hashlib.md5(b"hello").hexdigest()` |
| `import threading` | 多线程模块 | `threading.Thread(target=func, args=(x,)); t.start()` |

## 列表推导式

列表推导式是 Python 的特色写法：用一条表达式替代 for + append 的循环。同理还有字典推导式、集合推导式与生成器表达式（生成器惰性求值、不占内存）。

| 语法 | 说明 | 示例 |
|---|---|---|
| `[x for x in list]` | 基础列表推导式 | `[x * 2 for x in range(5)]  # [0, 2, 4, 6, 8]` |
| `[x for x in list if 条件]` | 带条件的列表推导式 | `[x for x in range(10) if x % 2 == 0]  # [0, 2, 4, 6, 8]` |
| `{x: y for x, y in ...}` | 字典推导式 | `{k: v for k, v in items if v > 0}` |
| `{表达式 for x in list}` | 集合推导式 | `{x ** 2 for x in range(5)}  # {0, 1, 4, 9, 16}` |
| `(表达式 for x in list)` | 生成器表达式（不占内存，惰性求值） | `sum(x ** 2 for x in range(1000))` |

## 包管理与虚拟环境

第三方包统一通过 `pip` 管理；项目依赖一般用 `pip freeze > requirements.txt` 导出，再在别处用 `pip install -r requirements.txt` 一键还原。虚拟环境把每个项目的依赖隔离，避免互相污染。

| 命令 | 说明 |
|---|---|
| `pip install 包名` | 安装第三方包（如 `pip install requests`） |
| `pip install 包名==版本号` | 安装指定版本的包，如 `pip install requests==2.28.0` |
| `pip uninstall 包名` | 卸载包 |
| `pip list` | 查看已安装的所有包 |
| `pip show 包名` | 查看某个包的详细信息 |
| `pip freeze > requirements.txt` | 导出依赖列表（部署必备） |
| `pip install -r requirements.txt` | 根据文件批量安装依赖 |
| `python -m venv myenv` | 创建虚拟环境 |
| `source myenv/bin/activate` | 激活虚拟环境（Linux/Mac） |
| `myenv\Scripts\activate` | 激活虚拟环境（Windows） |
| `deactivate` | 退出虚拟环境 |

## 常用第三方库

用 `pip install` 安装的热门库：

| 库名 | 说明 | 安装命令 |
|---|---|---|
| `requests` | 最常用的 HTTP 请求库 | `pip install requests` |
| `flask` | 轻量级 Web 框架 | `pip install flask` |
| `django` | 全功能 Web 框架 | `pip install django` |
| `numpy` | 科学计算/矩阵运算 | `pip install numpy` |
| `pandas` | 数据分析/表格处理 | `pip install pandas` |
| `matplotlib` | 绘图可视化 | `pip install matplotlib` |
| `pillow` | 图片处理库 | `pip install pillow` |
| `scrapy` | 网页爬虫框架 | `pip install scrapy` |
| `pygame` | 游戏开发库 | `pip install pygame` |
| `beautifulsoup4` | HTML 解析库（爬虫常用） | `pip install beautifulsoup4` |

# C++ 语法

C++ 的难点集中在**内存管理、对象生命周期与泛型/模板**上。这一节按“内存 → 拷贝 → 多态 → 运算符重载 → 模板 → 移动语义 → STL → 并发 → C++17/20 新特性 → 编译调试”整理。

## 内存管理与底层机制

C++ 默认不提供垃圾回收，内存主要分**栈区**与**堆区**：栈上局部变量函数结束自动释放，速度快；堆上内存需手动 `new`/`delete`。现代 C++ 推荐用智能指针（`unique_ptr`/`shared_ptr`/`weak_ptr`）替代裸指针，配合 RAII 思想自动管理资源。

| 概念/关键字 | 说明 | 示例 |
|---|---|---|
| 栈区 | 局部变量，函数结束自动释放，速度快 | `void fn() { int a = 10; }  // a 在栈上` |
| 堆区 | 手动申请、手动释放，不会自动释放 | `int *p = new int(10); delete p;` |
| `new` | 在堆区分配内存并调用构造函数 | `Plant *p = new Plant("向日葵");` |
| `delete` | 释放堆区内存并调用析构函数 | `delete p;` |
| `delete[]` | 释放数组内存（必须加 `[]`，否则内存泄漏） | `int *arr = new int[10]; delete[] arr;` |
| 内存泄漏 | 申请的堆内存没有释放，长期占用系统内存 | 忘记写 `delete` |
| 智能指针 | 现代 C++ 替代 new/delete，自动释放内存 | `#include <memory>` |
| `unique_ptr` | 独占所有权智能指针（不可复制，只能移动） | `auto p = make_unique<int>(10);` |
| `shared_ptr` | 共享所有权智能指针（引用计数归零才销毁） | `auto p1 = make_shared<Plant>(); auto p2 = p1;` |
| `weak_ptr` | 弱引用指针（解决 shared_ptr 循环引用死锁） | `weak_ptr<Plant> wp = p1;` |
| `malloc/free` | C 语言内存函数（C++ 不推荐，不会调用构造函数） | `void *p = malloc(100); free(p);` |
| RAII | 资源获取即初始化（C++ 核心哲学），利用对象生命周期管理资源（锁、文件、内存） | 对象析构时自动释放资源 |

## 深拷贝与浅拷贝

编译器会**默认生成拷贝构造函数**，但它做的是浅拷贝——只复制指针地址，导致两个对象指向同一块堆内存，析构时 `double free` 报错。需要管理堆内存的类必须自己实现深拷贝（重新 `new` 内存并复制数据）。

| 概念 | 说明 | 示例 |
|---|---|---|
| 浅拷贝 | 编译器默认的拷贝构造直接复制指针地址，两个对象指向同一块堆内存（析构时 double free） | `Plant p2 = p1;  // 默认浅拷贝` |
| 深拷贝 | 在拷贝构造函数中重新 new 一块内存，把数据复制过去 | `Plant(const Plant& p) { this->hp = new int(*p.hp); }` |
| 拷贝构造函数 | 用一个已有对象初始化新对象时调用 | `Plant(const Plant& p);` |
| 拷贝赋值运算符 | 把一个对象赋值给另一个**已存在**的对象时调用 | `Plant& operator=(const Plant& p);` |
| 移动构造函数 | C++11 新增，stealing 资源（转移指针所有权，不分配新内存，极快） | `Plant(Plant&& p) noexcept;` |
| `= delete` | 禁用拷贝或赋值操作（单例模式常用） | `Plant(const Plant& p) = delete;` |

## 多态与虚函数机制

多态分两类：**静态多态**（编译期决定，函数重载、模板）与**动态多态**（运行期决定，靠虚函数 + 继承）。虚函数通过 **vptr 虚函数表**实现晚绑定；基类析构函数必须声明为 `virtual`，否则删除子类对象时子类析构不被调用，造成内存泄漏。

| 概念 | 说明 | 示例 |
|---|---|---|
| 静态多态 | 编译期决定调用哪个函数（函数重载、模板） | `int add(int a, int b); double add(double a, double b);` |
| 动态多态 | 运行期决定调用哪个函数（虚函数、继承） | `virtual void attack() = 0;` |
| 虚函数表 | vptr 指针指向的表，存储虚函数地址，实现晚绑定 | `class Base { virtual void fn() {} };` |
| 纯虚函数 | 虚函数后加 `= 0`，强迫子类必须重写，类变抽象类 | `virtual void draw() = 0;` |
| 抽象类 | 包含纯虚函数的类，不能直接实例化对象 | `class Shape { virtual void draw() = 0; };` |
| 虚析构函数 | 基类析构必须声明为 virtual，否则子类析构不被调用导致内存泄漏 | `virtual ~Base() {}` |
| `override` | C++11 关键字，显式声明重写，拼写错误会编译报错 | `void attack() override;` |
| `final` | C++11 关键字，禁止子类重写该函数，或禁止该类被继承 | `void attack() final;` |

## 运算符重载

运算符重载让自定义类型支持 `+ - * / []` 等运算。可以作为**类内成员函数**（左操作数是 `this`）或**全局函数**（左右操作数都作为参数，重载 `<<` 时配合友元）。

| 语法 | 说明 | 示例 |
|---|---|---|
| `返回类型 operator符号(参数)` | 重载定义格式 | `Complex operator+(const Complex& c)` |
| 类内成员函数重载 | 左操作数默认是 this | `Complex operator+(const Complex& c) { return Complex(real + c.real); }` |
| 全局函数重载 | 左右操作数都作为参数 | `ostream& operator<<(ostream& os, const Complex& c);` |
| 重载 `<<` | 配合友元实现 `cout << 对象` 输出自定义格式 | `cout << p1;` |
| 重载 `[]` | 让对象像数组一样用下标访问 | `int& operator[](int index) { return arr[index]; }` |
| 重载 `()` | 仿函数，让对象像函数一样被调用 | `class MyAdd { int operator()(int a, int b) { return a + b; } };` |
| 重载 `->` / `*` | 智能指针必须重载这两个才能像原生指针一样使用 | `p->attack(); *p;` |
| 重载 `++` / `--` | 区分前置和后置（后置多一个 int 占位参数） | `MyClass& operator++(); MyClass operator++(int);` |

## 模板进阶

模板是泛型编程的基础；进阶点包括特化、偏特化、非类型模板参数、可变参模板与 SFINAE（替换失败不是错误）。C++11 起推荐用 `using` 别名模板替代 `typedef`。

| 概念 | 说明 | 示例 |
|---|---|---|
| 模板特化 | 针对特定类型提供特殊实现 | `template<> void print<string>(string s) { ... }` |
| 偏特化 | 只特化部分模板参数 | `template<typename T1, typename T2> class A;`<br>`template<typename T> class A<T, int>;` |
| 非类型模板参数 | 模板参数可以是常量整数 | `template<int N> void fn() { int arr[N]; }`<br>`fn<10>();` |
| `typename` | 解决模板中的依赖类型名称问题 | `typename T::iterator it;` |
| 可变参模板(C++11) | 接收任意数量、任意类型的参数 | `template<typename... Args> void print(Args... args);` |
| `decltype` | 推导表达式的类型 | `decltype(a + b) c;` |
| using 别名模板(C++11) | 替代 typedef，更直观 | `using VecInt = std::vector<int>;` |
| SFINAE | 替换失败不是错误（模板元编程基础） | `enable_if`, `type_traits` |

## 右值引用与移动语义

C++11 引入右值引用 `T&&` 实现**移动语义**：把不再需要的对象的堆资源“偷”过来（转移指针所有权），避免深拷贝的开销。`std::move()` 并不搬运数据，只是把左值“无条件转为右值引用”。

| 概念 | 说明 | 示例 |
|---|---|---|
| 左值 | 有名字、可以取地址的对象 | `int a = 10;  // a 是左值` |
| 右值 | 没名字、不能取地址（字面量、临时对象等） | `10; "hello"; x + y;` |
| 右值引用 `T&&` | 绑定到右值上 | `int&& rref = 10;` |
| `std::move()` | 无条件把左值转为右值引用（不搬数据，只是转换类型） | `string str2 = std::move(str1);  // str1 变为空` |
| 移动语义 | 通过转移资源所有权（偷指针）避免深拷贝开销 | `vector<string> v2 = std::move(v1);` |
| 完美转发 | `std::forward<T>(t)` 保持参数的左值/右值属性传递给下一个函数 | `template<typename T> void wrapper(T&& arg) { process(std::forward<T>(arg)); }` |

## STL 迭代器与空间配置器

迭代器是容器通用的“泛型指针”，提供统一遍历方式。STL 的半开区间约定 `[begin, end)`：`end()` 指向最后一个元素的下一个位置。空间配置器负责容器内存的申请与释放（默认用 `new`，可自定义）。

| 概念 | 说明 | 示例 |
|---|---|---|
| 迭代器 | 泛型指针，提供统一方式遍历不同容器 | `vector<int>::iterator it = v.begin();` |
| `begin()` / `end()` | 指向第一个元素 / 指向最后一个元素的下一个位置 | `for (auto it = m.begin(); it != m.end(); ++it)` |
| `cbegin()` / `cend()` | 返回 const 迭代器（不能修改元素） | `auto it = v.cbegin();` |
| `rbegin()` / `rend()` | 反向迭代器（从后往前遍历） | `for (auto it = v.rbegin(); it != v.rend(); ++it)` |
| 空间配置器 | 负责容器内存的申请与释放（默认 new，可自定义） | `allocator<int> alloc; int *p = alloc.allocate(5);` |

## 并发与多线程

C++11 起标准库自带线程支持：`<thread>` 创建线程，`<mutex>` 提供互斥锁，`<atomic>` 提供无锁原子操作，`<future>` 支持异步任务。RAII 风格的 `std::lock_guard` 离开作用域自动解锁，比手写 `lock/unlock` 更安全。

| 头文件/类 | 说明 | 示例 |
|---|---|---|
| `<thread>` | 线程库头文件 | `#include <thread>` |
| `std::thread` | 创建线程对象 | `thread t(func, args); t.join();` |
| `t.join()` | 阻塞主线程，等待子线程执行完毕 | `t.join();` |
| `t.detach()` | 分离线程，在后台独立运行（不用等） | `t.detach();` |
| `<mutex>` | 互斥锁头文件 | `#include <mutex>` |
| `std::mutex` | 互斥锁（保护共享数据防竞争） | `std::mutex m; m.lock(); ... m.unlock();` |
| `std::lock_guard` | RAII 机制的锁（离开作用域自动解锁，极常用） | `std::lock_guard<std::mutex> lock(m);` |
| `std::unique_lock` | 更灵活的 RAII 锁（支持手动解锁、配合条件变量） | `std::unique_lock<std::mutex> lock(m);` |
| `<atomic>` | 原子操作头文件（无锁并发） | `#include <atomic>` |
| `std::atomic<int>` | 原子整型（多线程下安全自增，替代 mutex） | `atomic<int> count = 0; count++;` |
| `<future>` | 异步任务头文件 | `#include <future>` |
| `std::async` | 异步执行任务，返回 future 获取结果 | `future<int> f = async(launch::async, func);` |
| `std::promise` / `std::future` | 线程间传递值的通道 | `promise<int> p; future<int> f = p.get_future();` |
| `this_thread::sleep_for` | 当前线程休眠 | `std::this_thread::sleep_for(chrono::seconds(1));` |

## C++17/20 现代新特性

C++17 补上了大量“手感”改进：结构化绑定、`std::optional`/`variant`/`any`、`if constexpr`、`std::filesystem`；C++20 则带来 Concepts（约束模板）、Ranges（管道式算法）、协程与三路比较运算符。

| 特性 | 说明 | 示例 |
|---|---|---|
| 结构化绑定(C++17) | 一次性解包 pair/tuple/数组/结构体 | `auto [x, y] = make_pair(10, 20);`<br>`auto [key, val] = *map_it;` |
| `std::optional`(C++17) | 表示值可能存在也可能不存在（替代返回 -1 或 nullptr） | `optional<int> findUser();` |
| `std::variant`(C++17) | 类型安全的联合体（能存多种指定类型中的一种） | `std::variant<int, string> v = "hello";` |
| `std::any`(C++17) | 能装任何类型的容器（类似 Python 的 object，但有类型安全检查） | `std::any a = 10; a = string("hi");` |
| `if constexpr`(C++17) | 编译期 if 判断（模板中根据类型编译不同分支） | `if constexpr (is_integral<T>::value)` |
| `std::filesystem`(C++17) | 跨平台文件系统操作库 | `namespace fs = std::filesystem; fs::exists("test.txt");` |
| Concepts(C++20) | 模板约束，用通俗语言限制模板类型，替代晦涩的 SFINAE | `template<typename T> requires Integral<T> void fn(T t)` |
| Ranges(C++20) | 管道式算法写法，代码极其优雅 | 见下方代码块 |
| 协程(C++20) | 轻量级异步函数（标准库支持不完整，多依赖第三方如 asio） | `Task<int> fetchData();` |
| 三路比较 `<=>`(C++20) | 太空船运算符，一次定义所有比较运算符 | `auto operator<=>(const MyClass&) const = default;` |

Ranges 管道写法示例：

```cpp
#include <ranges>
#include <vector>

std::vector<int> v{1, -2, 3, -4, 5};
auto res = v | std::views::filter([](int i) { return i > 0; })
             | std::views::transform([](int i) { return i * 2; });
```

## 编译与调试常用命令

Linux 下用 `g++` 编译，配合 GDB 调试。常用参数：`-o` 指定输出名，`-c` 只编译不链接，`-g` 生成调试信息，`-Wall` 开常见警告，`-std=c++17` 指定标准，`-I/-L/-l` 分别指定头文件、库路径与链接库。

| 命令/参数 | 说明 | 示例 |
|---|---|---|
| `g++ main.cpp` | 编译生成默认的 `a.out` 可执行文件 | `g++ main.cpp` |
| `-o` | 指定输出的可执行文件名 | `g++ main.cpp -o game` |
| `-c` | 只编译生成目标文件，不链接 | `g++ -c utils.cpp` |
| `-g` | 生成调试信息（GDB 调试必须加） | `g++ -g main.cpp -o game` |
| `-Wall` | 开启所有常见警告（好习惯，必须加） | `g++ -Wall main.cpp` |
| `-std=c++17` | 指定 C++ 标准版本 | `g++ -std=c++17 main.cpp` |
| `-I` | 指定头文件搜索路径 | `g++ -I./include main.cpp` |
| `-L` | 指定库文件搜索路径 | `g++ -L./lib main.cpp` |
| `-l` | 链接指定的库（如 pthread 多线程库） | `g++ main.cpp -lpthread` |
| `-D` | 在命令行定义宏 | `g++ -DDEBUG main.cpp` |
| Makefile | 工程自动化构建脚本，管理多文件编译 | `make`（执行 make 命令） |
| CMake | 跨平台工程构建工具生成器（现代大项目标配） | `cmake ..; make` |
| `gdb` | GNU 调试器启动 | `gdb ./game` |
| `break`(b) | 在 gdb 中打断点 | `b main.cpp:20` |
| `run`(r) | 在 gdb 中运行程序 | `r` |
| `next`(n) | 单步跳过（不进函数内部） | `n` |
| `step`(s) | 单步进入（进入函数内部） | `s` |
| `print`(p) | 打印变量值 | `p hp` |

# Makefile 构建

Makefile 是 C/C++ 工程自动化构建的经典方案：它描述“目标依赖哪些文件、如何生成目标”，由 `make` 根据文件时间戳判断哪些需要重新编译。由于它常与 C++ 一起使用，这里紧接在 C++ 之后。

## 基本语法与核心概念

一条规则的格式是“目标: 依赖”，下一行写要执行的 Shell 命令，**命令前必须是 Tab 缩进**（不是空格）。

| 概念/语法 | 说明 |
|---|---|
| 目标 | 需要生成的文件（可执行文件、.o 文件、伪目标） |
| 依赖 | 生成目标所需要的源文件或其他目标 |
| 命令 | 生成目标所执行的 Shell 命令（必须用 Tab 键缩进！） |
| 规则 | 目标 + 依赖 + 命令 的组合体 |
| `$@` | 自动化变量：代表当前规则的目标名 |
| `$^` | 自动化变量：代表当前规则的所有依赖文件 |
| `$<` | 自动化变量：代表当前规则的第一个依赖文件 |
| `#` | 注释 |

```makefile
# 格式：目标: 依赖
# 命令必须以 Tab 开头
main: main.o utils.o
	g++ main.o utils.o -o main
```

## 基础四步编译模型

最朴素的多文件项目会写四条规则：每个 `.cpp` 编成 `.o`，再把 `.o` 链成可执行文件，外加一个清理用的 `clean`。

```makefile
main.o: main.cpp
	g++ -c main.cpp

utils.o: utils.cpp
	g++ -c utils.cpp

main: main.o utils.o
	g++ main.o utils.o -o main

clean:
	rm -f *.o main
```

## 变量定义与使用

变量类似 C 语言的宏替换。`=` 是递归展开赋值（用到时才替换，可能互相引用成死循环），`:=` 是立即展开赋值（推荐），`?=` 只在变量未定义时赋值，`+=` 追加内容。

| 语法 | 说明 | 示例 |
|---|---|---|
| `=` | 递归展开赋值（使用时才真正替换，可能死循环） | `CC = g++` |
| `:=` | 立即展开赋值（定义时直接替换，推荐） | `CC := g++` |
| `?=` | 如果未定义才赋值（不覆盖已有值） | `CC ?= clang` |
| `+=` | 追加内容 | `CFLAGS += -Wall` |
| `$(变量名)` | 引用变量（推荐写法） | `$(CC) $(CFLAGS) main.cpp` |
| `${变量名}` | 引用变量（另一种写法） | `${CC}` |
| `$(VAR:old=new)` | 替换变量中的字符串 | `$(SRC:.cpp=.o)` 把后缀 `.cpp` 替换为 `.o` |

## 常用预定义变量与环境变量

make 自带一批约定俗成的变量名，直接使用它们能让 Makefile 更通用、更容易被其他开发者看懂。

| 变量名 | 说明 | 默认值/常用设置 |
|---|---|---|
| `CC` | C 语言编译器 | `gcc` |
| `CXX` | C++ 编译器 | `g++` |
| `CFLAGS` | C 编译选项 | `CFLAGS := -Wall -g` |
| `CXXFLAGS` | C++ 编译选项 | `CXXFLAGS := -std=c++17 -Wall -g` |
| `LDFLAGS` | 链接器选项（如库路径） | `LDFLAGS := -L./lib` |
| `LDLIBS` | 链接的库文件（如 -lpthread） | `LDLIBS := -lpthread` |
| `MAKE` | 当前 make 命令的路径 | `$(MAKE)` |

## 自动变量推导与模式规则

模式规则用 `%` 通配符把“一类文件”的编译规则写一次，配合 `wildcard`/`patsubst` 自动收集源文件，能大幅减少重复代码。

```makefile
SRC := $(wildcard *.cpp)              # 收集当前目录所有 .cpp
OBJ := $(patsubst %.cpp, %.o, $(SRC)) # 把 .cpp 列表换成 .o 列表

%.o: %.cpp                            # 模式规则：任意 .o 依赖同名 .cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
```

## 伪目标声明

`clean`、`all` 这类名字不代表真实文件，称为伪目标。若不声明 `.PHONY`，当目录里恰好存在同名文件（比如一个叫 `clean` 的文件）时，`make clean` 会认为目标已是最新而不执行。

| 语法 | 说明 | 示例 |
|---|---|---|
| `.PHONY:` | 声明伪目标（告诉 make 这个名字不代表真实文件） | `.PHONY: clean all` |
| 为什么需要 | 目录下若恰好有叫 clean 的文件，make clean 就不会执行 | `rm -f *.o main` |
| 常见伪目标 | all、clean、install、uninstall | `.PHONY: all clean install` |

## 条件判断

用 `ifeq/ifneq` 判断变量相等性，用 `ifdef/ifndef` 判断变量是否定义，结尾都要写 `endif`。典型用途：根据是否定义了 `DEBUG` 决定是否加 `-g` 调试选项。

```makefile
ifeq ($(CC), g++)
	# 编译器是 g++ 时的分支
endif

ifdef DEBUG
	CXXFLAGS += -g
endif
```

## 常用内置函数

make 内置函数通过 `$(函数名 参数)` 调用，常用于处理文件列表与字符串。

| 函数 | 说明 | 示例 |
|---|---|---|
| `$(wildcard pat)` | 扫描文件系统获取匹配的文件 | `SRC := $(wildcard src/*.cpp)` |
| `$(patsubst a,b,$(var))` | 把 var 中的 a 替换成 b | `OBJ := $(patsubst %.cpp,%.o,$(SRC))` |
| `$(notdir $(var))` | 去掉路径，只留文件名 | `FILE := $(notdir src/a.cpp)  # a.cpp` |
| `$(dir $(var))` | 只保留路径部分 | `DIR := $(dir src/a.cpp)  # src/` |
| `$(shell command)` | 执行 Shell 命令，返回结果 | `DATE := $(shell date +%Y%m%d)` |
| `$(subst a,b,$(var))` | 简单字符串替换（不认通配符 %） | `$(subst .cpp,.o,$(SRC))` |
| `$(filter pattern,$(var))` | 从 var 中筛选出符合 pattern 的字符串 | `$(filter %.cpp,$(SRC))` |
| `$(filter-out pattern,$(var))` | 从 var 中剔除符合 pattern 的字符串 | `$(filter-out %test.cpp,$(SRC))` |

## 高级特性

| 特性 | 说明 | 示例 |
|---|---|---|
| `@` 符号 | 命令前加 @，不打印命令本身（静默执行） | `@echo "编译完成..."` |
| `-` 符号 | 命令前加 -，忽略该命令的报错（继续执行） | `-mkdir build` |
| `include` | 引入其他 Makefile 文件（类似 C 的 include） | `include config.mk` |
| `export` | 将变量传递给子 Makefile（嵌套 make 时使用） | `export CC CFLAGS` |
| `override` | 防止命令行参数覆盖 Makefile 中的变量 | `override CFLAGS += -g` |
| `define/endef` | 定义多行变量（宏包） | 见下方代码块 |
| 多目标规则 | 一个规则生成多个目标（依赖相同时合并） | `main.o utils.o: common.h` |

```makefile
# define/endef 定义多行宏
define COMPILE_CMD
	$(CXX) -c $< -o $@
endef
```

## 实战：一个标准的 C++ 项目 Makefile 模板

综合上面的知识点，一个把源码放 `src/`、中间文件放 `build/` 的 C++ 项目模板如下：

```makefile
# 定义变量
CXX := g++
CXXFLAGS := -std=c++17 -Wall -g
TARGET := my_game
BUILD_DIR := build
SRC_DIR := src

# 自动查找源文件和头文件
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))

# 伪目标
.PHONY: all clean $(BUILD_DIR)

# 默认目标
all: $(BUILD_DIR) $(TARGET)

# 创建 build 目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 链接生成最终可执行文件
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

# 模式规则：编译 .cpp 生成 .o（放到 build 目录下）
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# 重新编译
rebuild: clean all
```

## make 命令行常用参数

| 参数 | 说明 | 示例 |
|---|---|---|
| `make` | 默认执行 Makefile 中的第一个目标 | `make` |
| `make -f file` | 指定读取的 Makefile 文件名 | `make -f Makefile.linux` |
| `make -j N` | 开启 N 个线程并行编译（4 核用 -j4） | `make -j4` |
| `make -n` | 只打印要执行的命令但不实际执行（干跑） | `make -n` |
| `make -C dir` | 切换到指定目录执行 make | `make -C src` |
| `make clean` | 执行 clean 目标 | `make clean` |
| `make VAR=value` | 在命令行临时覆盖 Makefile 中的变量 | `make CFLAGS="-O2"` |
| `make -B` | 强制重新编译所有目标（无视时间戳） | `make -B` |
| `make -d` | 打印调试信息（排查 Makefile 问题） | `make -d` |

# JavaScript 语法

JavaScript 是弱类型、基于原型的脚本语言，ES6+ 之后补上了块级作用域、class、模块、Promise 等现代语法。这一节按“变量声明 → 数据类型 → 运算符 → 流程控制 → 函数 → 对象/数组方法 → ES6+ 特性”整理。

## 基本语法与变量声明

变量声明有三种：`var`（函数作用域、有变量提升，逐渐被淘汰）、`let`（块级作用域、可重新赋值，现代首选）、`const`（块级作用域、不可重新赋值）。语句末尾的分号可省略，但建议保留避免歧义。

| 关键字/符号 | 说明 | 示例 |
|---|---|---|
| `var` | 旧版变量声明（函数作用域、变量提升，淘汰中） | `var name = "植物";` |
| `let` | 现代变量声明（块级作用域，可重新赋值） | `let score = 100; score = 200;` |
| `const` | 常量声明（块级作用域，声明后不可重新赋值） | `const PI = 3.14159;` |
| `;` | 分号（可省略，但建议加上防坑） | `let a = 1;` |
| `//` | 单行注释 | `// 这是注释` |
| `/* */` | 多行注释 | `/* 这是注释 */` |

## 数据类型

JS 的类型分“原始类型”与“对象类型”。`Number` 不区分整数与浮点，还包含 `NaN`、`Infinity`。`undefined` 表示已声明但未赋值，`null` 是人为设定的空对象指针。ES6 新增 `Symbol` 用于生成唯一标识。

| 类型 | 说明 | 示例 |
|---|---|---|
| `Number` | 数字（不区分整数和浮点数，含 NaN、Infinity） | `let age = 18; let price = 9.9;` |
| `String` | 字符串（单引号、双引号、反引号模板字符串） | `let s1 = '你好';`（反引号模板字符串写法见下） |
| `Boolean` | 布尔值 | `let isAlive = true; let isDead = false;` |
| `undefined` | 已声明但未赋值 | `let a; console.log(a);  // undefined` |
| `null` | 空值（人为设定的空对象指针） | `let data = null;` |
| `Object` | 对象（键值对集合） | `let obj = { name: "向日葵", cost: 50 };` |
| `Array` | 数组（特殊的对象） | `let arr = [1, 2, "豌豆"];` |
| `Symbol` | 唯一标识符（ES6 新增） | `let s = Symbol("id");` |

字符串行里的 `` `我是${s1}` `` 是模板字符串（见「ES6+ 核心现代语法」）。

## 运算符

JS 的“宽松相等” `==` 会做隐式类型转换（`'1' == 1` 为 true），推荐一律使用不做转换的 `===`/`!==`。`&&`/`||`/`!` 是逻辑运算符，ES2020 还加入了空值合并 `??` 与可选链 `?.`。

| 符号 | 说明 | 示例 |
|---|---|---|
| `==` | 宽松相等（会隐式类型转换） | `'1' == 1`  // true |
| `===` | 严格相等（不转换类型，推荐使用） | `'1' === 1  // false` |
| `!=` | 不等于 | `1 != 2` |
| `!==` | 严格不等于 | `1 !== '1'` |
| `&&` | 逻辑与 (AND) | `true && false  // false` |
| \|\| | 逻辑或 (OR) | false \|\| true  // true |
| `!` | 逻辑非 (NOT) | `!true  // false` |
| `??` | 空值合并运算符（左侧为 null/undefined 才返回右侧） | `let a = null ?? "默认值"` |
| `?.` | 可选链操作符（找不到属性返回 undefined 而非报错） | `let name = obj?.user?.name;` |

## 条件判断与循环

条件判断支持 `if/else if/else`、`switch...case` 与三元运算符；循环除了经典 `for` 和 `while/do...while`，还有遍历对象键名的 `for...in` 与遍历可迭代对象值的 `for...of`。

| 语法 | 说明 | 示例 |
|---|---|---|
| `if...else if...else` | 基础条件判断 | `if (score > 90) { ... } else { ... }` |
| `switch...case` | 多值匹配 | `switch (day) { case 1: ... break; }` |
| `? :` | 三元运算符（简写 if） | `let res = age > 18 ? "成年" : "未成年";` |
| `for` | 经典 for 循环 | `for (let i = 0; i < 10; i++) { ... }` |
| `while` | 当型循环（先判断后执行） | `while (isAlive) { ... }` |
| `do...while` | 直到型循环（先执行一次后判断） | `do { ... } while (isAlive);` |
| `for...in` | 遍历对象的可枚举属性名（不推荐遍历数组） | `for (let key in obj) { ... }` |
| `for...of` | 遍历可迭代对象的值（数组、字符串等） | `for (let val of arr) { ... }` |

## 函数

函数可用 `function` 声明，也可用 ES6 箭头函数 `() =>`。箭头函数没有自己的 `this`（继承外层作用域），更适合回调场景。`...args` 剩余参数把传入的多个参数打包成数组。

| 语法 | 说明 | 示例 |
|---|---|---|
| `function` | 普通函数声明 | `function shoot() { console.log("射击"); }` |
| `return` | 函数返回值 | `function add(a, b) { return a + b; }` |
| `() =>` | 箭头函数（ES6，没有自己的 this） | `let add = (a, b) => a + b;` |
| `...args` | 剩余参数（将传参打包成数组） | `function fn(...args) { console.log(args); }` |

## 对象与数组常用方法

数组方法里，`map`/`filter` 返回**新数组**且不改变原数组，`forEach` 仅遍历；对象常用静态方法取键、取值与合并。

| 类型 | 方法 | 说明 | 示例 |
|---|---|---|---|
| Array | `.push()` | 末尾添加元素 | `arr.push("新元素");` |
| Array | `.pop()` | 删除末尾元素 | `arr.pop();` |
| Array | `.shift()` | 删除头部元素 | `arr.shift();` |
| Array | `.unshift()` | 头部添加元素 | `arr.unshift("第一");` |
| Array | `.map()` | 映射（返回新数组，不改变原数组） | `arr.map(x => x * 2)` |
| Array | `.filter()` | 过滤（返回符合条件的新数组） | `arr.filter(x => x > 5)` |
| Array | `.forEach()` | 遍历（无返回值，等同 for 循环） | `arr.forEach(x => console.log(x))` |
| Array | `.includes()` | 判断是否包含某元素 | `arr.includes("豌豆");` |
| Array | `.findIndex()` | 查找满足条件的第一个索引 | `arr.findIndex(x => x.id === 1)` |
| Object | `Object.keys()` | 获取对象所有键名组成的数组 | `Object.keys(obj)` |
| Object | `Object.values()` | 获取对象所有值组成的数组 | `Object.values(obj)` |
| Object | `Object.assign()` | 合并对象（浅拷贝） | `Object.assign({}, obj1, obj2)` |

## ES6+ 核心现代语法

模板字符串用反引号包裹、支持换行与 `${}` 插值；展开运算符 `...` 解包数组/对象；解构赋值从对象或数组中一次性取出多个值；`class`、模块、Promise、`async/await` 则分别解决了面向对象、代码组织与异步回调地狱问题。

| 语法 | 说明 | 示例 |
|---|---|---|
| 模板字符串 | 反引号包裹，支持换行和嵌入变量 | 见下方代码块 |
| `...` | 展开运算符（数组/对象解包） | `let arr2 = [...arr1, 4, 5];` |
| `{ a, b }` | 对象解构赋值 | `const { name, cost } = plant;` |
| `[ a, b ]` | 数组解构赋值 | `const [x, y, z] = [10, 20, 30];` |
| `class` | 类的定义（面向对象） | `class Plant { constructor(name) { this.name = name; } }` |
| `export` / `import` | 模块化导出与引入 | `export default App; import App from './App.js';` |
| `Promise` | 异步编程（解决回调地狱） | `new Promise((resolve, reject) => { ... })` |
| `async` / `await` | Promise 的语法糖（异步代码看起来像同步） | `async function getData() { let res = await fetch(url); }` |
| `try...catch` | 异常捕获（防代码崩溃） | `try { ... } catch (e) { console.log(e); }` |

```js
// 模板字符串：支持换行与 ${} 插值
let hp = 30, maxHp = 100;
let s = `血量：${hp} / ${maxHp}`;
console.log(s); // 血量：30 / 100

// 展开 + 解构
let arr1 = [1, 2, 3];
let arr2 = [...arr1, 4, 5];   // [1, 2, 3, 4, 5]
const { name, cost } = { name: "向日葵", cost: 50 };
```

# 位运算

位运算直接在二进制位层面操作整数，是底层编程、算法优化、网络协议与嵌入式开发的基础。这一节是 **Python 与 C++ 对照**版：两门语言的位运算符完全一致（`& | ~ ^ << >>`），但 Python 整数无限精度、C++ 有固定位数与溢出规则，差异会在各小节标注。来自两份位运算笔记的内容已合并去重。

## 基本概念与核心定位

| 概念 | 说明 |
|---|---|
| 位 | 计算机中最小的数据单位，值为 0 或 1 |
| 字节 | 由 8 个 bit 组成，是内存寻址的最小单位（1 Byte = 8 bits） |
| 字 | 由 2 个或 4 个字节组成（32 位系统 1 字 = 4 字节 = 32 bits；64 位系统 1 字 = 8 字节 = 64 bits） |
| 二进制表示 | `0b` 前缀（Python: `0b1010` = 10；C++: `0b1010` = 10） |
| 十六进制表示 | `0x` 前缀（Python: `0xFF` = 255；C++: `0xFF` = 255） |
| 八进制表示 | `0o` 前缀（Python: `0o77` = 63；C++ 传统写法 `077` = 63，C++14 起建议用 `0o77`） |
| 移位操作 | 将二进制位向左或向右移动，空位补 0（右移有符号数补符号位） |
| 掩码 | 用特定的二进制模式与目标值进行位运算，提取或修改特定位 |
| 原码/反码/补码 | 计算机存储有符号整数的方式：正数三码相同；负数反码 = 原码取反，补码 = 反码 + 1 |
| 溢出 | 运算结果超出数据类型能表示的范围（C++ 中无符号数溢出是定义良好的取模，有符号溢出是未定义行为） |
| 大端序(Big Endian) | 高位字节存储在低地址（网络字节序，人类可读顺序） |
| 小端序(Little Endian) | 低位字节存储在低地址（x86/x64 架构默认，有性能优势） |
| 位域(Bit Field) | C++ 中在结构体中以 bit 为单位定义成员，节省内存 |

```python
# 进制字面量：0b / 0o / 0x 前缀
a = 0b11111111   # 255
b = 0xFF         # 255
c = 0o77         # 63
```

## 进制转换（Python / C++ 对照）

| 场景 | Python | C++ | 说明 |
|---|---|---|---|
| 十进制转二进制字符串 | `bin(10)` -> `'0b1010'` | 无内置函数，需手写或用 bitset | — |
| 二进制字符串转十进制 | `int('1010', 2)` -> 10 | `std::stoi("1010", nullptr, 2)` 或 `std::bitset<8>("1010").to_ulong()` | — |
| 十进制转十六进制 | `hex(255)` -> `'0xff'` | `std::hex` + cout，或 `sprintf(buf, "%x", 255)` | — |
| 十六进制字符串转十进制 | `int('ff', 16)` -> 255 | `std::stoi("ff", nullptr, 16)` 或 `std::stoul("0xff", nullptr, 16)` | — |
| 十进制转八进制 | `oct(63)` -> `'0o77'` | `std::oct` + cout | — |
| 格式化输出 | `format(x, 'b')`、`format(x, 'x')`、`format(x, 'o')` | `std::hex << x`；`std::cout << std::bitset<8>(x)` | — |
| 任意进制转十进制 | `int(str, base)` | `std::stoi(str, nullptr, base)` | base 范围 2-36 |
| C++ 格式化（现代） | 无 | `std::format("{:b}", x)`、`std::format("{:#x}", x)`、`std::format("{:#o}", x)` | C++20 的 std::format |
| C++ 二进制输出 | — | `std::bitset<8>(x)` 或 `std::format("{:b}", x)` | 需 `#include <bitset>` 或 C++20 `<format>`；bitset 需指定位数 |

Python 里还有一些方便的进制输出写法：

```python
bin(42)          # '0b101010'   二进制
oct(42)          # '0o52'       八进制
hex(42)          # '0x2a'       十六进制
format(10, 'b')  # '1010'       格式化，不带前缀
f"{10:08b}"      # '00001010'   固定 8 位，前导补 0
bin(10)[2:]      # '1010'       用切片去掉 0b 前缀
```

## 按位与运算（AND &）

规则：**两个位都为 1 结果才为 1，否则为 0**。用途：把某位清零、检查某位是 0 还是 1、提取特定位。Python 与 C++ 都写作 `a & b`；C++ 要求操作数必须是整数类型（不能对浮点数做位运算），Python 整数任意精度、不会溢出。

| 场景 | 说明 |
|---|---|
| 清零某一位 | `a & ~(1 << 3)`：掩码 `~(1 << 3) = ...11110111`，与运算后第 3 位变 0，其他位不变 |
| 检查第 n 位是否为 1 | `(a & (1 << n)) != 0` 表示第 n 位为 1；Python 中可直接 `if a & (1 << n):` |
| 检查奇偶 | `a & 1`：最低位为 1 就是奇数，为 0 就是偶数（Python 写法 `if a & 1: 奇数 else: 偶数`） |
| 取低 4 位 | `a & 0x0F`：`0x0F = 0b00001111`，只保留低 4 位 |
| 判断两值是否相等/是否全 0 | `a ^ b == 0` 或 `!(a ^ b)` |

```python
# 清零第 3 位
a = a & ~(1 << 3)
# 检查第 n 位是否为 1
if a & (1 << n):
    ...
# 检查奇偶
if a & 1:
    print("奇数")
# 取低 4 位
b = a & 0x0F
```

## 按位或运算（OR |）

规则：**只要有一个位为 1，结果位就为 1**；只有参与位都为 0 时结果才为 0。可理解为“1 置 1、0 保留”，用途是把某位设置为 1、合并多个标志位。

| 场景 | 说明 |
|---|---|
| 置某位为 1 | a \| (1 << 3)：`1 << 3 = 0b00001000`，或运算后第 3 位变为 1，其余不变 |
| 合并权限标志 | READ \| WRITE \| EXECUTE，如 0b001 \| 0b010 \| 0b100 = 0b111 |
| 低 4 位全置 1 | a \| 0x0F |
| 末尾追加一个位 | (c << 1) \| 1 在 c 的末尾添加 1；(c << 1) \| 0 添加 0 |

```python
# 将第 3 位置为 1
a = a | (1 << 3)

# 合并权限标志
READ, WRITE, EXECUTE = 0b001, 0b010, 0b100
flags = READ | WRITE | EXECUTE   # 0b111

# 状态压缩：左移后拼接一位
c = 0b101
c = (c << 1) | 1   # 0b1011，末尾追加 1
c = (c << 1) | 0   # 0b10110，末尾追加 0
```

## 按位异或运算（XOR ^）

规则：**相同为 0，不同为 1**。两个重要性质：`a ^ a = 0`（自反性）、`a ^ 0 = a`（恒等性）。用途：翻转某一位、无临时变量交换两个变量、简单加解密、找出只出现一次的数。

| 场景 | 说明 |
|---|---|
| 翻转某一位 | `a ^ (1 << 3)`：第 3 位如果是 1 变 0、是 0 变 1 |
| 清 0 | 自己与自己异或：`a ^ a == 0` |
| 交换两变量（无临时变量） | `a ^= b; b ^= a; a ^= b`（原理 `a^(a^b)=b`，可读性差，不推荐实际使用） |
| 简单加密/解密 | `cipher = msg ^ key; plain = cipher ^ key`，异或同一个 key 两次还原原文（OTP 原理） |
| 找唯一出现一次的数 | 数组中所有数出现两次、只有一个出现一次时，全部异或即可：`reduce(xor, nums)` |

```python
# 翻转第 3 位
a = a ^ (1 << 3)

# 交换两个变量（不推荐，仅演示）
a ^= b
b ^= a
a ^= b

# 简单加密/解密
cipher = msg ^ key
plain = cipher ^ key   # 还原 msg
```

## 按位取反（NOT ~）

规则：**0 变 1、1 变 0**，所有位全部翻转。Python 与 C++ 在这里有一个关键差异：

| 语言 | 行为 | 示例 |
|---|---|---|
| Python | `~a = -a - 1`（整数无限精度，取反后加 1 再取负） | `~5 = -6`，`~(-5) = 4` |
| C++ | 按位取反，有符号数结果依赖补码表示 | 32 位 int 中 `~5 = -6`（`0b...11111010`）；`~0 = 0xFFFFFFFF = -1` |

用途：配合 AND 做掩码清零（`a & ~(1 << n)`）、求补码。C++ 中 `~0` 在 32 位下等于 `0xFFFFFFFF`（即 -1）。

## 左移运算（<<）

规则：所有位向左移动 n 位，右边空位补 0。左移 1 位相当于乘以 2。Python 整数无限精度，`5 << 100` 完全合法、不会溢出；C++ 中若移位数大于等于类型位宽，行为是未定义的（UB）。

| 用途 | 说明 | 示例 |
|---|---|---|
| 快速乘 2 的幂 | `a << n` 等价于 `a * 2**n`，通常比乘法快 | `5 << 3` = 40 |
| 构造单点掩码 | `1 << n` 构造只有第 n 位为 1 的掩码 | `1 << 3 = 0b00001000 = 8` |
| 构造全 1 掩码 | `(1 << n) - 1` 构造低 n 位全为 1 的掩码 | `(1 << 4) - 1 = 0b00001111 = 15` |
| 颜色分量合并 | 把 RGB 三个字节合并为一个整数 | 见下方代码块 |

```python
# RGB 三个字节合并为一个整数（常见于颜色存储）
color = (r << 16) | (g << 8) | b
```

## 右移运算（>>）

规则：所有位向右移动 n 位。Python 与 C++ 在负数/符号位处理上不同：

| 语言/情形 | 行为 | 示例 |
|---|---|---|
| Python | 算术右移（负数高位补 1），因整数无限精度 | `-5 >> 1 = -3` |
| C++ 无符号 | 逻辑右移（高位补 0） | `(unsigned)5 >> 1 = 2` |
| C++ 有符号 | 实现定义（通常是算术右移，高位补符号位） | `-5 >> 1 = -3`（通常） |

| 用途 | 说明 | 示例 |
|---|---|---|
| 快速除以 2 的幂 | `a >> n` 近似等价于整除 `2**n`（注意负数右移行为） | `a >> 3` ≈ `a / 8` |
| 提取字节 | 取出第 2 个字节 | `b = (a >> 8) & 0xFF` |
| 逐位处理 | 从最低位开始逐位取出处理 | 见下方代码块 |

```python
# 从最低位开始逐位处理
while a > 0:
    bit = a & 1
    a >>= 1
    # 处理 bit
```

## 复合赋值运算符

| 运算符 | Python | C++ | 等价于 |
|---|---|---|---|
| 按位与赋值 | `a &= b` | `a &= b` | `a = a & b` |
| 按位或赋值 | a \|= b | a \|= b | a = a \| b |
| 按位异或赋值 | `a ^= b` | `a ^= b` | `a = a ^ b` |
| 左移赋值 | `a <<= n` | `a <<= n` | `a = a << n` |
| 右移赋值 | `a >>= n` | `a >>= n` | `a = a >> n` |

## 位运算优先级

优先级从高到低为：取反 `~`（一元）→ 移位 `<<` `>>` → 按位与 `&` → 按位异或 `^` → 按位或 `|`。Python 与 C++ 的优先级一致。

| 优先级(高→低) | 运算符 | 说明 |
|---|---|---|
| 1 | `~` | 一元取反，优先级最高 |
| 2 | `<<` `>>` | 移位 |
| 3 | `&` | 按位与 |
| 4 | `^` | 按位异或 |
| 5 | \| | 按位或 |

最容易踩的坑是 `1 | 2 & 3`：按优先级它等价于 `1 | (2 & 3) = 1 | 2 = 3`，而不是很多人以为的 `(1 | 2) & 3 = 0`。建议：**复杂位运算表达式一律加括号**，例如写成 `(x & mask) != 0`。

```python
x = 1 | 2 & 3
# 实际是 1 | (2 & 3) = 3，不是 (1 | 2) & 3 = 0
```

## 字节序转换

网络传输与跨平台数据交换必须先统一字节序。**网络字节序 = 大端序**（高位在前）；x86/x64 主机默认小端（低位在前）。

Python 用 `int.from_bytes` / `int.to_bytes` 与 `struct` 模块控制字节序：

```python
# 字节串 -> 整数
int.from_bytes(b'\x01\x00', 'big')     # 256    大端：0x0100
int.from_bytes(b'\x00\x01', 'little')  # 256    小端：0x0100

# 整数 -> 字节串
(256).to_bytes(2, 'big')               # b'\x01\x00'
(256).to_bytes(2, 'little')            # b'\x00\x01'

# struct 打包/解包（> 大端，< 小端，I = uint32）
import struct
struct.pack('>I', 256)                 # b'\x00\x00\x01\x00'
struct.unpack('<I', b'\x00\x01\x00\x00')   # (256,)
```

C++ 手动拼拆字节（把 4 字节数组按大端/小端组合成整数，或反过来拆开）：

```cpp
unsigned char b[4];
unsigned int val;

// 大端组合（高位在前）
val = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
// 小端组合（低位在前）
val = b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);

// 整数拆成 2 字节（大端），val = 0x0100 -> b[0]=0x01, b[1]=0x00
b[0] = (val >> 8) & 0xFF;
b[1] = val & 0xFF;
```

C/C++ 还提供现成的字节序转换函数：

| 函数 | 说明 | 头文件 |
|---|---|---|
| `htonl(x)` / `ntohl(x)` / `htons(x)` / `ntohs(x)` | 主机序与网络序互转（网络序 = 大端） | `<arpa/inet.h>` |
| `htobe32(x)` / `be32toh(x)` / `htole32(x)` / `le32toh(x)` | 本机序与指定字节序互转 | `<endian.h>`（Linux） |

### Python struct 打包与解包

`struct` 模块把 Python 值按格式串打包成 C 风格字节，或反向解包。

| 函数 | 说明 | 示例 |
|---|---|---|
| `struct.pack(format, val)` | 将 Python 值按格式打包成 bytes | `struct.pack('>I', 256)` -> `b'\x00\x00\x01\x00'` |
| `struct.unpack(format, bytes)` | 将 bytes 按格式解包成 Python 值（返回元组） | `struct.unpack('<HH', b'\x01\x00\x02\x00')` -> `(1, 2)` |
| `struct.pack_into(fmt, buf, offset, val)` | 将值打包写入已有的 buffer | 操作 C 风格内存 buffer |
| `struct.unpack_from(fmt, buf, offset)` | 从 buffer 的偏移位置解包 | — |

字节序前缀与常用格式：

| 前缀/格式 | 含义 |
|---|---|
| `>` | 大端序 |
| `<` | 小端序 |
| `@` | 本机字节序（含对齐填充） |
| `=` | 本机字节序（无填充） |
| `!` | 网络字节序（大端） |
| `b` / `B` | signed / unsigned char（1 字节） |
| `h` / `H` | signed / unsigned short（2 字节） |
| `i` / `I` | signed / unsigned int（4 字节） |
| `q` / `Q` | signed / unsigned long long（8 字节） |
| `f` / `d` | float（4 字节）/ double（8 字节） |
| 数字前缀 | 重复次数，如 `3I` 表示 3 个 unsigned int |

## C++ 位域（Bit Field）

位域允许在结构体里**按 bit 定义成员**，适合寄存器配置、网络协议头等需要精确位布局的场景。注意位域只适用于整型，且不能取地址。

| 项 | 说明/示例 |
|---|---|
| 语法 | `struct StructName { type member1 : n; type member2 : m; };`，每个成员占 n、m 位 |
| 示例 | `struct Flags { unsigned int a : 1; unsigned int b : 3; unsigned int c : 4; };` 总共约 1 字节（8 位） |
| 用途 | 节省内存、精确控制位级数据表示（网络协议、寄存器配置、硬件编程） |
| 访问 | `flags.a = 1; flags.b = 5;` 像普通成员一样使用 |
| 注意 1 | 位域只能是整型（整型、枚举），不能是浮点型或指针 |
| 注意 2 | 位域不能取地址（不能 `&flags.a`），因为不一定对齐到字节边界 |
| 注意 3 | 跨平台布局可能不同（字节序、对齐）；网络协议慎用位域，推荐手写位操作 |

```cpp
struct Flags {
    unsigned int a : 1;   // 占 1 位
    unsigned int b : 3;   // 占 3 位
    unsigned int c : 4;   // 占 4 位
};                        // 加起来 8 位 ≈ 1 字节

Flags flags;
flags.a = 1;
flags.b = 5;
```

## C++ std::bitset（固定长度位集合）

`std::bitset<N>` 是编译期定长的位集合，下标 0 表示最低位（最右边）。它把“数组式位操作”封装成了安全的接口，还支持全部位运算符。

| 操作 | 说明 | 示例 |
|---|---|---|
| 头文件 | `#include <bitset>` | — |
| 创建 | 全 0 / 按二进制 / 按字符串初始化 | `std::bitset<8> b;`、`std::bitset<8> b(0b10101010);`、`std::bitset<8> b("10101010");` |
| 访问/修改 | 索引 0 是最低位（最右边） | `b[0] = 1; bool x = b[3];` |
| 设置全 1 | 所有位设为 1 | `b.set()` |
| 设置指定位 | 把第 3 位设为 1 | `b.set(3, 1)` |
| 重置全 0 | 所有位设为 0 | `b.reset()` |
| 翻转 | 翻转所有位或指定位 | `b.flip()` 或 `b.flip(3)` |
| 测试 | 第 3 位是否为 1（带范围检查） | `b.test(3)` |
| 计数 | 返回 1 的个数 | `b.count()` |
| 长度 | 返回总位数 | `b.size()` |
| 转字符串 | 得到 `"10101010"` | `b.to_string()` |
| 转整数 | 转 unsigned long / unsigned long long | `b.to_ulong()` / `b.to_ullong()` |
| 位运算 | 支持 & \| ^ ~ << >> | 见下方代码块 |
| 输入输出 | 直接输出二进制表示 | `std::cout << b;` |
| 限制 | 编译期确定大小，不能动态调整；需要动态位数用 `vector<bool>` 或 `boost::dynamic_bitset` | — |

```cpp
#include <bitset>
#include <iostream>

std::bitset<8> b1(0b10101010), b2(0b11001100);
auto r = (b1 & b2) | (b1 ^ b2);   // 支持 & | ^ ~ << >>
std::cout << r << std::endl;      // 直接输出二进制串
```

## 常用位运算技巧总结

下面这些技巧 Python 与 C++ 通用（Python 写法示意）：

| 技巧 | 表达式 | 说明 |
|---|---|---|
| 判断奇偶 | `n & 1` | `n & 1 == 1` 奇数，`== 0` 偶数 |
| 乘以 2 的幂 | `n << k` | `n << 3` = `n * 8` |
| 除以 2 的幂 | `n >> k` | `n >> 3` = `n / 8` |
| 判断 2 的幂 | `n > 0 and (n & (n - 1)) == 0` | 2 的幂只有一个 1 位 |
| 取最低位 1 | `n & (-n)` | 提取最右边的 1（`0b101100` -> `0b000100`） |
| 去掉最低位 1 | `n & (n - 1)` | 消除最右边的 1（`0b101100` -> `0b101000`） |
| 统计 1 的个数 | `while n: n &= n - 1; count += 1` | Brian Kernighan 算法 |
| 判断第 k 位 | `(n >> k) & 1` | 取出第 k 位 |
| 设置第 k 位为 1 | n \|= (1 << k) | 只改这一位，其他不变 |
| 设置第 k 位为 0 | `n &= ~(1 << k)` | 掩码除第 k 位外全为 1 |
| 翻转第 k 位 | `n ^= (1 << k)` | 0 变 1、1 变 0 |
| 取低 n 位 | `n & ((1 << n) - 1)` | `(1 << n) - 1` 是低 n 位全 1 的掩码 |
| 取模 2 的幂 | `n & (m - 1)`（当 `m = 2^k`） | 等价于 `n % m` 但更快 |
| 交替 01 掩码 | `0xAAAAAAAA`（偶数位 1）/ `0x55555555`（奇数位 1） | 32 位常量 |
| 下一个 2 的幂 | `power = 1; while power < n: power <<= 1` | 不小于 n 的最小 2 的幂 |
| 最高有效位位置 | `n.bit_length() - 1`（Python） | `31 - __builtin_clz(n)`（C++ GCC 内置） |
| 两数平均（无溢出） | `(x & y) + ((x ^ y) >> 1)` | 避免 `(x + y) / 2` 的溢出 |

Python 里“拼接二进制位”也常用移位 + 或的写法：`(c << 1) | 1` 在末尾追加 1、`(c << 1) | 0` 追加 0（见「按位或运算」一节）。

## 常见问题与排错（FAQ）

| 问题 | Python 表现 | C++ 表现 | 解决 |
|---|---|---|---|
| 整数溢出 | 不会溢出（无限精度） | 有符号溢出是 UB，无符号溢出是取模 | C++ 注意数据类型范围 |
| 负数右移 | 算术右移（高位补 1） | 实现定义（通常算术右移） | C++ 避免有符号数右移，先转 unsigned |
| 移位数超限 | `a << 100` 合法 | `a << 32`（int=32 位）是 UB | C++ 移位数应小于类型位数 |
| 浮点位运算 | 不支持（TypeError） | 不支持（编译错误） | 必须先转 int |
| 位运算优先级混淆 | 与 C++ 一致 | 与 Python 一致 | 一律加括号 |
| 字节序问题 | struct 可控字节序 | 手动处理或用 htonl/ntohl | 跨平台通信必须统一字节序 |
| 大数位运算 | 天然支持 | 用 `__int128` 或大数库 | C++ 可考虑 boost::multiprecision |
| 可读性差 | 与 C++ 一致 | 与 Python 一致 | 复杂位运算务必加注释说明掩码含义 |
