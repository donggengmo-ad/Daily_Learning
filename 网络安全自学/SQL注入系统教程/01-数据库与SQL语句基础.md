# 第一章：数据库与常用 SQL 语句详解

## 1. 数据库、DBMS 和 SQL 分别是什么

数据库是有组织的数据集合；数据库管理系统（DBMS）是负责保存、查询、并发控制、权限和恢复的软件；SQL 是应用与关系型 DBMS 交互的主要语言。

例如 MySQL、PostgreSQL、SQL Server、Oracle 和 SQLite 都是 DBMS。它们都支持 SQL，但函数名、分页语法、错误消息、注释规则和权限模型并不完全相同。这些差异既影响正常开发，也影响 SQL 注入的判断方法。

关系型数据库的几个核心对象：

- **表（table）**：同类数据的集合；
- **行（row）**：一条记录；
- **列（column）**：记录中的一个属性；
- **主键（primary key）**：唯一标识一行；
- **外键（foreign key）**：表达表之间的引用关系；
- **约束（constraint）**：限制可以写入的数据；
- **索引（index）**：用额外存储换取查询速度；
- **视图（view）**：保存的查询接口，通常不独立保存数据；
- **事务（transaction）**：一组要么全部成功、要么全部失败的操作。

## 2. SQL 语句的类别

SQL 常被分为以下几类：

| 类别 | 含义 | 常见语句 |
|---|---|---|
| DDL | 定义数据库结构 | `CREATE`、`ALTER`、`DROP`、`TRUNCATE` |
| DML | 修改表中数据 | `INSERT`、`UPDATE`、`DELETE` |
| DQL | 查询数据 | `SELECT` |
| DCL | 控制权限 | `GRANT`、`REVOKE` |
| TCL | 控制事务 | `BEGIN`、`COMMIT`、`ROLLBACK`、`SAVEPOINT` |

分类不是所有产品都严格采用，但有助于理解风险：一个只读查询中的注入，在高权限且允许堆叠语句的连接上，可能越过 DQL 边界；因此数据库权限和驱动配置同样重要。

## 3. 建立练习表

以下结构接近 PostgreSQL/MySQL 的通用写法。SQLite 不支持所有 `ALTER` 和数据类型细节，但足以完成大多数查询练习。

```sql
CREATE TABLE users (
    id INTEGER PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    role VARCHAR(20) NOT NULL DEFAULT 'user',
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CHECK (role IN ('user', 'admin'))
);

CREATE TABLE products (
    id INTEGER PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    category VARCHAR(50) NOT NULL,
    price DECIMAL(10, 2) NOT NULL,
    stock INTEGER NOT NULL DEFAULT 0,
    training_flag VARCHAR(80),
    CHECK (price >= 0),
    CHECK (stock >= 0)
);

CREATE TABLE orders (
    id INTEGER PRIMARY KEY,
    user_id INTEGER NOT NULL,
    status VARCHAR(20) NOT NULL DEFAULT 'pending',
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE TABLE order_items (
    order_id INTEGER NOT NULL,
    product_id INTEGER NOT NULL,
    quantity INTEGER NOT NULL,
    unit_price DECIMAL(10, 2) NOT NULL,
    PRIMARY KEY (order_id, product_id),
    FOREIGN KEY (order_id) REFERENCES orders(id),
    FOREIGN KEY (product_id) REFERENCES products(id),
    CHECK (quantity > 0)
);
```

### 3.1 常用数据类型

| 类型 | 用途 | 注意点 |
|---|---|---|
| `INTEGER`、`BIGINT` | 整数、编号、计数 | 范围由产品决定；不要用字符串保存数值 |
| `DECIMAL(p,s)` | 金额等精确小数 | 比浮点类型更适合金额 |
| `FLOAT`、`REAL` | 科学计算近似值 | 存在二进制浮点误差 |
| `CHAR(n)` | 固定长度文本 | 可能补空格，普通文本多用 `VARCHAR` |
| `VARCHAR(n)`、`TEXT` | 可变长度文本 | 长度与编码规则依数据库而异 |
| `DATE`、`TIME`、`TIMESTAMP` | 日期和时间 | 明确时区；服务端与数据库应统一策略 |
| `BOOLEAN` | 真/假 | MySQL 常映射到小整数，SQLite 使用动态类型体系 |
| `JSON`、`JSONB` | 结构化文档 | 仍需参数绑定；JSON 内的动态路径也要审计 |
| `BLOB`、`BYTEA` | 二进制数据 | 通常通过驱动参数传递，不要拼接十六进制文本 |

