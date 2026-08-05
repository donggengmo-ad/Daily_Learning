# Linux 命令
[TOC]

## 快捷键
- <kbd>Ctrl</kbd>+<kbd>C</kbd>: 终止当前命令
- <kbd>Ctrl</kbd>+<kbd>Z</kbd>: 暂停当前命令
- <kbd>Up</kbd>/<kbd>Down</kbd>: 历史命令切换
- <kbd>Tab</kbd>: 自动补全命令或文件名

## 符号
### 目录符号
- `.`: 当前目录
- `..`: 上级目录
- `~`: 用户主目录
- `/`: 根目录
- `/<directory>`: 绝对路径
- `<directory>`: 相对路径

### 字符集表示
- `a-z`: 表示字符范围
- `a-zA-Z`: 范围组合
- `'[<POSIX character class>]'`: POSIX 字符类
  - `[:alnum:]`: 字母和数字
  - `[:alpha:]`: 字母
  - `[:digit:]`: 数字
  - `[:lower:]`: 小写字母
  - `[:upper:]`: 大写字母
  - `[:space:]`: 空白字符（空格、制表符、换行符等）
  - `[:punct:]`: 标点符号

### 正则表达式
- `.`: 匹配任意单个字符
- `?`: 匹配前一个字符零次或一次
- `*`: 匹配前一个字符零次或多次
- `[abc]`: 匹配方括号内任意一个字符
- `[a-z]`: 匹配方括号内任意一个字符范围
- `+`: 匹配前一个字符一次或多次
- `\`: 转义字符
  >[!TIP] 匹配任意字符
  > - `.*?`: 非贪婪，匹配尽可能少的字符
  > - `.*`: 贪婪，匹配尽可能多的字符

### 重定向符号
- `>`: 输出重定向，覆盖文件内容
- `>>`: 输出重定向，追加文件内容
- `<`: 输入重定向，从文件读取输入
  
### 文件描述符
- `0`: 标准输入（stdin）
- `1`: 标准输出（stdout）
- `2`: 标准错误（stderr）
  > [!TIP] 常用 
  > - `> file`: 默认重定向标准输出
  > - `> file 2>&1`: 标准输出、错误到同一文件（简化为 `&> file`）
  > - `2>/dev/null`: 丢弃错误输出

### 命令连接符
- `|`: 管道符，将前一个命令的输出作为下一个命令
- `;`: 分隔符，顺序执行多个命令
- `&&`: 前一个命令成功时执行下一个命令
- `||`: 前一个命令失败时执行下一个命令
- `&`: 后台运行命令

## 终端
### 作业管理
- `jobs`: 显示当前用户的作业列表
  - `-l`: 显示作业的详细信息
  - `-p`: 仅显示作业的进程 ID
  
- `fg <job_id>`: 将作业调到前台运行（默认最近的作业）
- `bg <job_id>`: 将作业调到后台运行

### 复用

- `tmux`: 使用 [tmux](./tools/tmux.md) 终端复用

### 显示

- `echo <text>`: 输出文本到终端
  - `-n`: 不换行输出

- `history`: 显示命令历史

- `clear`: 清屏

- `reset`: 重置终端

### 命令

- `man <command>`: 查看命令的手册页

- `alias <name>='<command>'`: 创建命令别名
  - `unalias <name>`: 删除命令别名

- `which <command>`: 显示命令的完整路径

- `whereis <command>`: 显示命令的源代码、手册等相关路径

- `whatis <command>`: 显示命令的简介

## 文件操作
### 目录导航
- `pwd`: 显示当前工作目录

- `cd <directory>`: 切换目录
  
- `ls`: 列出目录内容
  - `-l`: 以长格式列出目录内容
  - `-h`: 以人类可读的格式显示文件大小
  - `-a`: 列出所有文件，包括隐藏文件
  - `-R`: 递归列出目录内容 

- `tree`: 以树状结构显示目录内容
  - `-L <level>`: 限制显示的目录层级
  - `-a`: 显示所有文件，包括隐藏文件
  - `-d`: 仅显示目录，不显示文件

- `mkdir <directory>`: 创建目录
  - `-p`: 创建多级目录
  - `-m <mode>`: 设置目录权限
  - `-v`: 显示创建的目录

- `rmdir <directory>`: 删除空目录
  - `-p`: 删除多级空目录
  - `-v`: 显示删除的目录

- `mktemp`: 创建临时文件
  - `-d`: 创建临时目录

### 文件查看
- `cat <filename>`: 文件内容
  - `-n`: 显示行号

- `less <filename>`: 分页查看文件内容
  - 快捷键类似 [vim](./tools/vim.md)
  - `q`: 退出

- `more <filename>`: 分页，只能向下翻页
  - `q`: 退出
  - `space`: 向下翻页
  - `v`: 使用 vi

- `file <filename>`: 一个或多个文件类型

- `stat <filename>`: 详细信息

- `head <filename>`: 前几行
  - `-n <number>`: 显示前几行，默认 10

- `tail <filename>`: 后几行
  - `-n <number>`: 显示最后几行，默认 10
  - `-f`: 实时追加内容变化（看日志常用）

- `find <directory>`: 指定目录下查找文件
  - `-name <filename>`: 按文件名查找
  - `-iname <filename>`: 按文件名查找，忽略大小写
  - `-type <filetype>`: 按文件类型查找
    - `f`: 普通文件
    - `d`: 目录
    - `l`: 符号链接
  - `-size <size>`: 按文件大小查找
    - `+<size>`: 大于指定大小
    - `-<size>`: 小于指定大小
    - `<size>c`: 指定大小，单位为字节
    - `<size>k`: 指定大小，单位为千字节
    - `<size>M`: 指定大小，单位为兆字节
    - `<size>G`: 指定大小，单位为千兆字节
  - `-user <username>`: 按文件所有者查找
  - `-group <groupname>`: 按文件所属组查找

- `<path>/<filename>`: 运行可执行文件（必须有路径）

### 文件管理
- `touch <filename>`: 创建空文件或更新时间戳
  - `-c`: 不创建新文件，只更新时间戳
  - `-a`: 仅更新访问时间
  - `-m`: 仅更新修改时间
 
- `rm <filename>`: 删除文件
  - `-f`: 强制删除，不提示
  - `-i`: 提示确认删除
  - `-r`: 递归删除目录及其内容
  - `-v`: 显示删除的文件

- `cp <source> <destination>`: 复制文件
  - `-r`: 递归复制目录及其内容
  - `-p`: 保留文件属性（权限、时间戳等）

- `mv <source> <destination>`: 移动或重命名文件
  - `-i`: 提示确认覆盖
  - `-f`: 强制覆盖，不提示
  - `-u`: 仅在源文件比目标文件新时才移动
  - `-v`: 显示移动的文件

- `vim <filename>`: 使用[Vim](./tools/vim.md)编辑文件

> [!NOTE] 特殊文件名
> - `--`: 表示后续参数为文件名而非选项
> - `'<filename>'`: 引号包裹防展开
> - `\<special character>`: 转义特殊字符
> - `./<filename>`: 使用完整路径，覆盖开头特殊字符

### 压缩
- `tar`: 归档（打包但不压缩）
  - `-c`: 创建新的归档文件
  - `-x`: 解压归档文件
  - `-t`: 列出归档文件内容
  - `-f <filename>`: 指定归档文件名
  - `-v`: 显示详细信息
  - `-z`: 使用 gzip 压缩或解压
  - `-j`: 使用 bzip2 压缩或解压
  > [!TIP] 常见联用
  > - `tar -czf <file> <directory>`: 创建 gzip 压缩的归档文件
  > - `tar -xzf <file>`: 解压 gzip 压缩的归档文件
  > - `tar -tzf <file>`: 列出 gzip 压缩的归档文件内容

- `gzip <filename>`: 压缩文件，生成 `<filename>.gz`
  - `-d`: 解压缩（等同于 `gunzip`）
  - `-c`: 将压缩结果输出到标准输出，而不是生成文件
  - `-k`: 保留原文件（默认删除）

- `bzip2 <filename>`: 用法同 `gzip`，生成 `<filename>.bz2`
  > 压缩效率高，但速度慢

## 文本处理
### 字符操作
- `grep <pattern> <filename>`: 查找文件内容
  - `-i`: 忽略大小写
  - `-v`: 反向匹配（显示不匹配的行）
  - `-r`: 递归查找
  > 常配合管道: `command | grep <pattern>`

- `awk <pattern> <filename>`: 文本处理工具
  - `'{print $n}`: 打印第 n 列，默认空格分隔
  - `-F`: 指定字段分隔符（可正则）

