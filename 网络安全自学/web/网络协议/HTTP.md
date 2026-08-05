# http协议
[TOC]

## 特点
- **无状态**：每次请求独立，没有记忆能力
- **无连接**：每次连接只处理一个请求，完成后立即关闭

## url
统一资源定位符（Uniform Resource Locator）标识互联网上某一资源的地址。
```
scheme://host:port/path?query#fragment
```
- **scheme**: 协议类型，http、https、ftp等
- **host**: 主机名或IP地址，资源所在的服务器
- **port**: 端口号，http默认80，https默认443
- **path**: 资源的路径，服务器上资源的位置
- **query**: 查询字符串，用于传递参数
- **fragment**: 锚点，指向资源中某元素

## 请求
```http
请求行
请求头

请求体
```
### 请求行
```
method path version
```
- **method**: 请求方法
  - `GET`: 请求指定的资源
  - `POST`: 提交数据给指定的资源
  - `PUT`: 更新指定的资源
  - `DELETE`: 删除指定的资源

- **path**: 请求的资源路径
  - `/`: 根路径
  - `/index.html`: 指定的HTML文件
  
- **version**: HTTP协议版本
  - `HTTP/1.0`: 早期版本，简单但不支持持久连接
  - `HTTP/1.1`: 支持持久连接，增加了Host头
  - `HTTP/2`: 二进制协议，支持多路复用和头部压缩
  - `HTTP/3`: 基于QUIC协议，支持更快的连接建立和更低的延迟

### 请求头
传递客户端信息和请求参数

- `Host`: 指定请求的主机名和端口号
  
- `User-Agent`: 客户端软件信息
  - 浏览器类型、版本、操作系统等
  
- `Referer`: 指定请求来源的URL

- `Connection`: 连接管理
  - `keep-alive`: 保持连接
  - `close`: 关闭连接
  
- `Accept`: 客户端可处理的内容类型
  
- `Cookie`: 客户端存储的cookie信息


### 请求体
传递请求参数和数据，在POST、PUT等方法中使用

## 响应
### 状态行
```
version status_code reason_phrase
```
- **version**: HTTP协议版本
  
- **status_code**: 响应状态码
  - `1xx`: 信息性状态码
    - `100 Continue`: 继续，客户端应继续请求
    - `101 Switching Protocols`: 切换协议
    - `102 Processing`: 处理请求
  
  - `2xx`: 成功状态码，表示请求已成功处理
    - `200 OK`: 请求成功
    - `201 Created`: 请求成功并创建了新资源
    - `202 Accepted`: 请求已接受，但尚未处理
    - `204 No Content`: 请求成功，但无内容返回
  
  - `3xx`: 重定向状态码，表示需要客户端进一步操作
    - `301 Moved Permanently`: 永久重定向
    - `302 Found`: 临时重定向
    - `304 Not Modified`: 资源未修改，可使用缓存
  
  - `4xx`: 客户端错误状态码，表示请求有语法错误或请求无法完成
    - `400 Bad Request`: 请求语法错误
    - `401 Unauthorized`: 未授权
    - `403 Forbidden`: 禁止访问
    - `404 Not Found`: 资源未找到
  
  - `5xx`: 服务器错误状态码，表示服务器未能完成合法请求
    - `500 Internal Server Error`: 服务器内部错误
    - `501 Not Implemented`: 服务器不支持请求的功能
    - `502 Bad Gateway`: 网关错误
    - `503 Service Unavailable`: 服务不可用
  
- **reason_phrase**: 简要描述状态

### 响应头
传递服务器信息和响应参数
- `Location`: 重定向地址
  
- `Set-Cookie`: 设置cookie信息

- `WWW-Authenticate`: 认证信息，提示客户端进行身份验证
  - `Basic`: 基本认证，使用用户名和密码

### 响应体
传递响应数据和内容，通常是HTML、JSON、图片等

