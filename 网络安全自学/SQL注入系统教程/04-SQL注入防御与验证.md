# 第四章：SQL 注入防御、失效原因与验证

## 1. 防御目标

真正的防御目标不是“识别所有恶意 payload”，而是建立不可跨越的代码与数据边界：

```text
固定 SQL 结构 + 独立绑定的参数值 + 受控的动态标识符
```

只要不可信输入始终作为一个参数值进入数据库，它包含引号、注释、关键字、Unicode 或编码后的文本都不能改变语句结构。

完整防御分为四层：

1. **消除漏洞**：参数化查询、可靠查询构造器、标识符白名单；
2. **限制影响**：数据库最小权限、网络隔离、关闭高风险能力；
3. **减少泄露**：统一错误处理、最少数据查询、敏感字段隔离；
4. **发现和恢复**：日志、告警、自动化测试、凭据轮换和事件响应。

WAF 属于补充检测层，不能代替第一层。

## 2. 参数化查询：首要修复

### 2.1 参数化到底做了什么

字符串拼接把模板和输入合并成一段 SQL 文本：

```text
"... WHERE username = '" + username + "'"
```

参数化把 SQL 模板和参数分开提交：

```text
SQL template: SELECT ... WHERE username = ?
parameters:   ["alice' OR '1'='1"]
```

数据库解析出的结构始终只有一个等值条件。整个参数内容是一个字符串值，不会被再次解析成操作符或注释。

### 2.2 Python SQLite

```python
row = connection.execute(
    "SELECT id, username, role FROM users WHERE username = ?",
    (username,),
).fetchone()
```

单元素元组必须写 `(username,)`。不要把值先用 `%` 或 f-string 放进模板。

### 2.3 Python PostgreSQL 驱动

```python
cursor.execute(
    "SELECT id, username, role FROM users WHERE username = %s",
    (username,),
)
```

这里的 `%s` 是驱动占位符，不是 Python 字符串格式化：

```python
# 错误
cursor.execute("SELECT * FROM users WHERE username = '%s'" % username)
```

占位符周围通常不手工加引号，驱动会按参数类型编码。

### 2.4 Java JDBC

```java
String sql = "SELECT id, username, role FROM users WHERE username = ?";
try (PreparedStatement statement = connection.prepareStatement(sql)) {
    statement.setString(1, username);
    try (ResultSet rows = statement.executeQuery()) {
        // 处理结果
    }
}
```

不要先用字符串拼接生成 `sql`，再交给 `prepareStatement`。调用名包含 “prepared” 不代表之前的拼接会自动变安全。

### 2.5 PHP PDO

```php
$statement = $pdo->prepare(
    'SELECT id, username, role FROM users WHERE username = :username'
);
$statement->execute(['username' => $username]);
$row = $statement->fetch();
```

不要用 `PDO::quote()` 手工拼接整个查询。预处理和绑定更清晰，也更不容易在字符集、类型和遗漏分支上犯错。

### 2.6 Node.js PostgreSQL

```javascript
const result = await pool.query(
  'SELECT id, username, role FROM users WHERE username = $1',
  [username],
);
```

### 2.7 Node.js MySQL

```javascript
const [rows] = await connection.execute(
  'SELECT id, username, role FROM users WHERE username = ?',
  [username],
);
```

确认使用的库和方法真正执行参数绑定。某些库的“格式化”只是客户端字符串转义，安全属性和配置依实现而异。

### 2.8 C# ADO.NET

```csharp
using var command = connection.CreateCommand();
command.CommandText =
    "SELECT id, username, role FROM users WHERE username = @username";
command.Parameters.Add("@username", SqlDbType.NVarChar, 50).Value = username;
```

显式类型和长度能减少隐式转换及执行计划问题。不要使用 `AddWithValue` 代替对类型的理解。

## 3. 参数化查询不能直接绑定什么

值参数通常不能代替 SQL 语法对象：

- 表名；
- 列名；
- `ASC` / `DESC`；
- 运算符；
- 任意 SQL 关键词；
- 整段 `WHERE`、`JOIN` 或 `ORDER BY`；
- 可变数量的占位符列表本身。

