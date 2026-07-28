# 第三章：sqlmap 常见参数与授权测试流程

## 1. sqlmap 做了什么

sqlmap 是 SQL 注入检测和验证工具。它自动完成的核心流程仍然是第二章的内容：

1. 发送正常请求建立响应基线；
2. 为指定参数生成与上下文有关的测试输入；
3. 比较真/假响应、错误、联合结果或时间差；
4. 推断后端 DBMS 和可用注入技术；
5. 在用户明确要求时执行有限的元数据查询或数据验证；
6. 把结果、会话和 HTTP 请求保存到输出目录。

sqlmap 不是“输入网址就自动拿下服务器”的按钮。动态页面、登录状态、CSRF Token、限流、缓存、网络抖动、非 SQL 后端和统一错误处理都会影响结果。工具输出仍需人工验证。

## 2. 安装和版本检查

任选一种安装方式，不要同时混用多个来源：

```bash
# macOS Homebrew
brew install sqlmap

# 使用 pipx 隔离 Python 命令行工具
pipx install sqlmap

# 检查版本
sqlmap --version
```

如果通过系统包管理器安装，后续也应通过同一包管理器更新。`sqlmap -h` 显示基础帮助，`sqlmap -hh` 显示完整帮助。不同版本的参数可能变化，应以本机 `-hh` 为准。

本教程不要求必须安装。理解参数含义后，再在本地靶场执行。

## 3. 第一条低强度本地检测命令

先启动第二章的本地靶场，然后执行：

```bash
sqlmap \
  -u 'http://127.0.0.1:8765/product?id=1' \
  -p id \
  --dbms=SQLite \
  --technique=BU \
  --level=1 \
  --risk=1 \
  --threads=1 \
  --batch
```

逐项解释：

- `-u`：唯一目标 URL，包含一个正常参数值；
- `-p id`：只测试 `id`，避免工具测试无关参数；
- `--dbms=SQLite`：因为实验源码已经明确告诉我们数据库类型；真实授权测试中不确定时不要乱指定；
- `--technique=BU`：只启用布尔盲注和联合查询，足以覆盖本地路由；
- `--level=1`：使用最小测试集合；
- `--risk=1`：避免高开销或可能影响数据的测试；
- `--threads=1`：单线程，便于查看服务端日志；
- `--batch`：使用默认答案，不在命令行中反复询问。

一开始不要同时使用最高 `level`、最高 `risk`、大量线程和所有技术。那会扩大请求数量、增加误报，也可能对授权系统造成不必要压力。

## 4. 帮助、版本和日志详细度

### 4.1 `-h` 与 `-hh`

```bash
sqlmap -h
sqlmap -hh
```

- `-h`：常用参数摘要；
- `-hh`：完整参数表，排查版本差异时优先查看。

### 4.2 `--version`

```bash
sqlmap --version
```

报告问题时应记录版本，因为 payload、指纹和解析逻辑会随版本更新。

### 4.3 `-v LEVEL`

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' -p id -v 3
```

详细度一般从 `0` 到 `6`：

| 值 | 主要内容 |
|---|---|
| `0` | 只显示关键结果和错误 |
| `1` | 默认信息 |
| `2` | 调试信息 |
| `3` | 注入 payload |
| `4` | HTTP 请求 |
| `5` | HTTP 响应头 |
| `6` | HTTP 响应正文 |

高详细度可能把 Cookie、令牌和响应数据写进终端或日志。处理授权系统时应保护输出文件，不要上传到公开 issue。

## 5. 指定目标

### 5.1 `-u URL` / `--url=URL`

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1'
```

用于测试单个 HTTP 目标。URL 应包含正常、可工作的参数值。Shell 中使用单引号包住 `&`、`?` 等字符，避免被 shell 解释。

### 5.2 `-r REQUEST_FILE`

复杂请求推荐保存为原始 HTTP 文件：

```http
GET /product?id=1 HTTP/1.1
Host: 127.0.0.1:8765
Accept: text/html
Connection: close

```

```bash
sqlmap -r request.txt -p id --level=1 --risk=1 --batch
```

