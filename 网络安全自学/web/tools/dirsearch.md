# dirsearch
基于 Python 的 Web 目录爆破工具

## 用法
### 最简用法
```bash
dirsearch -u <url>
```

### 常规用法
```bash
dirsearch -i 200 -t 20 -u <url>
```

## 参数
- `-u <url>`: 指定目标 URL
  
- `-e <extensions>`: 指定文件扩展名
  
- `-w <wordlist>`: 指定字典文件
  
- `-t <threads>`: 指定线程数，默认 10

- `-x <status_codes>`: 指定要排除的 HTTP 状态码

- `-i <status_codes>`: 指定要包含的 HTTP 状态码

- `-r`: 启用递归扫描

> 可多选的参数大多逗号分隔，支持范围
