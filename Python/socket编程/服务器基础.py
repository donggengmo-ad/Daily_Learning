
## socket编程基础原理
# socket编程是指使用socket库进行网络通信的编程方式。
# socket是一个抽象层，提供了一套接口，使得程序可以通过网络进行通信。
# 
## socket编程的基础原理包括以下几个方面：
# 1. 套接字（socket）：套接字是网络通信的基本单位，它是一个抽象的概念，表示一个通信端点。
#/       TCP套接字（SOCK_STREAM）：提供面向连接的通信，保证数据的可靠传输。
#/       UDP套接字（SOCK_DGRAM）：提供无连接的通信，不保证数据的可靠传输。（但快速）
# 2. 绑定（bind）：
#/       服务器端，bind方法绑定地址和端口。
# 3. 连接（connect）：
#/       在TCP套接字中，客户端通过connect方法与服务器连接。
# 4. 监听（listen）：
#/       服务器端，listen方法监听客户端连接请求。
# 5. 接受连接（accept）：
#/       服务器端，accept方法接受客户端连接请求，并返回新的套接字对象和客户端地址。
# 6. 发送和接收数据（send和recv）：
#/       在连接建立后，客户端和服务器使用send方法发送数据，使用recv方法接收数据。
# 7. 关闭连接（close）：
#/       通信完成后，客户端和服务器都使用close方法关闭套接字，释放资源。（也可以用with语句） 

# 示例
import socket as sk

with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s: #表示IPv4地址，TCP协议
    s.bind(("", 1234)) #空字符串等价于“0.0.0.0”，表示监听所有可用的网络接口，1234是端口号
    s.listen() 
    print("服务器已启动，等待连接...")
    conn, addr = s.accept() #等待客户端连接，返回新的套接字对象和客户端地址
    # s用于监听连接，conn用于与客户端通信，二者是不同的套接字对象
    with conn: #with语句确保连接关闭
        print("连接来自：", addr)
        while True:
            data = conn.recv(1024) #接收数据， 一次性最多接收1024字节，返回字节数据
            if not data: #如果没有数据，说明客户端关闭了连接
                print("客户端已关闭连接")
                break
            print("收到数据：", data.decode()) #解码字节数据为字符串
            conn.sendall(data) #将收到的数据原样发送回客户端