适合以下情况：

- POST 表单或 JSON；
- 多个 Cookie、Header；
- URL 很长；
- 需要准确复现代理中抓到的授权请求。

保存请求前删除不必要的真实令牌和个人数据。原始请求文件中的 `Host` 决定目标，使用前必须再次确认范围。

### 5.3 `-m BULK_FILE`

从文件读取多个目标 URL：

```bash
sqlmap -m local-targets.txt --batch
```

批量模式容易越过授权范围，也容易制造较大流量。入门学习和单一应用验证不使用它；只有目标清单经过资产所有者确认时才考虑。

### 5.4 `-d CONNECTION_STRING`

`-d` 直接连接数据库，例如测试人员已持有实验库凭据时进行数据库审计。它不是 SQL 注入检测，也不应拿来尝试猜测凭据。连接字符串会包含敏感信息，应避免出现在 shell 历史、截图和共享日志中。

### 5.5 `-c CONFIG_FILE`

从配置文件加载参数：

```bash
sqlmap -c local-lab.conf
```

适合反复执行同一套授权测试。配置文件可能含 Cookie 或认证信息，应限制权限并排除出 Git。

### 5.6 自定义注入标记 `*`

当目标位置不是普通参数，`*` 可以明确标记测试点：

```text
http://127.0.0.1:8080/api/product/1*
```

原始 JSON 示例：

```json
{"filters":{"category":"book*"},"page":1}
```

标记让工具只替换该位置。不要在请求中留下多个无意的星号。

## 6. 控制 HTTP 请求

### 6.1 `--method=METHOD`

```bash
sqlmap -u 'http://127.0.0.1:8080/api/search' \
  --method=PUT --data='q=SQL' -p q
```

显式指定 `GET`、`POST`、`PUT` 等方法。提供 `--data` 时 sqlmap 通常会使用 POST，但显式写出可减少歧义。

### 6.2 `--data=DATA`

表单请求：

```bash
sqlmap -u 'http://127.0.0.1:8080/search' \
  --data='category=book&q=SQL' -p q
```

JSON 请求：

```bash
sqlmap -u 'http://127.0.0.1:8080/api/search' \
  --method=POST \
  --data='{"category":"book","q":"SQL"}' \
  --headers='Content-Type: application/json' \
  -p q
```

`--data` 在 shell 中应使用单引号包围。复杂嵌套 JSON 更适合 `-r`，避免转义错误。

### 6.3 `--param-del=CHAR`

当参数不是使用普通 `&` 分隔时，指定分隔符：

```bash
sqlmap -u 'http://127.0.0.1:8080/search?a=1;b=2' --param-del=';'
```

只在应用确实使用该格式时设置。

### 6.4 `--cookie=COOKIE`

```bash
sqlmap -u 'http://127.0.0.1:8080/account?id=1' \
  --cookie='lab_session=example; theme=dark' -p id
```

用于保持本地或授权靶场会话。Cookie 等同临时凭据：不要提交到仓库，不要粘贴到公开聊天，不要使用真实管理员会话做不必要测试。

如果测试点就在 Cookie 中，可使用 `-p cookie_name` 或在原始请求中加 `*` 标记。提高 `--level` 也可能让 sqlmap 自动测试 Cookie，但显式指定更可控。

### 6.5 `--headers=HEADERS` 与 `-H HEADER`

```bash
sqlmap -u 'http://127.0.0.1:8080/api/item?id=1' \
  --headers='Accept: application/json
X-Lab-Client: tutorial'
```

```bash
sqlmap -u 'http://127.0.0.1:8080/api/item?id=1' \
  -H 'X-Lab-Client: tutorial'
```

`--headers` 可提供多行 Header，`-H` 适合追加一项。认证头尤其敏感，优先放入受保护的请求文件。

### 6.6 `-A` / `--user-agent`、`--referer`、`--host`

```bash
sqlmap -u 'http://127.0.0.1:8080/item?id=1' \
  -A 'Local-Lab-Tester/1.0' \
  --referer='http://127.0.0.1:8080/'
```

