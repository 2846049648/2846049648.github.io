---
title: SQL 数据库语法
date: 2026-04-10
category: 学习记录
tags: ["sql", "数据库"]
excerpt: SQL 查询、增删改、聚合、连接与事务语法速查
---
# SQL 基础概念

SQL（Structured Query Language，结构化查询语言）是操作关系型数据库的标准语言。数据以“表”的形式组织，表与表之间可以通过外键建立关系。

## 基本概念

| 概念 | 说明 |
|---|---|
| SQL | Structured Query Language，结构化查询语言，用于操作关系型数据库的标准语言 |
| 数据库 | 按照数据结构来组织、存储和管理数据的仓库（如 MySQL、PostgreSQL、Oracle、SQL Server） |
| 关系型数据库 | RDBMS，数据以表格形式存储，表与表之间可以通过外键建立关系 |
| 表 | 由行和列组成的二维数据结构，类似 Excel 表格 |
| 行 | 表中的一条记录，代表一个实体 |
| 列 | 表中的一个字段，代表实体的某个属性 |
| 主键 | 唯一标识表中每一行记录的字段，不能为空且不能重复（如 id） |
| 外键 | 用于建立表与表之间关联的字段，引用另一张表的主键 |
| 索引 | 提高查询速度的数据结构（类似书本目录），但会降低插入/更新速度 |

## SQL 语言分类

| 分类 | 全称 | 说明 |
|---|---|---|
| DDL | Data Definition Language | 数据定义语言（建表、改表、删表） |
| DML | Data Manipulation Language | 数据操作语言（增、删、改） |
| DQL | Data Query Language | 数据查询语言（查） |
| DCL | Data Control Language | 数据控制语言（授权、回收权限） |
| TCL | Transaction Control Language | 事务控制语言（提交、回滚） |

# 数据库与表定义（DDL）

## 数据库操作

| 命令 | 说明 | 示例 |
|---|---|---|
| `CREATE DATABASE` | 创建数据库 | `CREATE DATABASE mydb;` |
| `CREATE DATABASE IF NOT EXISTS` | 不存在则创建（防报错） | `CREATE DATABASE IF NOT EXISTS mydb;` |
| `DROP DATABASE` | 删除数据库（危险！） | `DROP DATABASE mydb;` |
| `USE` | 切换/使用当前数据库 | `USE mydb;` |

## 表操作

| 命令 | 说明 | 示例 |
|---|---|---|
| `CREATE TABLE` | 创建表 | `CREATE TABLE users (id INT, name VARCHAR(50));` |
| `IF NOT EXISTS` | 表不存在时才创建 | `CREATE TABLE IF NOT EXISTS users (...);` |
| `DROP TABLE` | 删除表（连同数据一起删除） | `DROP TABLE users;` |
| `DROP TABLE IF EXISTS` | 安全删除 | `DROP TABLE IF EXISTS users;` |
| `TRUNCATE TABLE` | 清空表数据（比 DELETE 快，不可回滚） | `TRUNCATE TABLE users;` |
| `ALTER TABLE ADD` | 添加新列 | `ALTER TABLE users ADD age INT;` |
| `ALTER TABLE MODIFY` | 修改列的数据类型 | `ALTER TABLE users MODIFY age VARCHAR(3);` |
| `ALTER TABLE CHANGE` | 修改列名和数据类型（MySQL） | `ALTER TABLE users CHANGE age user_age INT;` |
| `ALTER TABLE DROP COLUMN` | 删除列 | `ALTER TABLE users DROP COLUMN age;` |
| `RENAME TABLE` | 重命名表 | `RENAME TABLE old_name TO new_name;` |
| `DESC` / `DESCRIBE` | 查看表结构 | `DESC users;` |
| `SHOW TABLES` | 查看当前数据库所有表 | `SHOW TABLES;` |

## 数据类型