因此下面通常不能按预期排序：

```sql
SELECT id, name FROM products ORDER BY ?;
```

参数会成为一个常量值，而不是列标识符。开发者有时因此退回字符串拼接，重新引入漏洞。正确解法是白名单映射。

## 4. 动态列、排序和表名的白名单

```python
SORT_COLUMNS = {
    "name": "name",
    "price": "price",
    "created": "created_at",
}

SORT_DIRECTIONS = {
    "asc": "ASC",
    "desc": "DESC",
}

column = SORT_COLUMNS.get(requested_column, "name")
direction = SORT_DIRECTIONS.get(requested_direction, "asc")

sql = f"SELECT id, name, price FROM products ORDER BY {column} {direction}"
rows = connection.execute(sql).fetchall()
```

这里能进入 f-string 的内容只来自源代码中的固定值。不要写成“只要匹配字母数字就允许”，因为合法标识符集合通常很小，固定映射更容易审计。

多租户分表或报表系统确需动态表名时：

1. 用业务 ID 查询可信元数据；
2. 把业务 ID 映射到服务端控制的物理表名；
3. 使用数据库驱动提供的标识符引用 API；
4. 数据库账号只访问允许的 schema；
5. 不把客户端传来的表名原样插入 SQL。

## 5. 动态筛选条件的安全构造

业务经常有可选筛选条件。安全模式是拼接固定片段，只绑定值：

```python
clauses = []
parameters = []

if category is not None:
    clauses.append("category = ?")
    parameters.append(category)

if min_price is not None:
    clauses.append("price >= ?")
    parameters.append(min_price)

if in_stock_only:
    clauses.append("stock > 0")

sql = "SELECT id, name, price FROM products"
if clauses:
    sql += " WHERE " + " AND ".join(clauses)

rows = connection.execute(sql, parameters).fetchall()
```

安全性来自：`clauses` 中的每个字符串由程序员固定编写，不包含原始输入；所有变化的数据进入 `parameters`。

复杂项目宜使用成熟查询构造器，并检查它生成的 SQL 和绑定参数。自制 DSL 容易在标识符、函数和 raw fragment 上重蹈字符串拼接问题。

## 6. 安全处理 `IN` 列表

占位符数量由列表长度决定，但每个值仍单独绑定：

```python
product_ids = [int(value) for value in requested_ids]

if not product_ids:
    rows = []
else:
    placeholders = ",".join("?" for _ in product_ids)
    sql = f"SELECT id, name FROM products WHERE id IN ({placeholders})"
    rows = connection.execute(sql, product_ids).fetchall()
```

占位符字符串由列表长度生成，没有任何输入文本进入 SQL。还应限制列表最大长度，防止超大查询。

某些数据库和驱动支持数组参数，例如 PostgreSQL 的 `= ANY($1)`，可以减少动态占位符处理。

## 7. `LIKE` 查询的两类问题

参数绑定解决 SQL 注入：

```python
pattern = f"%{query}%"
rows = connection.execute(
    "SELECT id, name FROM products WHERE name LIKE ?",
    (pattern,),
).fetchall()
```

但如果业务要求按字面搜索，用户输入的 `%` 和 `_` 仍会成为 `LIKE` 通配符。应按数据库方言转义，并显式指定转义字符：

```python
def escape_like(value: str) -> str:
    return value.replace("\\", "\\\\").replace("%", "\\%").replace("_", "\\_")

pattern = f"%{escape_like(query)}%"
rows = connection.execute(
    "SELECT id, name FROM products WHERE name LIKE ? ESCAPE '\\'",
    (pattern,),
).fetchall()
```

“通配符扩大”不是 SQL 注入，因为输入仍无法引入 SQL 运算符，但它可能造成结果过宽和性能问题。

## 8. 数字、日期和布尔输入

类型校验是业务约束，参数绑定是结构安全，两者不能互相代替：

```python
try:
    product_id = int(raw_product_id)
except ValueError:
    return bad_request("invalid product id")

row = connection.execute(
    "SELECT id, name FROM products WHERE id = ?",
    (product_id,),
).fetchone()
```