- `-A` 设置 `User-Agent`；
- `--referer` 设置来源页；
- `--host` 覆盖 `Host` Header，常用于授权的虚拟主机实验。

推荐使用明确标识测试用途的 User-Agent，方便日志审计。`--random-agent` 会随机选择浏览器标识；它偶尔用于兼容只接受浏览器 UA 的实验环境，但不是修复 403 的通用办法，也不应拿来隐藏测试身份。

### 6.7 `--auth-type` 和 `--auth-cred`

HTTP Basic、Digest 等认证场景：

```bash
sqlmap -u 'http://127.0.0.1:8080/private/item?id=1' \
  --auth-type=Basic --auth-cred='labuser:labpassword'
```

命令行中的凭据可能进入 shell 历史，更安全的做法是使用权限受控的请求/配置文件，并在实验后作废临时凭据。

### 6.8 `--force-ssl`

强制使用 HTTPS。若 `-r` 中是旧请求或端口映射需要 HTTPS，可显式设置。不要使用它绕过证书验证问题；测试环境也应尽量使用正确证书。

## 7. 代理、超时和请求速率

### 7.1 `--proxy=URL`

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  --proxy='http://127.0.0.1:8081' -p id
```

把请求发送到本地调试代理，便于查看 sqlmap 实际生成的 HTTP 请求。代理必须属于你的实验环境，避免把授权数据送到第三方代理。

### 7.2 `--proxy-cred=USER:PASS`

为需要认证的测试代理提供凭据。与其他凭据一样，应避免出现在公开日志中。

### 7.3 `--delay=SECONDS`

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  -p id --delay=0.5 --threads=1
```

每个请求之间等待指定秒数。对资源有限的授权系统，降低速率比增加线程更重要。

### 7.4 `--timeout=SECONDS`

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' --timeout=10
```

单次连接或请求超时。设置过短会造成误判，过长会让失败目标拖延很久。时间盲注还要结合 `--time-sec`。

### 7.5 `--retries=COUNT`

网络失败后的重试次数。目标不稳定时适当增加，但先查清失败是网络问题、限流还是应用崩溃。

### 7.6 `--threads=COUNT`

并发请求数。布尔盲注的数据推断可受益于并发，但会增加服务器负载和响应交错。建议：

- 初始检测使用 `1`；
- 明确系统容量和授权后才逐步增加；
- 时间盲注和不稳定目标保持低并发；
- 发现错误率或延迟上升立即降低。

### 7.7 `--keep-alive`

尝试复用 HTTP 连接，减少握手开销。它与部分代理或多线程组合可能受限；遇到异常时回到默认配置。

## 8. 精确选择测试参数

### 8.1 `-p PARAMETER`

```bash
sqlmap -u 'http://127.0.0.1:8080/search?q=SQL&page=1' -p q
```

只测试指定参数。多个参数使用逗号：

```bash
-p q,category
```

这是控制范围最重要的参数之一。已知怀疑点时不要让工具测试整个请求。

### 8.2 `--skip=PARAMETER`

```bash
sqlmap -u 'http://127.0.0.1:8080/search?q=SQL&page=1' --skip=page
```

跳过已知安全、昂贵或会触发副作用的参数。

### 8.3 `--param-exclude=REGEX`

按正则排除参数，例如排除令牌和会话字段：

```bash
--param-exclude='(?i)token|session|csrf'
```

在运行前验证正则，避免误排除真正目标。

### 8.4 `--param-filter=PLACE`

限制参数位置，例如 `GET`、`POST`、`COOKIE` 或 Header 类别。具体可用值以当前版本 `-hh` 为准。它比盲目提高 `--level` 更容易控制范围。

### 8.5 `--skip-static`

跳过看起来不会影响响应的参数，可减少请求数。但“响应暂时不变”不等于参数绝对安全，代码审计确认的高风险参数应显式用 `-p` 测试。

## 9. 检测强度：`--level` 与 `--risk`

### 9.1 `--level=1..5`

`level` 控制测试数量、边界和参数位置：

- `1`：默认，优先测试 GET/POST 参数和常见 payload；
- `2`：测试范围扩大，通常也会考虑 Cookie；
- `3`：进一步考虑 `User-Agent`、`Referer` 等位置；
- `4`、`5`：加入更多 payload 和边界组合，请求数量显著增加。

更高不是更准确的同义词。最佳做法是先用 `-p` 和源码定位，再从 `1` 逐级提高。

### 9.2 `--risk=1..3`

`risk` 控制测试本身的潜在影响：

- `1`：默认、较温和；
- `2`：可能加入更重的时间型测试；
- `3`：可能加入基于 `OR` 等更具影响的测试，在 `UPDATE`、`DELETE` 等语句上下文中可能影响多行。

对任何可能修改数据的接口，未经明确授权不要使用高风险等级。即使是授权测试，也应使用快照、测试数据和事务保护。

## 10. 限定数据库和操作系统

### 10.1 `--dbms=DBMS`

```bash
--dbms=SQLite
--dbms=MySQL
--dbms=PostgreSQL
```

已从源码、架构文档或可靠错误信息确认数据库时，可以减少无关测试。错误指定会导致漏报；不确定就让工具指纹识别。

### 10.2 `--os=OS`

指定数据库服务器操作系统，主要影响更高风险的系统相关能力。普通注入检测不需要设置，本教程也不进行操作系统接管。

## 11. 选择注入技术：`--technique`

可组合的字母：

| 字母 | 技术 | 主要观察 |
|---|---|---|
| `B` | Boolean-based blind | 真/假响应差异 |
| `E` | Error-based | 数据库错误内容 |
| `U` | UNION query-based | 联合结果出现在响应 |
| `S` | Stacked queries | 是否允许堆叠语句 |
| `T` | Time-based blind | 条件延迟 |
| `Q` | Inline queries | 在原表达式中嵌入查询 |

示例：

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  -p id --technique=BU --level=1 --risk=1
```