| 分类 | 类型 | 说明 | 示例 |
|---|---|---|---|
| 整数型 | `INT` | 标准整数（4 字节） | `age INT` |
| 整数型 | `TINYINT` | 小整数（1 字节，常用于状态 0/1） | `is_deleted TINYINT` |
| 整数型 | `BIGINT` | 大整数（8 字节） | `id BIGINT` |
| 小数型 | `DECIMAL(M,D)` | 精确小数，M 为总位数，D 为小数位（财务必用） | `price DECIMAL(10,2)` |
| 小数型 | `FLOAT` | 单精度浮点数（有精度丢失风险） | `score FLOAT` |
| 字符串 | `VARCHAR(N)` | 变长字符串（常用，按实际长度存储） | `name VARCHAR(50)` |
| 字符串 | `CHAR(N)` | 定长字符串（不足补空格） | `code CHAR(10)` |
| 大文本 | `TEXT` | 长文本数据 | `content TEXT` |
| 日期时间 | `DATETIME` | 日期和时间（YYYY-MM-DD HH:MM:SS） | `created_at DATETIME` |
| 日期时间 | `DATE` | 仅日期 | 记录生日：`DATE` |
| 日期时间 | `TIMESTAMP` | 时间戳（自动记录插入/更新时间） | `updated_at TIMESTAMP` |
| 布尔型 | `BOOLEAN` / `TINYINT(1)` | MySQL 无真正布尔，用 1 和 0 表示 | `is_active BOOLEAN` |

## 约束

| 约束 | 说明 | 示例 |
|---|---|---|
| `PRIMARY KEY` | 主键约束（唯一且非空） | `id INT PRIMARY KEY` |
| `AUTO_INCREMENT` | 主键自增（MySQL 特有） | `id INT PRIMARY KEY AUTO_INCREMENT` |
| `NOT NULL` | 非空约束 | `name VARCHAR(50) NOT NULL` |
| `UNIQUE` | 唯一约束（不能重复，但可以为空） | `email VARCHAR(100) UNIQUE` |
| `DEFAULT` | 默认值约束 | `status INT DEFAULT 1` |
| `CHECK` | 检查约束（条件必须为真） | `age INT CHECK (age >= 18)` |
| `FOREIGN KEY` | 外键约束（关联其他表主键） | `dept_id INT FOREIGN KEY REFERENCES dept(id)` |
| 组合主键 | 多个字段共同作为主键 | `PRIMARY KEY (class_id, student_id)` |

把上面这些数据类型和约束组合起来，一个典型的建表语句大致长这样：

```sql
CREATE TABLE users (
  id INT PRIMARY KEY AUTO_INCREMENT,
  name VARCHAR(50) NOT NULL,
  email VARCHAR(100) UNIQUE,
  status INT DEFAULT 1
);
```

# 数据修改（DML）

## 插入、更新与删除

| 操作 | 语法 | 示例 |
|---|---|---|
| 插入完整行 | `INSERT INTO 表 VALUES (值1, 值2...)` | `INSERT INTO users VALUES (1, 'Tom', 20);` |
| 插入指定列 | `INSERT INTO 表 (列1, 列2) VALUES (值1, 值2)` | `INSERT INTO users (name, age) VALUES ('Tom', 20);` |
| 批量插入 | `INSERT INTO 表 VALUES (...), (...), (...)` | `INSERT INTO users VALUES (1,'A',18), (2,'B',20);` |
| UPDATE 基础 | `UPDATE 表 SET 列=新值 WHERE 条件` | `UPDATE users SET age=21 WHERE id=1;` |
| UPDATE 多列 | `UPDATE 表 SET 列1=值1, 列2=值2 WHERE 条件` | `UPDATE users SET age=21, name='Jerry' WHERE id=1;` |
| DELETE 基础 | `DELETE FROM 表 WHERE 条件` | `DELETE FROM users WHERE id=1;` |
| DELETE 清空表 | `DELETE FROM 表`（逐行删除，可回滚） | `DELETE FROM users;` |

注意：UPDATE/DELETE 忘记加 WHERE 会修改/删除全表数据，执行前务必检查 WHERE 条件。
# 数据查询（DQL）

查询是 SQL 中使用最频繁的操作，核心是 `SELECT`，再配合各种子句过滤、排序与分页。

## SELECT 基本查询

| 语法要素 | 说明 | 示例 |
|---|---|---|
| `SELECT *` | 查询所有列 | `SELECT * FROM users;` |
| `SELECT 列` | 查询指定列 | `SELECT name, age FROM users;` |
| `AS` 别名 | 给列或表起别名（AS 可省略） | `SELECT name AS '姓名' FROM users u;` |
| `DISTINCT` | 去重查询 | `SELECT DISTINCT department FROM employees;` |
| `LIMIT` | 限制返回行数 | `SELECT * FROM users LIMIT 10;` |
| `LIMIT 偏移量` | 分页查询（跳过前 5 条，取 10 条） | `SELECT * FROM users LIMIT 5, 10;` 或 `LIMIT 10 OFFSET 5;` |
| `WHERE` 条件 | 过滤查询（不支持别名） | `SELECT * FROM users WHERE age > 18;` |