只执行 `int()` 可以阻止这个特定数字位置的拼接注入，但代码以后可能改成字符串字段，或异常路径又使用原始值。统一坚持绑定参数更可靠。

日期应使用语言和驱动的日期类型，不要用正则拼出 SQL 日期表达式。布尔值应解析成明确的 `True/False`，不要允许任意字符串作为条件片段。

## 9. ORM 的安全边界

正常 ORM 条件通常会绑定参数：

```python
Product.query.filter(Product.category == category)
```

风险集中在：

- `raw()`、`text()`、`extra()`、原生查询接口；
- 动态 `order_by` 字符串；
- 动态表名、列名和函数名；
- 手工构造过滤表达式；
- 为性能优化而复制出的原生 SQL；
- ORM 日志中误把“渲染后的 SQL”复制回代码。

代码审计不能只搜索数据库连接 API，还应搜索 ORM 的 raw SQL 逃生口。

## 10. 存储过程的安全边界

安全的存储过程使用固定语句和参数。危险模式是在过程内部执行拼接的动态 SQL：

```sql
-- 伪代码：危险
SET query_text = 'SELECT ... WHERE name = ''' + input_name + '''';
EXECUTE query_text;
```

数据库通常为动态 SQL 提供绑定机制，例如 PostgreSQL `EXECUTE ... USING`、SQL Server `sp_executesql` 参数。标识符仍需白名单和数据库提供的安全引用函数。

存储过程还要审查执行者权限与定义者权限差异，避免低权限应用借过程取得不必要能力。

## 11. 输入校验：有用但不是主要 SQL 防线

输入校验适合表达业务规则：

- 商品 ID 必须是正整数；
- 页大小在 `1..100`；
- 状态只能是 `pending/paid/closed`；
- 排序字段只能是固定集合；
- 用户名长度和 Unicode 规范明确。

它不能替代参数绑定，因为：

- 姓名、地址和搜索词合法地包含引号、连字符和 Unicode；
- 黑名单不可能枚举 SQL 的全部等价语法；
- 同一数据可能在第二个功能中进入不同 SQL 上下文；
- 数据库方言、字符集和解码层会变化；
- 校验代码可能在某条异常分支被遗漏。

推荐顺序：解析和规范化 → 业务白名单/范围校验 → 参数绑定。

## 12. 规范化和重复参数

请求可能被多层处理：

```text
客户端 -> CDN/WAF -> 反向代理 -> Web 框架 -> 应用 -> 驱动 -> DBMS
```

每层对 URL 编码、Unicode、重复参数和非法字节的处理可能不同。典型风险是网关检查第一个 `id`，应用却使用最后一个 `id`。

防御建议：

- 整条链统一 UTF-8 和规范化策略；
- 拒绝不允许的重复参数；
- 在解码后的规范值上做业务校验；
- WAF 和应用采用一致的解析器行为；
- 无论规范化结果是什么，数据库访问仍使用绑定参数。

## 13. 数据库最小权限

为每个服务或职责分配独立账号：

| 组件 | 建议权限 |
|---|---|
| 商品查询 API | 指定表/视图的 `SELECT` |
| 下单服务 | 所需表的 `SELECT`、`INSERT`、有限 `UPDATE` |
| 后台迁移任务 | 仅在部署窗口获得 DDL 权限 |
| 备份系统 | 专用只读/备份角色 |
| 管理员 | 不作为 Web 应用运行身份 |

避免授予：

- 创建或删除数据库对象；
- 读取/写入服务器文件；
- 执行外部程序或不受控扩展；
- 任意跨库访问；
- 创建高权限用户；
- 不必要的网络访问能力。

最小权限不能让漏洞消失，但能防止一个商品查询漏洞直接影响用户表、文件系统或整台数据库服务器。

## 14. 多账号、视图和行级权限

大型系统可以进一步隔离：

- 读写账号分离；
- 每个微服务独立数据库角色；
- 只暴露必要列的视图；
- PostgreSQL Row Level Security 等行级策略；
- 租户 ID 同时由应用和数据库策略约束；
- 管理功能使用独立网络和身份边界。

