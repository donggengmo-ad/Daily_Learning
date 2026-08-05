# Dsniff
[TOC]

## 简介
dsniff 是一个网络嗅探和中间人攻击工具集，每个子工具都有特定的功能定位。
| 子工具 | 主要功能 | 功能定位 |
|--------|----------|----------|
| `dsniff` | 主嗅探器 | 被动捕获明文认证凭证 |
| `arpspoof` | ARP 欺骗 | 劫持局域网流量 |
| `dnsspoof` | DNS 欺骗 | 篡改域名解析结果 |
| `macof` | MAC 泛洪 | 淹没交换机 MAC 地址表 |
| `tcpkill` | TCP 连接阻断 | 发送伪造 RST 包 |
| `tcpnice` | 流量整形 | 注入假拥塞信号限制连接速率 |
| `urlsnarf` | HTTP URL 记录 | 抓取所有访问的网址 |
| `webspy` | 实时 URL 嗅探 | 将目标浏览器访问的 URL 发送到本地浏览器 |
| `sshmitm` | SSH 中间人 | 劫持 SSH 连接（降级加密） |
| `webmitm` | HTTP/HTTPS 中间人 | 中继并篡改 HTTP(S) 流量 |

## dsniff
### 简介
被动监听网络流量，自动解析明文协议中的登录凭证

### 用法
```bash
dsniff <option> [expression]
```


**保存供 Wireshark 分析**
```bash
dsniff -i eth0 -w capture.pcap
```

### 参数
**监听与输入源**
- `-i <interface>`: 指定监听网络接口
- `-p <file>`: 从 pcap 文件读取数据包（离线）

**输出与显示**
- `-a`: 显示所有信息
- `-d`: 调试模式，显示详细日志
- `-w <savefile>`: 将捕获的原始数据包写入 pcap 文件
- `-N`: 将 IP 解析为域名

**高级控制**
- `-s <snaplen>`: 设置捕获长度（默认 1024 字节，完整包用 -s 65535）
- `<expression>`: BPF 过滤规则（tcpdump 格式）

## arpspoof
### 简介
持续发送伪造的 ARP 响应包，劫持目标与网关之间的流量。

### 用法
```bash
arpspoof <option> <host> #host通常是网关
```

**单向欺骗（监视出站流量）**
```bash
arpspoof -i eth0 -t <target> <gateway>
```

**双向欺骗（MITM）**
```bash
arpspoof -i eth0 -t <target> -r <gateway>
```

### 参数
- `-i <interface>`: 指定网络接口
- `-t <target>`: 指定目标主机 IP（默认整个网段）
- `-r`: 双向欺骗，劫持目标与网关的流量

## dnsspoof
### 简介
配合 ARP 欺骗劫持 DNS 请求后，伪造 DNS 响应

### 用法
```bash
dnsspoof <option> [expression]
```

**配合 ARP 欺骗伪造域名解析**
```bash
# ARPspoof 成为中间人
arpspoof -i eth0 -t <target> -r <gateway>
# 配置映射文件
echo "<IP> <domain>" > hosts.txt
# 启动 dnsspoof
dnsspoof -i eth0 -f hosts.txt
```
### 参数
- `-i <interface>`: 指定网络接口
- `-f <hostsfile>`: 指定伪造的 域名-IP 映射文件
    >文件格式：
    >```
    ><IP> <domain>
    >...
    >```
- `<expression>`: BPF 过滤规则

## macof
### 简介
向交换机高速发送大量伪造 ARP 包，引起 CAM 表溢出。

### 用法
```bash
macof <option>
```

**基础泛洪**
```bash
macof -i eth0
```

**伪装 HTTP 流量**
```bash
macof -i eth0 -s <ip> -d <ip> -y 80
```


### 参数
**发包控制**
- `-i <interface>`: 指定网络接口
- `-n <count>`: 指定发送包的数量

**地址伪造**
- `-s <src>`: 指定源 IP 地址
- `-d <dst>`: 指定目标 IP 地址
- `-e <tha>`: 指定源 MAC 地址

**端口控制**
- `-x <srcport>`: 指定 TCP 源端口
- `-y <dstport>`: 指定 TCP 目标端口

## tcpkill
### 简介
监听指定 TCP 连接，向通信双方发送 RST 包，强制终止。

### 用法
```bash
tcpkill <option> [expression]
```

**阻断目标主机的 HTTP 连接**
```bash
tcpkill -i eth0 host <target> and port 80
```

### 参数
- `-i <interface>`: 指定网络接口
- `-<1-9>`: 阻断强度
- `<expression>`: BPF 过滤规则

## urlsnarf
### 简介
被动嗅探 HTTP 流量，输出目标主机访问的所有 URL

### 用法
```bash
urlsnarf <option> [pattern [expression]]
```

**嗅探所有 URL**
```bash
urlsnarf -i eth0
```

**嗅探目标主机特定 URL**
```bash
urlsnarf -i eth0 "*.example.*" host <target>
```

### 参数
- `-i <interface>`: 指定网络接口
- `-n`: 不解析域名，直接显示 IP
- `-p`: 离线分析 pcap 文件

**匹配与过滤**
- `pattern`: URL 匹配模式（正则表达式）
- `expression`: BPF 过滤规则
- `-v`: 反向匹配