## WHERE 条件过滤

| 运算符 | 说明 | 示例 |
|---|---|---|
| `=` | 等于 | `WHERE age = 20` |
| `<>` 或 `!=` | 不等于 | `WHERE status <> 0` |
| `>` / `<` / `>=` / `<=` | 大于 / 小于 / 大于等于 / 小于等于 | `WHERE price >= 100` |
| `BETWEEN A AND B` | 在 A 和 B 之间（包含边界） | `WHERE age BETWEEN 18 AND 30` |
| `IN (值1, 值2)` | 在指定集合中 | `WHERE city IN ('北京', '上海')` |
| `NOT IN` | 不在集合中 | `WHERE city NOT IN ('北京')` |
| `LIKE '模式'` | 模糊匹配 | `WHERE name LIKE '张%'` |
| `IS NULL` | 为空（不能用 `= NULL`） | `WHERE email IS NULL` |
| `IS NOT NULL` | 不为空 | `WHERE email IS NOT NULL` |
| `AND` | 且（多个条件同时满足） | `WHERE age > 18 AND city = '北京'` |
| `OR` | 或（满足其中一个条件） | `WHERE age < 18 OR age > 60` |
| `NOT` | 非 | `WHERE status NOT IN (0, -1)` |

## LIKE 模糊匹配通配符

| 通配符 | 说明 | 示例 |
|---|---|---|
| `%` | 匹配任意数量的字符（包括 0 个） | `WHERE name LIKE '张%'`（以张开头） |
| `_` | 匹配单个任意字符 | `WHERE name LIKE '_明'`（两个字，第二个字是明） |
| 结合使用 | 把 `%` 和 `_` 组合起来匹配 | `WHERE code LIKE 'G_%'`（G 开头，至少两个字符） |
| 注意 | 模糊查询不能使用普通索引（会导致全表扫描） | 数据量大时慎用 `LIKE '%...'`，考虑全文索引或 ES |

# 排序、分组与聚合

## 排序 ORDER BY

| 关键字 | 说明 | 示例 |
|---|---|---|
| `ORDER BY 列 ASC` | 升序排列（默认） | `SELECT * FROM users ORDER BY age ASC;` |
| `ORDER BY 列 DESC` | 降序排列 | `SELECT * FROM users ORDER BY created_at DESC;` |
| 多列排序 | 先按第一列排，相同再按第二列排 | `ORDER BY dept_id ASC, age DESC;` |

## 分组 GROUP BY 与 HAVING

`GROUP BY` 把行按列分组（常配合聚合函数），`HAVING` 过滤分组后的结果，不能用 WHERE 代替。

| 关键字 | 说明 | 示例 |
|---|---|---|
| `GROUP BY 列` | 按某列分组（常配合聚合函数） | `SELECT dept_id, COUNT(*) FROM users GROUP BY dept_id;` |
| 多列分组 | 按多个列组合分组 | `GROUP BY dept_id, role;` |
| `HAVING` | 过滤分组后的结果（不能用 WHERE） | `SELECT dept_id, COUNT(*) AS cnt FROM users GROUP BY dept_id HAVING cnt > 5;` |
| `WHERE` vs `HAVING` | WHERE 在分组前过滤行，HAVING 在分组后过滤组 | `WHERE age > 18 GROUP BY ... HAVING COUNT(*) > 3;` |

## 聚合函数

| 函数 | 说明 | 示例 |
|---|---|---|
| `COUNT(*)` | 统计行数（包括 NULL） | `SELECT COUNT(*) FROM users;` |
| `COUNT(列)` | 统计该列非 NULL 的行数 | `SELECT COUNT(email) FROM users;` |
| `SUM(列)` | 求和 | `SELECT SUM(price) FROM orders;` |
| `AVG(列)` | 求平均值 | `SELECT AVG(score) FROM students;` |
| `MAX(列)` | 求最大值 | `SELECT MAX(created_at) FROM logs;` |
| `MIN(列)` | 求最小值 | `SELECT MIN(price) FROM products;` |
| 结合 GROUP BY | 聚合函数通常与 GROUP BY 连用 | `SELECT dept_id, AVG(salary) FROM employees GROUP BY dept_id;` |
# 多表连接（JOIN）

当数据分散在多张表时，用 JOIN 把它们按关联字段连接起来查询。

## 连接类型与连接条件

