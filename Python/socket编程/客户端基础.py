import socket as sk

try:
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s: #表示IPv4地址，TCP协议
        s.connect(("127.0.0.1", 1234)) #“127.0.0.1”是本地地址，即“localhost”，1234是服务器端口号
        print("已连接到服务器")
        while True:
            message = input("请输入消息（输入exit退出）：")
            if message.lower() == "exit": #如果输入exit，退出循环
                print("正在关闭连接...")
                break
            #! 注意：消息是字节序列，不是字符串，所以需要编码和解码
            # 如果是硬编码字符串，要写b"..."，表示字节数据
            # 如果是变量，要调用encode方法将字符串编码为字节数据
            s.sendall(message.encode()) #将字符串编码为字节数据发送给服务器
            data = s.recv(1024) #接收服务器返回的数据，最多1024字节
            print("收到服务器回复：", data.decode()) #解码字节数据为字符串并打印
        
        print("连接已关闭")
        
except ConnectionRefusedError:
    print("无法连接到服务器")