- `tr <set1> <set2>`: 替换字符集
  - `-d <set>`: 删除字符集中的字符
  - `-s <set>`: 压缩重复字符为单个字符
  - `-c <set>`: 取反字符集
  > [!TIP] 常用配合:
  > - `tr -sc <set> '\n'`: 将字符集以外的字符替换为换行

- `sed <script> <filename>`: 流编辑器
  - `s/<pattern>/<replacement>/`: 替换匹配的文本
  - `-i`: 直接修改文件内容

- `cut`: 按列提取文本
  - `-d <delimiter>`: 指定分隔符，默认制表符
  - `-f <fields>`: 指定要提取的字段（列），单个或多个

### 文本文件
- `strings <filename>`: 提取文件中的可打印字符串
  - `-n <number>`: 设置最小字符串长度，默认 4
  - `-e <encoding>`: 设置字符串编码，默认 ASCII

- `sort <filename>`: 按行排序文件内容（字典序升序）
  - `-r`: 逆序排序
  - `-n`: 按数值排序
  - `-u`: 去重（相当于 `sort | uniq`）

- `uniq <filename>`: 相邻行去重
  - `-c`: 统计每行出现次数
  - `-d`: 只显示重复行
  - `-u`: 只显示不重复行
  > [!TIP] 常夹在管道中
  `cmd1 | sort | uniq | cmd2`

