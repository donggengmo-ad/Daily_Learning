import socket

class TCP:
    def __init__(self, host: str, port: int):
        self.host = host
        self.port = port
        # AF_INET: IPv4, SOCK_STREAM: TCP
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def client(self):
        self.sock.connect((self.host, self.port)) 
        self.sock.send(b"Hello, TCP Server!") 
        data = self.sock.recv(4096)
        print(f"Received {data} from server")      

class UDP:
    def __init__(self, host: str, port: int):
        self.host = host
        self.port = port
        # AF_INET: IPv4, SOCK_DGRAM: UDP
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    def client(self):
        self.sock.sendto(b"Hello, UDP Server!", (self.host, self.port))
        data, addr = self.sock.recvfrom(4096)
        print(f"Received {data} from {addr}")
        