类型转换可能是漏洞判断中的信号。例如把字符串输入放入数字比较时，数据库可能报错、隐式转换或直接拒绝；不同结果不代表一定存在注入，只说明需要查看最终 SQL 与驱动行为。

## 4. `CREATE`：创建数据库对象

`CREATE DATABASE` 创建数据库，`CREATE SCHEMA` 创建命名空间，`CREATE TABLE` 创建表，`CREATE INDEX` 创建索引，`CREATE VIEW` 创建视图。

```sql
CREATE DATABASE shop_lab;

CREATE INDEX idx_orders_user_id ON orders(user_id);

CREATE VIEW active_products AS
SELECT id, name, price
FROM products
WHERE stock > 0;
```

注意事项：

- `CREATE DATABASE` 通常需要较高权限，SQLite 没有该语句；
- 索引适合经常出现在 `WHERE`、`JOIN`、`ORDER BY` 中的列，但会增加写入成本；
- 视图不自动修复底层动态 SQL；应用查询视图时仍应绑定参数；
- 生产应用账号通常不应拥有随意创建或删除数据库对象的权限。

## 5. `ALTER`：修改表结构

```sql
ALTER TABLE products ADD COLUMN description TEXT;
ALTER TABLE products ALTER COLUMN name TYPE VARCHAR(200);
ALTER TABLE products DROP COLUMN description;
```

不同 DBMS 的 `ALTER TABLE` 差异较大。例如 MySQL 修改列常用 `MODIFY COLUMN`，SQLite 只支持一部分结构变更。执行结构变更前应考虑锁表、迁移时间、回滚方案和旧版本应用的兼容性。

应用一般不应该根据 HTTP 参数动态拼接 `ALTER` 语句。表名、列名等标识符通常不能通过普通值占位符绑定，确需动态选择时必须使用固定映射。

## 6. `DROP` 与 `TRUNCATE`：删除对象或清空表

```sql
DROP VIEW active_products;
DROP TABLE order_items;
TRUNCATE TABLE audit_events;
```

- `DROP` 删除对象本身；
- `TRUNCATE` 保留表结构但快速清空全部行；
- `DELETE FROM table` 也能删除全部行，但逐行日志、触发器和事务行为可能不同；
- 这些属于破坏性操作，教程的注入实验不使用它们；
- Web 应用数据库账号不应拥有不必要的 `DROP`、`ALTER` 或 `TRUNCATE` 权限。

## 7. `INSERT`：新增数据

一次插入一行：

```sql
INSERT INTO users (id, username, password_hash, role)
VALUES (1, 'alice', 'argon2-example-hash', 'admin');
```

一次插入多行：

```sql
INSERT INTO products (id, name, category, price, stock, training_flag)
VALUES
    (1, 'SQL 入门书', 'book', 59.90, 10, 'LAB{book-row}'),
    (2, '机械键盘', 'device', 399.00, 5, 'LAB{keyboard-row}'),
    (3, '显示器支架', 'device', 199.00, 0, 'LAB{stand-row}');
```

从查询结果插入：

```sql
INSERT INTO archived_orders (id, user_id, status, created_at)
SELECT id, user_id, status, created_at
FROM orders
WHERE status = 'closed';
```

重要规则：

- 显式写列名，避免表结构变化导致值错位；
- 字符串、日期和二进制内容通过驱动参数传递；
- 不要用 SQL 字符串拼接批量导入文件；
- PostgreSQL 的 `RETURNING`、MySQL 的自增 ID API 可取得新记录 ID，具体写法依产品而异。

## 8. `SELECT`：查询数据

### 8.1 基本结构

```sql
SELECT id, name, price
FROM products
WHERE category = 'device'
ORDER BY price DESC
LIMIT 10 OFFSET 0;
```

书写顺序与逻辑处理顺序不同。便于理解的逻辑顺序是：

```text
FROM/JOIN -> WHERE -> GROUP BY -> HAVING -> SELECT -> DISTINCT
-> ORDER BY -> LIMIT/OFFSET
```