视图和行级权限是纵深防御。若应用在高权限账号下绕开策略，或动态 SQL 使用了错误身份，它们也可能失效。

## 15. 禁用不需要的数据库能力

- 驱动默认关闭 multi-statements；
- 禁止应用账号加载扩展或用户定义函数；
- 禁止不必要的本地/远程文件访问；
- 数据库服务器不直接访问互联网；
- 出站 DNS、HTTP 和文件共享采用白名单；
- 关闭示例数据库、默认账号和不使用的监听接口；
- 及时安装数据库和驱动安全更新。

关闭堆叠语句不能修复联合或盲注，出站隔离也不能修复查询注入；它们用于减少升级路径。

## 16. 安全错误处理

客户端响应：

```json
{
  "error": "request_failed",
  "request_id": "7f2c..."
}
```

服务端受控日志记录：

```text
request_id, route, authenticated_user_id, error_class,
database_error_code, service_version
```

不要向客户端返回：

- 完整 SQL；
- 数据库产品和版本；
- 表名、列名和文件路径；
- 堆栈跟踪；
- 连接字符串和凭据；
- 原始敏感参数。

隐藏错误只减少报错型信息通道。联合、布尔或时间型注入仍可能存在，所以它不是根因修复。

## 17. 密码和敏感数据

即使 SQL 完全参数化，也必须正确保护数据：

- 密码使用 Argon2id、bcrypt 或组织批准的自适应密码哈希；
- 每个密码使用独立随机盐；
- API Token 尽量只保存不可逆摘要；
- 加密密钥与数据库分离管理；
- 查询只选择需要的列，不使用 `SELECT *`；
- 敏感字段采用额外访问控制和审计；
- 测试环境不复制未脱敏的生产数据。

这样即使未来出现其他读取漏洞，影响也更受限制。

## 18. 日志与检测

可观察信号包括：

- 同一参数短时间出现大量真假相近请求；
- 引号、注释、数据库函数和异常编码组合；
- 大量相似请求只有一个字符位置变化；
- 响应时间呈固定阶梯；
- 数据库语法错误突然增加；
- 应用账号访问从未使用过的系统目录或表；
- 数据库账号权限提升、扩展创建或出站连接；
- 同一来源触发多个参数和多种技术探测。

日志不应保存明文密码、完整 Token 或不必要的请求正文。对用户输入做结构化记录和长度限制，避免日志注入与敏感数据扩散。

告警要结合路由、用户身份、历史基线和服务端数据库错误，不能只匹配单个字符串。

## 19. WAF 的正确角色

WAF 可以：

- 阻断已知明显模式；
- 降低自动化扫描速度；
- 为修复窗口提供临时缓解；
- 记录跨应用的异常输入；
- 对敏感路径实施速率限制。

WAF 不能：

- 理解所有应用 SQL 上下文；
- 保证不同解码层看到相同文本；
- 修复应用内部、消息队列或批处理中的二次注入；
- 阻止不包含典型关键字的所有布尔表达式；
- 代替数据库最小权限和代码修复。

部署 WAF 后仍应进行源代码修复，并验证“关闭 WAF 时参数化查询依然安全”。

## 20. 薄弱防御为何失效，以及如何在靶场验证

本节的“绕过”是防守者在本地或授权环境中验证控制是否可靠，不是针对第三方系统的规避指南。