如果已经通过手工真假对照确认布尔差异，可只用 `B`，减少请求。堆叠和时间型测试更可能带来副作用或负载，不应作为第一选择。

## 12. 帮助 sqlmap 识别真假响应

动态页面可能使默认页面比较失效，此时使用稳定特征。

### 12.1 `--string=TEXT`

指定只在真响应出现的文字：

```bash
sqlmap -u 'http://127.0.0.1:8765/blind?id=1' \
  -p id --technique=B --string='Found' --batch
```

选择业务稳定、不会同时出现在假响应中的文字。

### 12.2 `--not-string=TEXT`

指定假响应特征：

```bash
--not-string='Not found'
```

通常在真响应没有稳定标记、假响应有固定错误提示时使用。

### 12.3 `--regexp=REGEX`

用正则匹配真响应。适合文字中包含少量动态值的情况，但正则应足够具体。

### 12.4 `--code=STATUS_CODE`

当真假条件稳定对应不同 HTTP 状态码时使用，例如真为 `200`、假为 `404`。先手工重复验证，避免把限流 `429` 或服务错误 `500` 当作真假通道。

### 12.5 `--text-only`

只比较页面文本，忽略标签结构变化。适合 HTML 中存在动态属性或布局噪声的页面。

### 12.6 `--titles`

只比较 HTML `<title>`。只有标题能稳定区分真假时才使用。

## 13. 时间型检测参数

### 13.1 `--time-sec=SECONDS`

```bash
sqlmap -u 'http://127.0.0.1:8080/time?id=1' \
  -p id --technique=T --time-sec=2 --threads=1
```

设置时间盲注条件为真时的延迟基准。值太小容易被网络抖动淹没，太大会让每次判断很慢并增加资源占用。先手工测量正常延迟，再选择合理值。

### 13.2 `--unstable`

提示工具目标响应不稳定，启用更保守的时间统计。它不是修复不稳定环境的按钮；应先减少并发、排除限流和后台任务。

## 14. 联合查询相关参数

### 14.1 `--union-cols=RANGE`

已知可能的列数范围时进行限制：

```bash
--technique=U --union-cols=1-5
```

本地 `/product` 已知是三列，可写 `--union-cols=3`。源码已知时不必让工具盲试大量列数。