这解释了为什么同一层级的 `WHERE` 通常不能直接引用刚在 `SELECT` 中定义的别名，而 `ORDER BY` 往往可以。

### 8.2 选择列和别名

```sql
SELECT
    name AS product_name,
    price,
    price * 0.9 AS discounted_price
FROM products;
```

`SELECT *` 适合临时探索，不适合作为长期接口：它增加无关数据传输、容易暴露新加入的敏感列，也使应用依赖列顺序。

### 8.3 `DISTINCT` 去重

```sql
SELECT DISTINCT category
FROM products;
```

`DISTINCT` 对所选列的组合去重。它不是修复错误 JOIN 的工具；如果连接产生了意外重复，应先检查关系和连接条件。

### 8.4 `WHERE` 条件

```sql
SELECT id, name
FROM products
WHERE category = 'device'
  AND price BETWEEN 100 AND 500
  AND stock > 0;
```

常见运算符：

| 运算符 | 含义 | 示例 |
|---|---|---|
| `=`、`<>`、`!=` | 等于、不等于 | `status <> 'closed'` |
| `<`、`<=`、`>`、`>=` | 大小比较 | `price >= 100` |
| `AND`、`OR`、`NOT` | 逻辑组合 | `stock > 0 AND price < 500` |
| `BETWEEN` | 闭区间 | `price BETWEEN 10 AND 20` |
| `IN` | 属于集合 | `role IN ('user', 'admin')` |
| `LIKE` | 模式匹配 | `name LIKE '%SQL%'` |
| `IS NULL` | 判断空值 | `training_flag IS NULL` |
| `EXISTS` | 子查询是否返回行 | `EXISTS (SELECT 1 ...)` |

`AND` 的优先级通常高于 `OR`，复杂条件应使用括号明确表达：

```sql
WHERE role = 'admin'
   OR (role = 'user' AND created_at >= '2026-01-01')
```

注入中常见的“恒真条件”本质上就是利用逻辑运算符和注释改变原来的条件树。防御不能依靠猜测攻击者会使用哪个逻辑表达式，而应阻止输入成为语法。

### 8.5 `NULL` 与三值逻辑

SQL 条件可能为真、假或未知。`NULL = NULL` 的结果不是 `TRUE`，因此应写：

```sql
SELECT * FROM products WHERE training_flag IS NULL;
SELECT * FROM products WHERE training_flag IS NOT NULL;
```

`NOT IN` 遇到包含 `NULL` 的集合时可能产生意外结果；安全开发中应理解业务上的“未知”含义，不能简单把 `NULL` 当空字符串或零。

### 8.6 `LIKE` 和通配符

```sql
SELECT id, name
FROM products
WHERE name LIKE '%键盘%';
```

- `%` 匹配任意长度字符串；
- `_` 匹配单个字符；
- 参数绑定能防 SQL 注入，但如果业务要求“按字面搜索”，还要转义用户输入中的 `%` 和 `_`，否则会产生通配符扩大问题；
- “通配符扩大”与 SQL 注入不同：前者仍是数据，只是搜索语义过宽。

### 8.7 `ORDER BY` 排序

```sql
SELECT id, name, price
FROM products
ORDER BY category ASC, price DESC;
```

排序列和 `ASC`/`DESC` 属于 SQL 结构，通常不能用普通值参数绑定。错误示例：

```python
sql = "SELECT id, name, price FROM products ORDER BY " + user_sort
```

正确做法是固定映射：

```python
sort_columns = {
    "name": "name",
    "price": "price",
    "created": "created_at",
}
sort_column = sort_columns.get(user_sort, "name")
direction = "DESC" if user_direction == "desc" else "ASC"
sql = f"SELECT id, name, price FROM products ORDER BY {sort_column} {direction}"
```

这里仍使用了字符串格式化，但进入格式化的内容只能来自程序内固定集合，不是未经验证的原始输入。

### 8.8 `LIMIT` 与分页

```sql
SELECT id, name
FROM products
ORDER BY id
LIMIT 20 OFFSET 40;
```

多数现代驱动允许把数量和偏移量作为整数参数绑定。还应设置最大页大小，避免一次查询耗尽内存。数据量很大时，基于稳定索引的游标分页通常比深 `OFFSET` 更高效。