- `wc <filename>`: 统计文件内容
  - `-l`: 统计行数
  - `-w`: 统计单词数
  - `-c`: 统计字节数
  - `-m`: 统计字符数
  - `-L`: 统计最长行的长度

- `diff <file1> <file2>`: 比较文件内容
  - `-u`: 以统一格式显示差异
  - `-c`: 以上下文格式显示差异
  - `-y`: 并排显示差异内容
  - `-q`: 静默模式，仅显示是否不同
  - `-i`: 忽略大小写
  - `-w`: 忽略空格和制表符
  - `-r`: 递归比较目录

- `cmp <file1> <file2>`: 逐字节对比文件内容
  - `-l`: 显示不同字节的偏移量和字节值

- `rev <filename>`: 反转文件内容的每一行

### 编码
- `base64 <filename>`: 对文件内容进行 Base64 编码
  - `-d`: 解码
  - `-i`: 忽略非打印字符
  - `-w <number>`: 设置每行输出的字符数，一般改 0 使不换行

- `xxd <filename>`: 将文件内容以十六进制显示
  - `-r`: 反向操作，将十六进制转换为原始文件
  - `-p`: 纯模式，处理纯十六进制字符串，不带偏移量
  - `-l <length>`: 设置输出的字节数


## 权限管理
### 用户与组
- `whoami`: 显示当前用户

- `adduser <username>`: 添加新用户（仅 root 可用
  - `-m`: 创建用户主目录
  - `-G <group>`: 指定用户所属组

- `userdel <username>`: 删除用户（仅 root 可用）
  - `-r`: 删除用户主目录和邮件

- `passwd <username>`: 修改用户密码

- `su <username>`: 切换用户身份
  - `-c <command>`: 执行指定命令后退出

- `id`: 显示当前用户的 UID、GID 和所属组
  - `-u`: 仅显示 UID
  - `-g`: 仅显示 GID
  - `-G`: 显示所有所属组的 GID
  - `-n`: 显示用户名而非 UID/GID

- `umask <mask>`: 设置默认权限掩码
  - 权限: `rwx(u) rwx(g) rwx(o)`，1 允许，0 禁止
    > [!SUMMARY] 权限表示
    > - `r`: 读权限，`w`: 写权限，`x`: 执行权限
    > - `u`: 所有者，`g`: 所属组，`o`: 其他人
  - 掩码: `<mask>` 表示要ban的权限
  - 默认: 文件`666`，目录`777`

- `chmod <mode> <filename>`: 修改文件权限
  - `<mode>`: 权限模式，可以是数字或符号表示
    - 数字: `rwx(u) rwx(g) rwx(o)`
    - 符号: 
      - `+`: 添加权限，`-`: 移除权限，`=`: 设置权限
      - `u`: 所有者，`g`: 所属组，`o`: 其他人，`a`: 所有人
      - `r`: 读权限，`w`: 写权限，`x`: 执行权限
  - 特殊权限: `<mode>` 中首位
    - `s`(4): 文件执行时临时获得所有者权限
    - `s`(2): 
      - 文件: 执行时临时获得所属组权限
      - 目录: 新建文件继承所属组权限
    - `t`(1): 目录下的文件只能被所有者或 root 删除或重命名

- `install <source> <destination>`: 复制文件并设置权限
  - `-m <mode>`: 设置权限模式
  - `-o <owner>`: 设置所有者
  - `-g <group>`: 设置所属组
  - `-d`: 创建目录组件（类似 `mkdir -p`）