| 类型 | 说明 | 图解 | 示例 |
|---|---|---|---|
| `INNER JOIN`（内连接） | 只返回两表中能匹配上的行 | 交集 | `SELECT * FROM a INNER JOIN b ON a.id = b.aid;` |
| `LEFT JOIN`（左连接） | 返回左表所有行，右表无匹配则为 NULL | 左表全集 + 交集 | `SELECT * FROM a LEFT JOIN b ON a.id = b.aid;` |
| `RIGHT JOIN`（右连接） | 返回右表所有行，左表无匹配则为 NULL | 右表全集 + 交集 | `SELECT * FROM a RIGHT JOIN b ON a.id = b.aid;` |
| `FULL JOIN`（全连接） | 返回两表所有行，无匹配处填 NULL | 并集（MySQL 不支持，用 UNION 代替） | `SELECT * FROM a FULL OUTER JOIN b ON a.id = b.aid;` |
| `CROSS JOIN`（交叉连接） | 笛卡尔积，左表每行与右表每行组合 | - | `SELECT * FROM a CROSS JOIN b;` |
| 自连接 | 表与自己连接（如查询员工及其经理） | - | `SELECT e.name, m.name FROM emp e LEFT JOIN emp m ON e.manager_id = m.id;` |
| `ON` | 连接条件（连接两张表的关联字段） | - | `ON a.id = b.aid` |
| `USING` | 如果两表连接字段名相同，可用 USING 简化 | - | `LEFT JOIN b USING (id);` |

# 子查询与联合查询

## 子查询

把一个 SELECT 的结果嵌进另一个查询里，就叫子查询。按返回结果可分为标量子查询（单个值）与列子查询（一列多行）。

| 类型 | 说明 | 示例 |
|---|---|---|
| WHERE 子查询 | 将查询结果作为 WHERE 的条件值 | `SELECT * FROM users WHERE age > (SELECT AVG(age) FROM users);` |
| IN 子查询 | 将查询结果集作为 IN 的集合 | `SELECT * FROM users WHERE dept_id IN (SELECT id FROM dept WHERE name='IT');` |
| EXISTS 子查询 | 判断子查询是否返回行（有返回则真） | `SELECT * FROM dept d WHERE EXISTS (SELECT 1 FROM users u WHERE u.dept_id = d.id);` |
| FROM 子查询 | 将查询结果当作一张临时表使用 | `SELECT t.avg_age FROM (SELECT AVG(age) AS avg_age FROM users) t;` |
| 标量子查询 | 子查询返回单个值（一行一列） | 常用于 `=`、`>`、`<` 比较后面 |
| 列子查询 | 子查询返回一列（多行一列） | 常用于 `IN`、`NOT IN` 后面 |

## UNION 联合查询

`UNION` 用来纵向合并多个 SELECT 的结果。

| 关键字 | 说明 | 示例 |
|---|---|---|
| `UNION` | 合并多个 SELECT 结果，并去除重复行 | `SELECT name FROM table1 UNION SELECT name FROM table2;` |
| `UNION ALL` | 合并结果，不去除重复行（性能更好） | `SELECT name FROM table1 UNION ALL SELECT name FROM table2;` |

使用条件：1. 列数必须相同；2. 对应列的数据类型必须兼容；3. 默认按第一列排序。

# 常用内置函数与窗口函数

## 常用内置函数

| 分类 | 函数 | 说明 | 示例 |
|---|---|---|---|
| 字符串 | `CONCAT(s1,s2)` | 拼接字符串 | `SELECT CONCAT(first_name, ' ', last_name);` |
| 字符串 | `LENGTH(s)` | 获取字符串长度 | `SELECT LENGTH(name);` |
| 字符串 | `UPPER(s)` / `LOWER(s)` | 转大写 / 转小写 | `SELECT UPPER(name);` |
| 字符串 | `SUBSTRING(s, start, len)` | 截取字符串（从 1 开始） | `SELECT SUBSTRING(phone, 8, 4);` |
| 字符串 | `TRIM(s)` | 去除首尾空格 | `SELECT TRIM(name);` |
| 字符串 | `REPLACE(s, old, new)` | 替换字符串 | `SELECT REPLACE(content, 'bad', 'good');` |
| 数学 | `ROUND(num, digits)` | 四舍五入 | `SELECT ROUND(price, 2);` |
| 数学 | `CEIL(num)` / `FLOOR(num)` | 向上取整 / 向下取整 | `SELECT CEIL(4.1);` |
| 数学 | `ABS(num)` | 取绝对值 | `SELECT ABS(-5);` |
| 日期 | `NOW()` | 获取当前日期和时间 | `SELECT NOW();` |
| 日期 | `CURDATE()` | 获取当前日期 | `SELECT CURDATE();` |
| 日期 | `DATE_FORMAT(date, format)` | 格式化日期 | `SELECT DATE_FORMAT(created_at, '%Y-%m-%d');` |
| 日期 | `DATEDIFF(d1, d2)` | 计算两个日期相差的天数 | `SELECT DATEDIFF('2024-01-01', '2023-01-01');` |
| 流程控制 | `IF(expr, true_val, false_val)` | expr 为真返回 true_val，否则 false_val | `SELECT IF(score>=60, '及格', '不及格');` |
| 流程控制 | `CASE WHEN THEN ELSE END` | 多条件判断 | `SELECT CASE WHEN age<18 THEN '未成年' WHEN age<60 THEN '成年' ELSE '老年' END;` |