SQL Server 常见写法：

```sql
SELECT id, name
FROM products
ORDER BY id
OFFSET 40 ROWS FETCH NEXT 20 ROWS ONLY;
```

## 9. `UPDATE`：修改已有数据

```sql
UPDATE products
SET price = 369.00,
    stock = stock + 5
WHERE id = 2;
```

没有 `WHERE` 会更新整张表：

```sql
UPDATE products SET stock = 0;
```

工程实践：

- 先用相同 `WHERE` 执行 `SELECT`，确认目标行；
- 重要修改放在事务中；
- 检查受影响行数，避免静默修改过多记录；
- 乐观锁可在条件中加入版本号：`WHERE id = ? AND version = ?`；
- 所有值仍应绑定参数，不能因为是内部管理接口就拼接。

## 10. `DELETE`：删除行

```sql
DELETE FROM order_items
WHERE order_id = 1001 AND product_id = 2;
```

`DELETE FROM products;` 会删除全部商品。生产系统通常采用软删除字段、审计记录或可恢复流程，但软删除不等于安全：所有查询都要正确处理状态，数据库权限仍应最小化。

## 11. 聚合函数、`GROUP BY` 和 `HAVING`

常用聚合函数：

- `COUNT(*)`：行数；
- `COUNT(column)`：该列非 `NULL` 的行数；
- `SUM(column)`：求和；
- `AVG(column)`：平均值；
- `MIN(column)`、`MAX(column)`：最小值、最大值。

```sql
SELECT
    category,
    COUNT(*) AS product_count,
    AVG(price) AS average_price,
    SUM(stock) AS total_stock
FROM products
GROUP BY category
HAVING COUNT(*) >= 2
ORDER BY average_price DESC;
```

`WHERE` 在分组前过滤行，`HAVING` 在分组后过滤组。动态报表经常拼接筛选和排序，是常见注入入口；应为不同筛选条件组合 SQL 模板，并只绑定值。

## 12. `JOIN`：连接多张表

### 12.1 `INNER JOIN`

只返回两侧匹配的行：

```sql
SELECT o.id, u.username, o.status
FROM orders AS o
INNER JOIN users AS u ON u.id = o.user_id;
```

### 12.2 `LEFT JOIN`

保留左表全部行；右表无匹配时对应列为 `NULL`：

```sql
SELECT u.username, o.id AS order_id
FROM users AS u
LEFT JOIN orders AS o ON o.user_id = u.id;
```

把右表过滤条件放在 `WHERE` 中，可能让 `LEFT JOIN` 实际变成内连接：

```sql
-- 只保留存在 pending 订单的用户
SELECT u.username, o.id
FROM users AS u
LEFT JOIN orders AS o ON o.user_id = u.id
WHERE o.status = 'pending';
```

如果希望保留没有订单的用户，应把条件放在 `ON`：

```sql
SELECT u.username, o.id
FROM users AS u
LEFT JOIN orders AS o
  ON o.user_id = u.id AND o.status = 'pending';
```

### 12.3 其他连接

- `RIGHT JOIN`：保留右表全部行；SQLite 不同版本支持情况不同；
- `FULL OUTER JOIN`：保留两侧全部行；MySQL 没有直接语法；
- `CROSS JOIN`：笛卡尔积，行数是两表行数乘积；
- 自连接：同一张表用两个别名连接，例如员工和上级关系。

漏写或写错连接条件可能产生大量数据，但这属于查询逻辑错误，不是 SQL 注入。

## 13. 子查询

标量子查询返回一个值：

```sql
SELECT name, price
FROM products
WHERE price > (SELECT AVG(price) FROM products);
```

集合子查询与 `IN`：

```sql
SELECT username
FROM users
WHERE id IN (
    SELECT user_id
    FROM orders
    WHERE status = 'pending'
);
```

相关子查询与 `EXISTS`：

```sql
SELECT u.id, u.username
FROM users AS u
WHERE EXISTS (
    SELECT 1
    FROM orders AS o
    WHERE o.user_id = u.id
      AND o.status = 'pending'
);
```

`EXISTS` 只关心是否存在行，里面常写 `SELECT 1`。复杂子查询同样应该绑定参数；嵌套层数不会天然提高安全性。