### 14.2 `--union-char=CHAR`

设置联合查询中用于列类型探测的字符，默认策略通常足够。只有默认 `NULL` 等测试被应用特殊处理、且你理解列类型时才调整。

### 14.3 `--union-from=TABLE`

某些数据库要求 `SELECT` 必须带 `FROM`，可指定一个合法表来源。它依赖数据库方言，不应凭猜测设置。

## 15. 前缀、后缀和边界

### 15.1 `--prefix=PREFIX`

### 15.2 `--suffix=SUFFIX`

当源码已经明确输入外有括号、引号或其他固定结构时，可告诉 sqlmap 使用特定前后缀：

```bash
--prefix="')" --suffix='-- '
```

这类参数非常依赖完整 SQL 上下文。随便复制前后缀通常只会制造语法错误；先在纸面上还原最终 SQL，再在本地使用。

## 16. 指纹与基本信息

### 16.1 `-f` / `--fingerprint`

执行更广泛的 DBMS 指纹识别。请求会增加，普通检测已经确定数据库时通常不需要。

### 16.2 `-b` / `--banner`

查询数据库版本 banner：

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' -p id -b
```

版本信息可能帮助修复和资产清点，但也属于环境信息。授权测试报告应只保存必要内容。

### 16.3 `--current-user`、`--current-db`、`--hostname`

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  -p id --current-user --current-db
```

- `--current-user`：数据库连接使用的身份；
- `--current-db`：当前数据库或 schema 上下文；
- `--hostname`：数据库主机名，SQLite 等本地文件数据库含义不同。

这些信息用于判断最小权限和修复范围，不应扩大到未授权基础设施。

### 16.4 `--is-dba`

判断当前数据库用户是否具有数据库管理员级权限。若结果为真，应把“应用使用过高权限账号”作为独立高风险问题修复，而不是继续尝试高权限功能。

## 17. 枚举数据库结构

以下参数只用于本地训练库或授权方明确允许的数据验证。

### 17.1 `--dbs`

列出可见数据库：

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' -p id --dbs
```

SQLite 通常表现为 `main` 等数据库名；服务器型 DBMS 会根据当前账号权限显示不同范围。

### 17.2 `-D DB` 与 `--tables`

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  -p id -D main --tables
```

`-D` 选择数据库，`--tables` 列出其中可见表。不要用 `--tables` 证明漏洞存在；真假对照已经足够时，结构枚举必须有额外授权理由。

### 17.3 `-T TABLE` 与 `--columns`

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  -p id -D main -T products --columns
```

`-T` 选择表，`--columns` 查看列名和类型。

### 17.4 `--schema`

尝试枚举完整数据库模式。范围通常比 `--tables` 和 `--columns` 大，可能产生很多请求；大型系统不应在初次验证中使用。

### 17.5 `--count`

获取表的行数：

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  -p id -D main -T products --count
```

行数也可能是敏感业务信息，真实授权中需要明确范围。

### 17.6 `--search`

按名称搜索数据库、表或列。通常结合 `-D`、`-T`、`-C` 模式使用。它可能跨多个 schema 产生大量元数据请求，优先从架构文档获取已知名称。

### 17.7 `--exclude-sysdbs`

排除系统数据库，减少无关枚举。在授权验证中通常应开启这一思路，只关注应用数据库。

## 18. 有限数据验证

### 18.1 `-C COLUMNS` 与 `--dump`

仅对本地虚构训练表：

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  -p id \
  -D main \
  -T products \
  -C id,name,training_flag \
  --dump \
  --start=1 \
  --stop=1