| 薄弱措施 | 失效原因 | 本地验证思路 | 正确修复 |
|---|---|---|---|
| 只在浏览器做校验 | 客户端请求可自行构造 | 用 `curl` 直接发送不符合前端规则的值 | 服务端校验 + 参数绑定 |
| 只禁止单引号 | 数字和排序上下文不需要字符串引号 | 在数字型实验参数比较真假表达式 | 所有上下文参数化/白名单 |
| 只匹配小写关键字 | SQL 关键词通常大小写不敏感 | 在自建过滤器测试大小写变化 | 删除黑名单依赖，参数化 |
| 只拦截 `UNION` | 布尔、时间、报错等通道不依赖联合 | 对靶场使用真/假条件 | 修复拼接根因 |
| 只拦截空格 | SQL 存在其他空白和语法分隔方式 | 单元测试规范化前后差异 | 参数化，不解析 payload |
| 只拦截注释 | 可以让剩余语句本身语法成立 | 构造合法括号/条件的测试用例 | 参数化 |
| 只做一次 URL 解码检查 | 网关与框架可能解码次数不同 | 测试单/双编码在各层的解析日志 | 统一规范化 + 参数化 |
| 重复参数取值不一致 | WAF 与应用可能选不同值 | 测试 `id=1&id=2` 的各层结果 | 拒绝重复或统一策略 |
| 手工替换 `'` 为 `''` | 容易漏掉上下文、字符集和非字符串位置 | 覆盖数字、标识符、二次使用 | 驱动参数绑定 |
| ORM 自动安全 | raw SQL 和动态排序仍可拼接 | 搜索 ORM raw/text/extra 接口 | 限制 raw API + 绑定/映射 |
| 存储过程自动安全 | 过程内部可能动态执行拼接 SQL | 审查过程中的 EXECUTE/EXEC | 动态 SQL 绑定 + 最小权限 |
| 只隐藏数据库错误 | 仍存在联合和盲通道 | 对比真假内容与时间 | 参数化 + 统一错误 |
| 只关闭多语句 | 只阻止部分堆叠场景 | 验证联合/布尔仍可能存在 | 参数化 + 最小权限 |
| 只依靠 WAF | 规范化、语法等价和业务上下文造成漏报 | 在专用测试规则上做变体回归 | 代码修复，WAF 作补充 |

表中每个测试都应使用固定的本地数据和无破坏条件，不执行数据修改或主机操作。

## 21. 为什么黑名单无法覆盖 SQL 语法

假设过滤器阻止某几个关键词，它仍需处理：

- 大小写差异；
- 不同空白和换行；
- 数据库注释语法；
- 等价逻辑和比较运算；
- 函数与运算符的同义表达；
- URL、JSON、Unicode 和字符集解码；
- 数字、字符串、标识符、路径等不同上下文；
- 数据库版本与方言差异；
- 输入在存储后被二次使用。

这是一个开放语法识别问题，而黑名单通常只是有限字符串匹配。参数化直接消除“输入被当成语法”这一前提，因此比扩充规则可靠得多。

## 22. 手工转义为什么不作为首选

正确实现、正确字符集、正确上下文下的数据库专用转义可能保护一个字符串字面量，但工程上容易出现：

- 忘记某个调用点；
- 对数字或标识符错误套用字符串转义；
- 连接字符集和转义函数字符集不一致；
- 先转义后解码，顺序错误；
- 二次存储后又拼接；
- 开发者给已经转义的数据再次转义；
- 更换 DBMS 后保留旧规则。

参数绑定把这些细节交给驱动协议和数据库类型系统处理。只有驱动确实不支持绑定的极少数场景，才应使用该 DBMS 官方标识符 API、固定白名单和严格代码审查。

## 23. 正确参数化有“通用绕过”吗

对于普通值位置，正确实现的参数化查询没有一种靠改变输入文本就能把参数重新解释为 SQL 的通用办法。如果安全测试发现“预处理语句仍可注入”，应优先寻找以下实现错误：

1. 在调用预处理 API 之前已经拼接；
2. 只有部分值绑定，排序或过滤片段仍直接插入；
3. ORM 退回 raw SQL；
4. 参数先安全存储，之后被二次拼接；
5. 应用自己实现了不正确的伪预处理；
6. 多条查询中只有第一条修复；
7. 日志、报表、导出或后台任务走另一条代码路径；
8. 实际请求仍由旧版本实例处理。

不要把这些实现缺口描述成“破解了参数化”；真正被绕过的是不完整的修复边界。

## 24. 认证查询的正确实现

```python
row = connection.execute(
    "SELECT id, password_hash, role FROM users WHERE username = ?",
    (normalized_username,),
).fetchone()

authenticated = False
if row is not None:
    authenticated = password_hasher.verify(row["password_hash"], supplied_password)
```

