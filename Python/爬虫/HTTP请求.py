
#_ HTTP请求组成部分
# 1. 请求行：包含请求方法、URL和HTTP版本
# 2. 请求头：包含一些键值对，描述请求的属性和客户端
# 3. 请求体：包含一些数据，通常用于POST、PUT等方法，发送给服务器

#_ HTTP请求方法
# 1. GET：请求获取资源，通常用于查询数据（爬虫最常用）
# 2. POST：向服务器提交数据，通常用于创建资源
# 3. PUT：更新资源，通常用于替换整个资源
# 4. DELETE：删除资源

#_ HTTP状态码
# 1xx：信息性状态码，表示请求已被接收，继续处理
# 2xx：成功状态码，表示请求已成功处理
# 3xx：重定向状态码，表示需要进一步操作才能完成请求 
# 4xx：客户端错误状态码，表示请求有语法错误或无法完成
# 5xx：服务器错误状态码，表示服务器未能完成合法的请求

#_ HTTP请求库
# requests库：一个简单易用的HTTP库，支持发送各种HTTP请求，处理响应数据，管理会话等功能
# urllib库：Python内置的HTTP库，提供了一些函数和类来处理URL、发送HTTP请求、解析响应等功能，但使用起来相对较复杂

# 示例
import requests

url = 'http://books.toscrape.com/' 
response = requests.get(url)  # 发送GET请求

if response.ok:
    op = input("请求成功，是否继续？（y/n）")
    if op.lower() == 'y':
        print(response.text)  # 打印网页内容
    else:
        print("操作已取消。")
else:
    print("请求失败，状态码：", response.status_code)

#_ HTTP响应组成部分
# 1. 状态行：包含HTTP版本、状态码和状态消息
# 2. 响应头：包含一些键值对，描述响应的属性和服务器
# 3. 响应体：包含一些数据，通常是服务器返回给客户端的内容

#_ HTTP状态码示例
# 200 OK：请求成功，服务器返回了请求的资源
# 301 Moved Permanently：请求的资源已被永久移动到新URL
# 400 Bad Request：请求有语法错误，服务器无法理解
# 401 Unauthorized：请求需要身份验证，客户端未提供有效的凭据
# 403 Forbidden：服务器拒绝请求，客户端没有权限访问资源
# 404 Not Found：请求的资源不存在
# 418 I'm a teapot：服务器拒绝请求，因为它是一个茶壶（愚人节彩蛋状态码）
# 500 Internal Server Error：服务器发生错误，无法完成请求