```

- `-C` 限制列；
- `--dump` 读取选定数据；
- `--start`、`--stop` 限制行范围。

在真实授权测试中，优先让系统所有者创建单独的 canary/test 行，或只验证固定非敏感值。不要下载密码哈希、会话令牌、支付数据或个人信息作为“漏洞证明”。

### 18.2 `--where=CONDITION`

进一步限制行：

```bash
--where='id=1'
```

条件本身应由测试者写成固定 SQL，不能复制不可信输入。不同 DBMS 的标识符和字符串语法可能不同。

### 18.3 `--first` 与 `--last`

限制盲注取得文本的字符位置，适合验证一个短训练标记而不是完整读取：

```bash
--first=1 --last=4
```

### 18.4 `--dump-all`

读取所有可见数据库中的大量数据。它通常超出“确认 SQL 注入”的必要范围，本教程不使用。即使目标属于自己，也应先评估日志、隐私和备份影响。

### 18.5 `--users`、`--roles`、`--privileges`

- `--users`：列出数据库用户；
- `--roles`：列出角色；
- `--privileges`：列出权限。

它们适合在授权配置审计中验证最小权限。`--passwords` 会尝试取得数据库账号密码哈希，涉及敏感认证材料，不应作为普通漏洞验证步骤，本教程不提供使用示例。

## 19. 执行自定义 SQL

### 19.1 `--sql-query=QUERY`

在本地训练库中执行单个只读查询：

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  -p id --sql-query='SELECT COUNT(*) FROM products'
```

仅使用明确的只读语句。不要把生产系统当数据库控制台。

### 19.2 `--sql-shell`

提供交互式 SQL 控制台，容易使测试范围从“验证漏洞”扩大为任意数据库操作。本教程不使用；需要数据库调试时，优先让所有者提供隔离的只读测试账号和标准数据库客户端。

### 19.3 `--sql-file=FILE`

从文件执行多条 SQL，同样容易造成大范围读取或修改。普通注入测试不需要。

## 20. 会话、缓存和重新测试

### 20.1 `-s SESSION_FILE`

指定会话文件，便于可重复测试。会话可能包含目标结构和响应信息，应妥善保护。

### 20.2 `--flush-session`

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  -p id --flush-session
```

清除当前目标保存的检测会话并从头开始。修复后复测或参数变化较大时使用。不要在每次运行都清除，否则会重复产生大量请求。

### 20.3 `--fresh-queries`

忽略会话中已缓存的查询结果，重新取值，但不一定重做全部检测。适合实验数据发生变化时使用。

### 20.4 `--offline`

只使用已有会话数据，不向目标发送新请求。适合查看保存结果或写报告。

## 21. 输出和审计记录

### 21.1 `--output-dir=PATH`

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  --output-dir='./sqlmap-lab-output'
```

把结果放到明确目录。输出可能包含目标信息和数据，实验结束后按项目数据处理规则保留或删除。

### 21.2 `-t TRAFFIC_FILE`

记录 HTTP 流量：

```bash
-t ./sqlmap-http-traffic.log
```

适合复核工具做了哪些请求。文件可能含 Cookie 和响应数据。

### 21.3 `--har=FILE`

输出浏览器和代理常用的 HAR 网络记录，便于分析时序。HAR 同样可能含认证头和正文。

### 21.4 `--results-file=FILE`

为多个测试目标指定汇总结果文件。单目标教学通常不需要。

### 21.5 `--dump-format=FORMAT`

控制本地训练数据的输出格式，例如 CSV、HTML 或 SQLite，具体支持值以版本帮助为准。格式只影响保存方式，不扩大读取授权。

## 22. 自动回答和批处理

### 22.1 `--batch`

使用默认答案，适合可重复的本地命令和 CI 靶场测试。默认答案不一定适合所有授权环境，第一次运行建议阅读交互问题。

### 22.2 `--answers=QUESTION=ANSWER`

为特定问题预设答案，比完全 `--batch` 更精确。匹配规则随版本变化，应查看 `-hh` 和实际提示。

### 22.3 `--wizard`

交互式向导，适合第一次了解参数，但它不能替代对目标范围和请求副作用的判断。

## 23. 动态会话与 CSRF

### 23.1 `--csrf-token=NAME`

告诉 sqlmap 哪个参数或 Header 是 CSRF Token，工具会尝试刷新并替换它。

### 23.2 `--csrf-url=URL`

指定从哪个授权页面取得新 Token。该 URL 也必须在测试范围内。

### 23.3 `--csrf-method` 与 `--csrf-data`

