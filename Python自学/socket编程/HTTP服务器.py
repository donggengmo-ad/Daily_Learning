import socket as sk
import threading as th
import os

def handle_client(conn, addr):
    print("连接来自：", addr)
    with conn:
        request = conn.recv(1024) #接收HTTP请求
        
        #解析HTTP请求，提取请求行和请求头
        headers = request.split(b"\r\n") #HTTP请求以\r\n分隔行
        file = headers[0].split()[1].decode() #请求行的第二部分是请求的文件路径，解码为字符串
        
        #构造HTTP响应
        if file == "/": #如果请求根路径，返回index.html
            # 这里就不放文件了，直接返回404
            file = "./index.html"
            
        try:
            #读取文件内容
            with open(WEBROOT + file ,"rb") as f:
                content = f.read()
            #构造HTTP响应头
            response = b"HTTP/1.1 200 OK\r\n\r\n" + content
            
        except FileNotFoundError:
            #如果文件不存在，返回404错误
            response = b"HTTP/1.1 404 Not Found\r\n\r\nFile Not Found"
        
        #发送HTTP响应
        conn.sendall(response) 
        
if __name__ == "__main__":
    
    #定义webroot目录，存放HTML文件
    WEBROOT = "webroot"
    
    #切换到当前脚本所在目录，确保能找到webroot文件夹
    os.chdir(os.path.dirname(os.path.abspath(__file__))) 
            
    #创建socket对象，绑定地址和端口，监听连接
    with sk.socket(sk.AF_INET, sk.SOCK_STREAM) as s:
        s.bind(("",80)) #HTTP默认端口是80，绑定到所有可用的网络接口
        s.listen()
        print("服务器已启动，等待连接...")
        
        while True:
            conn,addr = s.accept()
            t = th.Thread(target=handle_client, args=(conn,addr))
            t.start()
            print("当前线程数：", th.active_count())
        