## 窗口函数

窗口函数（Window Functions）在 MySQL 8.0+ 提供。语法结构：`函数名() OVER (PARTITION BY ... ORDER BY ...)`。

| 函数/关键字 | 说明 | 示例 |
|---|---|---|
| `ROW_NUMBER()` | 为每行分配唯一的连续序号（1,2,3...） | `ROW_NUMBER() OVER (ORDER BY score DESC)` |
| `RANK()` | 排名（相同值同名次，后续名次跳跃：1,2,2,4） | `RANK() OVER (ORDER BY score DESC)` |
| `DENSE_RANK()` | 密集排名（相同值同名次，后续名次不跳跃：1,2,2,3） | `DENSE_RANK() OVER (ORDER BY score DESC)` |
| `PARTITION BY` | 窗口分组（类似 GROUP BY，但不压缩行） | `ROW_NUMBER() OVER (PARTITION BY dept_id ORDER BY salary DESC)` |
| 聚合窗口函数 | 在每行后面显示聚合值，不压缩行 | `SUM(score) OVER (PARTITION BY class_id)` |
| `LAG(列, N)` | 获取当前行前第 N 行的数据 | `LAG(price, 1) OVER (ORDER BY date)`（对比昨天价格） |
| `LEAD(列, N)` | 获取当前行后第 N 行的数据 | `LEAD(price, 1) OVER (ORDER BY date)`（对比明天价格） |

经典场景——分组 TopN（每个部门工资前 3 名）：

```sql
SELECT * FROM (
  SELECT *, ROW_NUMBER() OVER(PARTITION BY dept ORDER BY sal DESC) rn FROM emp
) t WHERE rn <= 3;
```
# 视图与索引

## 视图

视图是保存了查询逻辑的“虚拟表”，本身不存数据。

| 操作 | 语法 | 示例 |
|---|---|---|
| `CREATE VIEW` | 创建视图（虚拟表） | `CREATE VIEW v_user AS SELECT id, name FROM users WHERE status=1;` |
| `ALTER VIEW` | 修改视图 | `ALTER VIEW v_user AS SELECT id, name, age FROM users;` |
| `DROP VIEW` | 删除视图 | `DROP VIEW v_user;` |

作用：1. 简化复杂 SQL；2. 数据安全（隐藏敏感列）；3. 逻辑复用。注意：视图不存储数据（除物化视图外），只是保存了查询逻辑。

## 索引

索引用于加快查询，但会降低插入/更新速度。

| 操作 | 语法 | 示例 |
|---|---|---|
| `CREATE INDEX` | 创建普通索引 | `CREATE INDEX idx_name ON users(name);` |
| `CREATE UNIQUE INDEX` | 创建唯一索引 | `CREATE UNIQUE INDEX idx_email ON users(email);` |
| `CREATE INDEX` 多列 | 创建组合索引（最左前缀原则） | `CREATE INDEX idx_dept_age ON users(dept_id, age);` |
| `DROP INDEX`（MySQL） | 删除索引 | `ALTER TABLE users DROP INDEX idx_name;` |
| `DROP INDEX` | 删除索引 | `DROP INDEX idx_name ON users;` |
| `SHOW INDEX` | 查看表索引 | `SHOW INDEX FROM users;` |
| `EXPLAIN` | 分析 SQL 执行计划（看是否走索引） | `EXPLAIN SELECT * FROM users WHERE name='Tom';` |

最左前缀原则：组合索引 `(a, b, c)` 支持查 `(a)`、`(a,b)`、`(a,b,c)`，不支持跳过前面的列直接查。

