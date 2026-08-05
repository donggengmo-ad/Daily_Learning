# hping3
加强版 ping，可用于端口扫描、DoS 攻击、防火墙测试等。

## 用法
### 最简用法
```bash
hping3 <target> # 通常要 sudo
```
> 我的 macOS 适配不对，需要 `sudo hping3 -I en0 <target>` 

### 端口扫描
```bash
# TCP SYN 扫描
hping3 -S -p <port> <target>
# UDP 扫描
hping3 -2 -p <port> <target>
```

### DoS 攻击
```bash
# TCP SYN 洪水攻击
hping3 -S -p <port> --flood --rand-source <target>
# ICMP 洪水攻击
hping3 -1 --flood --rand-source <target>
```

### 路由追踪
```bash
hping3 -T --ttl <value> <target>
```


## 参数
### 发包控制
- `-<0-2>`: 发包模式（默认 TCP）
  - `-0`: 原始 IP 数据包
  - `-1`: ICMP 模式
  - `-2`: UDP 模式

- `-c <count>`: 指定发包数量

- `-i <interval>`: 指定间隔时间（秒）

- `-d <size>`: 指定数据包大小（字节）

- `--flood`: 洪水模式，尽可能快地发送数据包

### TCP 标志位
- `-S`: SYN（发起连接）
- `-A`: ACK（确认应答）
- `-F`: FIN（结束连接）
- `-R`: RST（重置连接）
- `-P`: PSH（推送数据）

### 端口与地址
- `-p <port>`: 指定目标端口

- `-s <port>`: 指定源端口

- `-a <address>`: 伪造源 IP 地址

- `--rand-source`: 伪造随机源 IP 地址

- `-I <interface>`: 指定网络接口

### 路由追踪
- `-T`: 启用路由追踪

- `--ttl <value>`: 追踪跳数
