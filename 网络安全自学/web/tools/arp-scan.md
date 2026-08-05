# ARP-scan
[TOC]

## 简介
基于 ARP 协议的网络扫描工具，快速扫描局域网内的活动主机，获取IP 地址和 MAC 地址，无法被防火墙拦截。

## 用法
```bash
arp-scan [options] [hosts]
```

### 扫描本地网段
```bash
arp-scan -I eth0 -l
```

### 指定网段简单扫描
```bash
arp-scan -I eth0 -q -x <network>/<cidr>
```

### 高速扫描（不准确）
```bash
arp-scan -I eth0 -i 500u -r 1 -N <network>
```

## 参数
### 目标选取
- `-I`: 指定网络接口
- `-l`: 扫描本地子网（从接口推断）
- `-f <file>': 从文件读取目标列表

### 输出控制
- `-q`: 安静模式，只输出 IP-MAC 对
- `-v`: 冗余模式，输出更多信息
- `-d`: 解析地址为主机名
- `-N`: 不解析地址

### 扫描控制
- `-r <tries>`: 每主机尝试次数
- `-i <interval>`: 每次扫描间隔时间（ms, 加u为微秒）

