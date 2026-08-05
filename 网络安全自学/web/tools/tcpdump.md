# tcpdump
网络抓包工具。

## 用法
```bash
tcpdump <options> <filter>
```

### 最简用法
```bash
tcpdump
```

### 传递给 Wireshark 分析
```bash
tcpdump -i <interface> -w <file>
wireshark <file>
```

## 参数
### 抓包控制
- `-i <interface>`: 指定网络接口
- `-c <count>`: 指定抓包数量

### 域名解析
- `-n`: 不解析主机名（提速）
- `-nn`: 不解析主机名和端口号

### 输出控制
- `-A`: 以 ASCII 格式输出数据包内容
- `-w <file>`: 将抓到的数据包写入文件
- `-q`: 简洁输出
- `-v`, `-vv`, `-vvv`: 输出详细程度

## 过滤器
### 协议过滤
- `tcp`, `udp`, `icmp`
- `arp`
- `ip`, `ip6`
- ...

### 类过滤
- `net <network>`: 指定网络
- `host <host>`: 指定主机
- `port <port>`: 指定端口
- `if <interface>`: 指定接口
- `proc <process>`: 指定进程名

### 方向过滤
- `src <host>`: 指定源地址
- `dst <host>`: 指定目的地址
- `dir <in|out>`: 指定方向（入站或出站）

### 逻辑运算
- `and`, `or`, `not`
- `( )`: 用于分组