Token 获取请求不是普通 GET 时，指定方法和正文。复杂认证流程通常更适合使用测试账号、稳定 API 或经过审阅的预处理脚本。

### 23.4 `--safe-url`、`--safe-post`、`--safe-req`、`--safe-freq`

某些实验应用需要定期访问安全页面保持会话：

- `--safe-url`：定期访问的 URL；
- `--safe-post`：该 URL 的 POST 数据；
- `--safe-req`：从原始请求文件读取安全请求；
- `--safe-freq`：每多少次测试请求发送一次安全请求。

这些参数用于维持合法测试会话，不代表可以绕过认证或扩大账号权限。

## 24. 表单和爬取

### 24.1 `--forms`

解析目标页面中的 HTML 表单并询问是否测试。它可能发现多个输入点，但 JavaScript 动态表单支持有限。入门阶段更推荐手工选定请求和 `-p`。

### 24.2 `--crawl=DEPTH`

从入口爬取链接并测试参数。它会快速扩大请求和目标集合，很容易访问注销、删除或跨域链接。只有授权范围、排除规则和测试数据都明确时才考虑。

### 24.3 `--scope=REGEX`

用正则限制爬取或代理日志中的目标：

```bash
--scope='^http://127\.0\.0\.1:8080/'
```

使用爬取前必须先设置严格 scope，并手工检查将要测试的 URL。

## 25. 编码和页面解析

### 25.1 `--encoding=ENCODING`

指定页面字符编码，例如 `UTF-8`。只有自动检测错误时才设置。

### 25.2 `--charset=CHARSET`

限制盲注推断字符集合，可以减少本地训练值的请求数。错误限制会得到截断或错误结果。

### 25.3 `--hex`

使用十六进制函数检索非文本或编码异常数据。普通验证不需要，且会增加复杂度。

### 25.4 `--parse-errors`

尝试从响应中解析并显示数据库错误。适合自有环境排障，但日志中可能包含 SQL 和数据。

### 25.5 `--base64=PARAMETERS`

告诉工具某些参数值使用 Base64 编码，使其在测试前后正确解码/编码。Base64 只是编码，不是安全措施。

## 26. `--tamper` 的正确理解

tamper 脚本会在发送前转换 payload，例如改变空白、大小写、运算符或编码形式。其常见教学用途是验证“关键字黑名单或只匹配固定文本”的过滤器为什么不可靠。

查看当前版本提供的脚本：

```bash
sqlmap --list-tampers
```

指定脚本的一般格式：

```text
--tamper=script_name
```

使用原则：

- 只在自建的薄弱过滤器实验或明确授权的 WAF 验证中使用；
- 先阅读脚本源码，理解它改变了什么；
- 一次只引入一个变量，保留原始请求和变换后请求；
- 不能把“某脚本通过 WAF”当作后端一定存在 SQL 注入；
- 不要堆叠一串来源不明的脚本，转换之间可能互相破坏；
- 真正修复仍是参数化、白名单和最小权限，而不是添加更多正则。

第四章会用防守视角分析规范化和黑名单失效。教程不提供针对第三方防护产品的规避配方。

## 27. 本教程不实操的高风险能力

sqlmap 还存在文件读取/写入、用户定义函数注入、操作系统命令、注册表、数据库进程接管和带外通信等参数。它们会把一个查询漏洞升级为主机或数据层面的实际控制，远超确认和修复 SQL 注入所需范围。

本教程不提供以下能力的命令示例：

- 操作系统 Shell 或命令执行；
- 上传、写入或覆盖服务器文件；
- 读取服务器任意文件；
- 注册表读取或修改；
- UDF/共享库注入；
- 带外数据传输；
- 尝试破解数据库密码哈希；
- 删除、修改或批量导出真实数据。

在专业测试中，这些也必须有单独、明确的授权、隔离环境、备份和恢复方案，不能由“允许做 SQL 注入测试”自动推出。

## 28. 推荐的授权测试流程

### 阶段一：准备

1. 确认域名、IP、路径、账号、时间窗口和禁止操作；
2. 获取专用测试账号和测试数据；
3. 明确最大请求速率、并发和故障联系人；
4. 保存一条正常请求并手工验证；
5. 开启应用与数据库日志。