## 14. 集合运算：`UNION`、`UNION ALL`、`INTERSECT`、`EXCEPT`

```sql
SELECT username AS label FROM users
UNION
SELECT name AS label FROM products;
```

规则：

- 两侧查询的列数必须相同；
- 对应列的数据类型必须兼容；
- `UNION` 去重，`UNION ALL` 保留重复并通常更快；
- 最终列名通常来自第一条查询；
- `INTERSECT` 取交集，`EXCEPT` 取差集，MySQL 的支持情况与版本有关。

联合注入正是利用“列数相同、类型兼容”的规则，把另一条查询结果并入原页面结果。第二章会在本地固定数据上逐步展示。

## 15. `CASE` 条件表达式

```sql
SELECT
    name,
    price,
    CASE
        WHEN price < 100 THEN 'low'
        WHEN price < 300 THEN 'medium'
        ELSE 'high'
    END AS price_level
FROM products;
```

`CASE` 可以出现在 `SELECT`、`ORDER BY`、聚合表达式等位置。布尔盲注的本质也可理解为让数据库根据某个条件选择不同结果，但防守者不应试图逐个封禁条件函数。

## 16. 常用函数

函数名称存在方言差异，下面列出常见概念：

| 类别 | 示例 | 用途 |
|---|---|---|
| 字符串 | `LOWER`、`UPPER`、`LENGTH`、`SUBSTRING`、`TRIM` | 大小写、长度、截取、去空白 |
| 数值 | `ROUND`、`ABS`、`CEILING`、`FLOOR` | 舍入和数学计算 |
| 日期 | `CURRENT_TIMESTAMP`、日期加减函数 | 当前时间与区间计算 |
| 空值 | `COALESCE`、`NULLIF` | 默认值和条件空值 |
| 类型 | `CAST`、`CONVERT` | 显式类型转换 |
| 拼接 | `CONCAT` 或 `||` | 合并字符串，具体语法依数据库 |

```sql
SELECT
    UPPER(username) AS normalized_name,
    COALESCE(role, 'user') AS effective_role,
    CAST(id AS VARCHAR(20)) AS id_text
FROM users;
```

注入检测经常借助字符串、类型转换、条件和延迟函数，但函数本身不是漏洞；漏洞仍来自攻击者能够改变语法结构。

## 17. CTE：`WITH`

公用表表达式让复杂查询更易读：

```sql
WITH user_totals AS (
    SELECT
        o.user_id,
        SUM(oi.quantity * oi.unit_price) AS total_amount
    FROM orders AS o
    JOIN order_items AS oi ON oi.order_id = o.id
    GROUP BY o.user_id
)
SELECT u.username, COALESCE(t.total_amount, 0) AS total_amount
FROM users AS u
LEFT JOIN user_totals AS t ON t.user_id = u.id;
```

递归 CTE 可处理树和图，但不同产品的限制不同。CTE 改善可维护性，不替代参数绑定。

## 18. 窗口函数

窗口函数在保留明细行的同时计算排名、累计值或分组统计：

```sql
SELECT
    category,
    name,
    price,
    ROW_NUMBER() OVER (
        PARTITION BY category
        ORDER BY price DESC
    ) AS price_rank
FROM products;
```

常见函数包括 `ROW_NUMBER`、`RANK`、`DENSE_RANK`、`LAG`、`LEAD` 和带 `OVER` 的聚合函数。报表接口中的动态窗口排序仍应使用固定列映射。

## 19. 事务与 ACID

事务的四个经典特征：原子性、一致性、隔离性和持久性。

```sql
BEGIN;

UPDATE products
SET stock = stock - 1
WHERE id = 2 AND stock >= 1;

INSERT INTO order_items (order_id, product_id, quantity, unit_price)
VALUES (1001, 2, 1, 399.00);

COMMIT;
```

发生异常时：

```sql
ROLLBACK;
```

保存点允许部分回滚：

```sql
BEGIN;
SAVEPOINT before_item;
-- 若后续操作失败
ROLLBACK TO SAVEPOINT before_item;
COMMIT;
```

隔离级别决定并发事务能看见什么，常见问题包括脏读、不可重复读和幻读。注入防御不能依赖“反正会回滚”：查询可能已经读取并返回数据，也可能调用事务之外的功能。

