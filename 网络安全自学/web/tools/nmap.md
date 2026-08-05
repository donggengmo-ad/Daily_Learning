# nmap
网络扫描工具。

## 用法
### 最简用法
```bash
nmap <target>
```

### 主机发现
```bash
# Ping 网段
nmap -sn <target>/24
# 禁 ICMP Ping 时用 TCP SYN Ping
nmap -PS <target>/24
```

### 端口扫描
```bash
# TCP SYN 全端口扫描
nmap -sS -p- <target>
# 侵略性扫描
nmap -A -T5 -v <target>
```


## 参数  
### 目标指定 
- `-p`: 指定扫描端口
  - `-p-`: 扫描所有端口

- `-F`: Fast 模式，扫描前 100 个常用端口

- `--top-ports <number>`: 扫描前 N 个常用端口

### 扫描类型（`-s` 参数）
- `-sU`: UDP 扫描
- `-sV`: 服务版本扫描
- `-sT`: TCP 连接扫描（完成三次握手）
- `-sS`: TCP SYN 扫描（发完 SYN 直接发 RST）
- `-sn`: 不扫描端口（只 ping 主机）
> 一般用 `-sUV` 做 UDP 扫描，应对 UDP 服务器不响应的情况

### Ping 类型（`-P` 参数）
- `-Pn`: 不进行 Ping 扫描，直接扫描端口（躲避防火墙）
- `-PU`: UDP Ping 扫描
- `-PS`: TCP SYN Ping 扫描
- `-PA`: TCP ACK Ping 扫描
- `-PE`: ICMP Echo Ping 扫描

- `-v`: 指定详细输出等级
  - `-vv` `-vvv`

### 更多信息
- `-A`: 高级扫描（操作系统检测、版本检测、脚本扫描和 traceroute）
  
- `-O`: 操作系统检测

### 性能控制
- `-T <0-5>`: 扫描速度，0 最慢，5 最快

- `--min-rate <number>`: 最小发送速率（包/秒）

