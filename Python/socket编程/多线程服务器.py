import socket as sk
import threading as th

def handle_client(conn, addr):
    #线程中代码与之前的服务器端代码相同
    print("连接来自：", addr)
    with conn: 
        while True:
            data = conn.recv(1024) 
            if not data: 
                print("客户端已关闭连接")
                break
            print("收到来自{}的数据：{}", addr, data.decode())
            conn.sendall(data)

with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s: 
    s.bind(("", 1234)) 
    s.listen() 
    print("服务器已启动，等待连接...")
    
    while True: #服务器端一直运行，等待多个客户端连接
        conn, addr = s.accept() 
        
        #为每个客户端连接创建一个新的线程，调用handle_client函数处理通信
        t = th.Thread(target=handle_client, args=(conn, addr))
        t.start()
        print("创建新线程，当前线程数：", th.active_count()) #输出当前活跃线程数，包含主线程和所有子线程
        