## 网络通信
### SSH
- `ssh <username>@<hostname>`: 远程登录
  - `-p <port>`: 指定端口号，默认 22
  - `-i <identity_file>`: 指定私钥文件
  - 直接追加命令执行（非交互式）: `ssh <username>@<hostname> <command>`

- `scp <source> <destination>`: 在本地和远程主机之间复制文件
  - `-P <port>`: 指定端口号，默认 22
  - `-i <identity_file>`: 指定私钥文件

### 状态检测
- `netstat`: 显示网络连接、路由表和网络接口信息
  - `-r`: 显示路由表
  - `-i`: 显示网络接口信息

- `nmap <hostname>`: 扫描主机端口
  - `-p <port>`: 指定端口号，默认扫描常用端口

- `ping <hostname>`: 测试网络连通性
  - `-c <count>`: 指定请求次数
  - `-s <size>`: 指定数据包大小（字节）
  - `-i <interval>`: 指定请求的间隔时间（秒）
  - `-f`: 洪水模式
  - `-W <timeout>`: 指定等待响应的超时时间（秒）

- `traceroute <hostname>`: 跟踪数据包经过的路由
  - `-n`: 不解析主机名，直接显示 IP 地址
  - `-m <max_hops>`: 设置最大跳数，默认 30
  - `-p <port>`: 指定目标端口，默认 33434

- `arp <option>`: 显示和修改 ARP 缓存
  - `-a`: 显示所有 ARP 条目
  - `-d <hostname>`: 删除指定主机的 ARP 条目
  - `-s <hostname> <mac_address>`: 添加静态 ARP 条目

### HTTP 请求
- `curl <url>`: 发送 HTTP 请求
  - `-X <method>`: 指定请求方法（GET、POST、PUT 等）
  - `-H <header>`: 自定义请求头
  - `-d <data>`: 请求体数据
  - `-u <user:password>`: HTTP 基本认证
  - `-o <file>`: 将响应内容保存到文件
  - `-s`: 静默模式，不显示进度条和错误信息
  - `-v`: 显示详细的请求和响应信息

- `wget <url>`: 下载文件
  - `-O <file>`: 保存为指定文件名
  - `-q`: 静默模式，不显示下载进度
  - `-c`: 断点续传
  - `-r`: 递归下载整个网站

### DNS 解析
- `host <domain>`: 查询域名解析结果

- `dig <domain>`: 查询域名解析链路
  - `-t <type>`: 指定查询类型（A、AAAA、MX等）
  - `-p <port>`: 指定 DNS 服务器端口，默认 53

### 综合工具
- `nc <hostname> <port>`: 连接到指定主机和端口
  - `-l`: 监听模式（默认连接模式）
  - `-v`: 显示详细信息
  - `-z`: 扫描端口而不发送数据
  - `-w <timeout>`: 设置超时时间（秒）
  - `-n`: 不进行 DNS 解析，直接使用 IP 地址
  - `-u`: 使用 UDP 协议（默认 TCP 协议）
  - `-k`: 保持监听（默认单次关闭）

- `openssl`: 加密与通信工具
  - `s_client`: SSL/TLS 客户端
    - `-connect <hostname>:<port>`: 连接到指定主机和端口
    - `quiet`: 不显示握手信息，只显示接收数据
  - `s_server`: SSL/TLS 服务器（需先生成证书和私钥）
    - `-accept <port>`: 监听指定端口
    - `-cert <certificate_file>`: 指定证书文件
    - `-key <private_key_file>`: 指定私钥文件
    - `-www`: 显示简单的网页内容

## 系统管理
### 系统信息
- `uname`: 显示系统信息
  - `-a`: 显示所有信息
  - `-r`: 显示内核版本
  - `-m`: 显示系统架构

- `neofetch`: 更美观的系统信息显示


### 定时任务
- `cron`: 使用 [cron](./tools/cron.md) 定时任务

### 进程
- `top`: 实时显示系统进程和资源使用情况
  - `-u <username>`: 显示指定用户的进程
  - `-p <pid>`: 显示指定进程的详细信息
  - `-n <number>`: 显示前 N 个进程，默认 10
  - `-d <delay>`: 设置刷新间隔时间（秒），默认 3 秒

- `ps`: 显示进程信息（静态快照）
  - `-e`: 显示所有进程
  - `-f`: 显示完整格式
  - `-u <username>`: 显示指定用户的进程
  - `-p <pid>`: 显示指定进程的详细信息

## Shell 脚本
[Shell Script](./tools/shellscript.md)