还应：

- 用户名具有明确规范化规则和唯一约束；
- 无论用户是否存在，对外错误信息一致；
- 密码验证使用成熟库；
- 失败尝试限速并记录；
- 管理员权限来自服务端查询结果，不来自客户端字段；
- 成功后重新生成会话 ID；
- 不在日志中记录口令。

## 25. 代码审计方法

### 25.1 搜索危险 API 和拼接

按技术栈搜索：

```text
execute, query, raw, text, extra, createNativeQuery,
Statement, prepareStatement, sp_executesql, EXECUTE IMMEDIATE
```

再检查同一行或上游是否有：

```text
字符串加法、格式化、模板字符串、join、replace、拼接的排序和列名
```

仅搜索单引号或 `SELECT` 会漏掉查询构造器、存储过程、`UPDATE`、日志数据库和二次注入。

### 25.2 做数据流追踪

对每个输入回答：

1. 来源是什么：URL、JSON、Cookie、Header、文件、消息还是数据库旧数据？
2. 经历了哪些解码、转换和校验？
3. 最终进入 SQL 的值、标识符还是整段语法？
4. 使用哪个数据库账号和连接配置？
5. 错误返回到哪里？
6. 同一数据是否在后台任务或报表中再次使用？

### 25.3 检查所有查询类型

不要只审查登录和 `SELECT`，还要覆盖：

- 搜索、筛选、排序、分页；
- 新增、批量更新和删除；
- 管理后台报表；
- CSV/Excel 导入；
- 审计日志查询；
- 定时任务和消息消费者；
- 多租户表/schema 选择；
- 数据迁移和维护脚本。

## 26. 单元测试

安全测试不应该断言某个 payload 被正则拦截，而应断言查询结构固定、参数独立。

```python
def test_product_lookup_treats_sql_text_as_value(client):
    response = client.get(
        "/safe/product",
        query_string={"id": "-1 UNION SELECT 1,2,3"},
    )

    assert response.status_code == 200
    assert "UNION-MARKER" not in response.text
    assert "Database error" not in response.text
```

再加入正常行为：

```python
def test_product_lookup_still_returns_known_product(client):
    response = client.get("/safe/product", query_string={"id": "1"})
    assert response.status_code == 200
    assert "SQL 入门书" in response.text
```

安全回归必须同时证明“恶意结构不起作用”和“正常功能没有被修坏”。

## 27. 集成测试与数据库日志验证

在测试数据库开启适当的查询审计，检查：

- 语句模板是否固定；
- 参数是否通过驱动单独传递；
- 应用账号是否只访问预期表；
- 错误是否被统一处理；
- multi-statements 是否关闭；
- 受影响行数是否符合预期。

注意不同驱动日志可能为了可读性把参数“渲染”进 SQL，看起来像拼接。需要结合驱动调用方式、数据库协议日志和行为测试判断，不能只看一行格式化日志。

## 28. 使用 sqlmap 做修复回归

只对本地安全路由执行低强度测试：

```bash
sqlmap \
  -u 'http://127.0.0.1:8765/safe/product?id=1' \
  -p id \
  --flush-session \
  --level=1 \
  --risk=1 \
  --threads=1 \
  --batch
```

应检查：

1. sqlmap 不再报告参数可注入；
2. 服务器日志显示固定 SQL 模板与独立参数；
3. 单引号、Unicode、长输入和边界数值不会产生数据库错误；
4. 正常查询仍返回正确结果；
5. WAF 开关不影响修复结论。

工具未发现漏洞不能单独证明所有代码路径安全，仍需代码审计和单元测试。

## 29. 静态分析和 CI

在 CI 中组合：

- 语言级静态分析规则，标记 SQL API 附近的字符串拼接；
- ORM raw SQL 使用清单；
- 依赖漏洞扫描；
- 单元和集成安全回归；
- 对隔离靶场的低强度动态测试；
- 数据库迁移权限检查；
- 审计日志字段与脱敏测试。

静态规则应允许安全的固定片段组合，但要求审查所有动态标识符。过多误报会使团队忽略真正问题，所以规则要结合本项目封装 API 调整。