索引失效的常见场景：1. 在索引列上运算；2. 使用函数；3. 左边模糊 `LIKE '%x'`；4. 隐式类型转换；5. `OR` 条件中有一方无索引。例如 `WHERE YEAR(created_at)=2024` 会失效。

# 事务

## 事务基础与 ACID

事务是包含多个步骤的逻辑操作单元，要么全成功、要么全失败。例如银行转账：A 扣钱和 B 加钱必须同时成功。

| 概念 | 说明 | 示例 |
|---|---|---|
| 事务 | 包含多个步骤的逻辑操作单元，要么全成功，要么全失败 | 银行转账：A 扣钱和 B 加钱必须同时成功 |
| `START TRANSACTION` / `BEGIN` | 开启事务 | `START TRANSACTION;` |
| `COMMIT` | 提交事务（所有修改永久生效） | `COMMIT;` |
| `ROLLBACK` | 回滚事务（撤销所有未提交的修改） | `ROLLBACK;` |
| Atomicity（原子性） | 不可分割 | 要么全做要么全不做 |
| Consistency（一致性） | 状态转换一致 | 转账前后总金额不变 |
| Isolation（隔离性） | 事务间互不干扰 | 并发事务互相看不到中间状态 |
| Durability（持久性） | 永久保存 | 提交后数据掉电也不丢 |

## 并发问题与隔离级别

| 概念 | 说明 | 解决办法 |
|---|---|---|
| 脏读 | 读到了其他事务未提交的数据 | 隔离级别提升到 `READ COMMITTED` 解决 |
| 不可重复读 | 同一事务内两次读取同一数据结果不同（被别人改了） | `REPEATABLE READ` 解决 |
| 幻读 | 同一事务内两次查询记录数不同（被别人插入了） | `SERIALIZABLE` 解决（MySQL 的 RR 级别通过 MVCC + 间隙锁部分解决） |
| 设置隔离级别 | 修改当前会话的隔离级别 | `SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED;` |

# 权限管理（DCL）

DCL 负责创建用户、授权与回收权限。

| 命令 | 说明 | 示例 |
|---|---|---|
| `CREATE USER` | 创建用户 | `CREATE USER 'testuser'@'localhost' IDENTIFIED BY 'password';` |
| `GRANT` | 授予权限 | `GRANT SELECT, INSERT ON mydb.* TO 'testuser'@'localhost';` |
| `ALL PRIVILEGES` | 授予所有权限 | `GRANT ALL PRIVILEGES ON *.* TO 'root'@'%' WITH GRANT OPTION;` |
| `REVOKE` | 撤销权限 | `REVOKE INSERT ON mydb.* FROM 'testuser'@'localhost';` |
| `SHOW GRANTS` | 查看用户权限 | `SHOW GRANTS FOR 'testuser'@'localhost';` |
| `DROP USER` | 删除用户 | `DROP USER 'testuser'@'localhost';` |
| `FLUSH PRIVILEGES` | 刷新权限（使权限立即生效） | `FLUSH PRIVILEGES;` |
| `%` 通配符 | 表示允许任意 IP 连接 | `'testuser'@'%'`（允许远程连接） |

# 经典查询场景与 SQL 编写顺序

## SQL 逻辑编写顺序与实际执行顺序

逻辑编写顺序：`SELECT` -> `FROM` -> `JOIN` -> `ON` -> `WHERE` -> `GROUP BY` -> `HAVING` -> `ORDER BY` -> `LIMIT`。口诀：先想查什么，再想从哪查，再过滤，再排序。

实际执行顺序：`FROM` -> `ON` -> `JOIN` -> `WHERE` -> `GROUP BY` -> `HAVING` -> `SELECT` -> `DISTINCT` -> `ORDER BY` -> `LIMIT`。口诀：先找表，再连接，再过滤，最后投影列。

## 经典查询场景

| 场景 | 实现方式 | 备注 |
|---|---|---|
| 分组 TopN | 窗口函数 `ROW_NUMBER()` 配合 `PARTITION BY` | 见窗口函数章节 |
| 树形结构查询 | 自连接（JOIN 自身）或递归 CTE（`WITH RECURSIVE`） | - |
| 留存率计算 | 利用自连接或条件聚合 | `ON t1.user_id = t2.user_id AND DATEDIFF(t2.date, t1.date)=1` |
| 连续登录天数 | `RANK()` 或 `ROW_NUMBER()` 减去日期差值分组 | - |