## 20. 索引和执行计划

```sql
CREATE INDEX idx_products_category_price
ON products(category, price);

EXPLAIN
SELECT id, name, price
FROM products
WHERE category = 'device'
ORDER BY price;
```

复合索引的列顺序很重要。索引通常提升读取速度，但会占空间并降低写入速度。`EXPLAIN` 用于理解执行计划，不应直接把用户输入拼接成任意诊断语句。

## 21. 权限：`GRANT` 和 `REVOKE`

不同数据库语法差异明显，概念示例如下：

```sql
GRANT SELECT, INSERT, UPDATE ON shop_lab.* TO app_user;
REVOKE DROP, ALTER ON shop_lab.* FROM app_user;
```

实际部署建议：

- 应用运行账号与数据库管理员账号分离；
- 只读接口使用只读账号或只读事务；
- 不授予应用不需要的结构修改、文件、网络或系统管理权限；
- 测试和生产使用不同凭据；
- 凭据放入安全的密钥管理系统，不写进代码仓库。

最小权限不能消除注入，但能显著限制漏洞影响范围。

## 22. 应用如何安全执行 SQL

### 22.1 错误：字符串拼接

```python
username = request.args["username"]
sql = "SELECT id, role FROM users WHERE username = '" + username + "'"
row = connection.execute(sql).fetchone()
```

如果 `username` 中含有引号、运算符或注释，最终 SQL 结构可能被改变。

### 22.2 正确：参数化查询

SQLite/Python：

```python
row = connection.execute(
    "SELECT id, role FROM users WHERE username = ?",
    (username,),
).fetchone()
```

PostgreSQL 常见驱动：

```python
cursor.execute(
    "SELECT id, role FROM users WHERE username = %s",
    (username,),
)
```

参数绑定的关键不是“自动添加引号”，而是数据库或驱动在解析语句结构时，把 SQL 模板和参数值作为不同通道处理。即使参数值包含 `' OR ...`，它也只是一个完整字符串值。

### 22.3 参数化不能直接处理的部分

普通值占位符通常不能绑定：

- 表名和列名；
- `ASC`、`DESC`；
- SQL 关键词和运算符；
- 任意数量的占位符列表；
- 一整段 `WHERE` 或 `ORDER BY` 子句。

这些需求应使用固定模板、白名单映射或查询构造器，而不是把原始用户输入直接插入 SQL。

## 23. 常见数据库方言差异速查

| 功能 | MySQL | PostgreSQL | SQL Server | SQLite |
|---|---|---|---|---|
| 字符串拼接 | `CONCAT()` | `||` | `+`/`CONCAT()` | `||` |
| 分页 | `LIMIT` | `LIMIT` | `OFFSET ... FETCH`/`TOP` | `LIMIT` |
| 当前数据库 | `DATABASE()` | `current_database()` | `DB_NAME()` | 单文件/附加库 |
| 当前用户 | `USER()` | `current_user` | `SYSTEM_USER` | 无传统服务器用户 |
| 版本信息 | `VERSION()` | `version()` | `@@VERSION` | `sqlite_version()` |
| 延迟函数 | `SLEEP()` | `pg_sleep()` | `WAITFOR DELAY` | 无内置网络式延迟函数 |
| 标识符引用 | 反引号 | 双引号 | 方括号/双引号 | 双引号 |

这些差异用于理解实验现象，不应被用来替代应用代码审计。看到特定错误消息只能形成数据库类型假设，还需要授权范围内的进一步证据。

## 24. 本章练习

1. 创建四张练习表并插入至少三名用户、五个商品和两张订单。
2. 查询每个分类的商品数、平均价格和库存总量。
3. 用 `LEFT JOIN` 查询所有用户及其订单数，包括没有订单的用户。
4. 用子查询找出高于全体商品平均价的商品。
5. 用 CTE 计算每位用户的订单总额。
6. 用窗口函数找出每个分类价格最高的两个商品。
7. 分别用字符串拼接和参数绑定查询一个包含单引号的用户名，比较最终行为。
8. 为商品排序接口设计 `name`、`price` 两个白名单字段及升降序映射。

完成后再进入第二章。若不能准确解释数据库最终收到的完整 SQL，直接学习 payload 很容易变成机械记忆。