## 30. 发现漏洞后的事件响应

如果生产系统确认存在 SQL 注入：

1. 暂停或隔离受影响接口，必要时用 WAF 做临时精确缓解；
2. 保全应用、代理、数据库、身份和网络日志；
3. 确认漏洞出现时间、代码版本、参数位置和数据库账号权限；
4. 检查异常查询、批量读取、权限变化、文件/扩展操作和出站连接；
5. 修复所有同类代码路径，不只修复已知 URL；
6. 轮换可能暴露的数据库凭据、会话密钥和受影响用户令牌；
7. 从可信版本部署并执行回归测试；
8. 按法规和组织流程完成影响评估及通知；
9. 把测试加入 CI，避免同类缺陷回归。

不要立刻删除日志或攻击请求。先按事件响应流程保全证据，并由授权人员决定隔离和恢复动作。

## 31. 防御评审清单

### 查询构造

- [ ] 所有值使用驱动参数绑定；
- [ ] 表名、列名、排序和运算符来自固定映射；
- [ ] 可选筛选只组合程序内固定片段；
- [ ] `IN` 列表为每个元素生成占位符或使用数组参数；
- [ ] ORM raw SQL 和存储过程动态 SQL 已审计；
- [ ] 数据库旧数据再次进入查询时仍绑定参数。

### 输入和业务

- [ ] 服务端执行类型、范围、长度和枚举校验；
- [ ] 重复参数和编码策略明确；
- [ ] `LIKE` 通配符语义符合业务要求；
- [ ] 页大小、批量 ID 和报表范围有限制；
- [ ] 正常的引号、Unicode 和空值输入有测试。

### 权限和配置

- [ ] Web 应用不使用 DBA 账号；
- [ ] 每个服务只访问必要表和操作；
- [ ] multi-statements 和高风险数据库能力默认关闭；
- [ ] 数据库出站网络受限；
- [ ] 生产、测试和迁移账号分离；
- [ ] 凭据由密钥管理系统保存并定期轮换。

### 错误和监控

- [ ] 客户端不显示数据库错误和堆栈；
- [ ] 服务端日志可通过 request ID 关联；
- [ ] 日志不包含密码、完整 Token 和敏感查询结果；
- [ ] 数据库语法错误、异常查询和固定延迟模式有监控；
- [ ] WAF 规则作为补充且有误报/漏报回归；
- [ ] 事件响应联系人和流程明确。

### 测试

- [ ] 漏洞样例已转化为单元回归测试；
- [ ] 正常业务用例同时通过；
- [ ] 集成测试确认参数与模板分离；
- [ ] 修复后清除 sqlmap 旧会话重新检测；
- [ ] 测试数据为虚构或脱敏数据；
- [ ] 动态测试目标和请求速率严格受控。

## 32. 本章练习

1. 把本地靶场 `/product` 的拼接查询改为参数化，并比较相同联合字符串的结果。
2. 为商品排序实现 `name/price` 与 `asc/desc` 固定映射，测试未知字段回退到默认值。
3. 为可选的分类和最低价格筛选实现固定片段 + 参数列表。
4. 为 `IN` 列表处理空列表、非数字、超长列表和正常列表。
5. 编写包含单引号、百分号、下划线和 Unicode 的 `LIKE` 搜索测试。
6. 搜索一个熟悉项目中的 raw SQL API，判断每个输入进入值还是结构。
7. 给虚构的商品查询服务设计最小数据库权限，不授予用户表访问和 DDL 权限。
8. 写一份修复回归报告，包含手工真假对照、代码差异、sqlmap 低强度结果和正常功能测试。

## 33. 最终原则

可以把整套防御压缩成五句话：

1. 值一律绑定，不拼接；
2. 结构一律从固定白名单选择；
3. 数据库账号只拥有完成业务所需的最小权限；
4. 错误、日志和网络边界减少可利用信息与升级路径；
5. 每个历史漏洞都变成自动回归测试。

如果一项防御需要不断识别新的 payload 才能维持安全，它就不应被视为根因修复。