### 阶段二：最小检测

```bash
sqlmap -r request.txt \
  -p id \
  --level=1 \
  --risk=1 \
  --threads=1 \
  --batch \
  -t traffic.log
```

### 阶段三：确认

根据初步结果只保留一种可靠技术，设置 `--string`、`--not-string` 或 `--code`，重复验证真假条件，并与服务端日志对照。

### 阶段四：有限影响证明

优先查询当前数据库用户、当前数据库和授权方准备的测试标记；不要默认执行全库枚举或数据导出。

### 阶段五：修复和复测

清除旧会话后使用完全相同的请求复测：

```bash
sqlmap -r fixed-request.txt \
  -p id \
  --flush-session \
  --level=1 \
  --risk=1 \
  --threads=1 \
  --batch
```

再检查正常业务输入仍能工作，避免只关注安全 payload 导致功能回归。

## 29. 常见问题排查

### “参数似乎不影响页面”

- 手工修改正常值，确认参数确实参与请求；
- 检查缓存和 CDN；
- 使用 `-p` 明确目标；
- 查看原始请求中的参数是否被编码或嵌套；
- 从源码确认输入是否进入数据库。

### “页面动态导致比较失败”

- 找稳定的真/假文字，设置 `--string` 或 `--not-string`；
- 只比较文本或标题；
- 降低线程；
- 排除时间戳、随机数、Token 和推荐内容；
- 在自有环境建立专用、稳定的测试响应。

### “工具说有 WAF”

WAF 启发式提示可能来自统一错误页、限流或代理。先检查状态码和原始请求，不要立刻寻找规避脚本。后端参数化后，即使 WAF 关闭也应安全。

### “手工可复现，sqlmap 检测不到”

- 用 `-r` 保留完整请求；
- 明确 `-p`、`--dbms` 和可用 `--technique`；
- 设置真假标记；
- 检查 CSRF 和登录状态；
- 查看 `-v 4` 的实际请求；
- 不要一上来提高所有强度参数。

### “修复后仍报告可注入”

- 使用 `--flush-session`；
- 确认请求确实到达新版本；
- 检查其他同名参数或重复参数；
- 对照数据库日志，确认绑定参数；
- 排除业务层真假差异造成的误报。

## 30. 常用命令模板

本地 GET 参数：

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  -p id --technique=BU --level=1 --risk=1 --threads=1 --batch
```

原始请求文件：

```bash
sqlmap -r request.txt \
  -p q --level=1 --risk=1 --threads=1 --batch -t traffic.log
```

稳定布尔标记：

```bash
sqlmap -u 'http://127.0.0.1:8765/blind?id=1' \
  -p id --technique=B --string='Found' --threads=1 --batch
```

本地训练表的单行有限验证：

```bash
sqlmap -u 'http://127.0.0.1:8765/product?id=1' \
  -p id -D main -T products -C id,name,training_flag \
  --dump --start=1 --stop=1 --threads=1 --batch
```

修复后重新检测：

```bash
sqlmap -u 'http://127.0.0.1:8765/safe/product?id=1' \
  -p id --flush-session --level=1 --risk=1 --threads=1 --batch
```

## 31. 本章练习

1. 阅读 `sqlmap -hh`，把本机版本中与教程名称不同的参数记录下来。
2. 对 `/product` 只启用 `B`，再只启用 `U`，比较请求数量和结论。
3. 对 `/blind` 分别使用默认比较与 `--string='Found'`，观察稳定性。
4. 通过本地调试代理查看 `-v 3` 所示 payload 与实际 HTTP 请求的关系。
5. 使用 `--output-dir` 和 `-t` 保存一次本地实验，检查其中是否含敏感字段。
6. 用 `--start=1 --stop=1` 只验证训练表第一行，解释为何“最少数据”足以证明影响。
7. 对 `/safe/product` 使用 `--flush-session` 复测，确认工具不再复用漏洞路由结论。
8. 为一次虚构授权测试写出范围、请求速率、允许的枚举动作和明确禁止的高风险动作。

