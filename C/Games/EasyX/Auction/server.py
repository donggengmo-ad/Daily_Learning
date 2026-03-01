#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
拍卖游戏Python服务器示例
用于与C++程序的socket.cpp通信
"""

import socket
import json
import threading
from typing import Dict, List, Any


class GameServer:
    def __init__(self, host: str = "127.0.0.1", port: int = 8888):
        self.host = host
        self.port = port
        self.clients: List[socket.socket] = []
        self.game_states: Dict[int, Dict[str, Any]] = {}
        self.server_socket = None
        self.running = False

    def start(self):
        """启动服务器"""
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server_socket.bind((self.host, self.port))
        self.server_socket.listen(5)
        self.running = True

        print(f"服务器已启动，监听 {self.host}:{self.port}")
        print("等待客户端连接...")

        try:
            while self.running:
                try:
                    client_socket, client_address = self.server_socket.accept()
                    print(f"新客户端连接: {client_address}")
                    self.clients.append(client_socket)

                    client_thread = threading.Thread(
                        target=self.handle_client,
                        args=(client_socket, client_address)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                except Exception as e:
                    if self.running:
                        print(f"接受连接时出错: {e}")
        except KeyboardInterrupt:
            print("\n服务器正在关闭...")
        finally:
            self.stop()

    def handle_client(self, client_socket: socket.socket, client_address: tuple):
        """处理单个客户端连接"""
        buffer = ""
        try:
            while self.running:
                data = client_socket.recv(8192)
                if not data:
                    break

                buffer += data.decode('utf-8', errors='ignore')

                while '\n' in buffer:
                    line, buffer = buffer.split('\n', 1)
                    line = line.strip()
                    if line:
                        try:
                            parsed_data = json.loads(line)
                            self.process_game_state(parsed_data, client_socket)
                        except json.JSONDecodeError as e:
                            print(f"JSON解析错误: {e}, 数据: {line}")
                        except Exception as e:
                            print(f"处理数据错误: {e}")

        except ConnectionResetError:
            print(f"客户端 {client_address} 连接被重置")
        except Exception as e:
            print(f"客户端 {client_address} 错误: {e}")
        finally:
            print(f"客户端断开连接: {client_address}")
            if client_socket in self.clients:
                self.clients.remove(client_socket)
            try:
                client_socket.close()
            except:
                pass

    def process_game_state(self, game_state: Dict[str, Any], client_socket: socket.socket):
        """处理接收到的游戏状态"""
        round_no = game_state.get("round_no", 0)
        state = game_state.get("state", 0)
        print(f"[回合 {round_no} 状态 {state}] 收到游戏状态")

        self.game_states[round_no] = game_state

        response = {
            "status": "ok",
            "message": "数据已接收",
            "round_no": round_no
        }

        try:
            response_str = json.dumps(response, ensure_ascii=False) + "\n"
            client_socket.send(response_str.encode('utf-8'))
        except Exception as e:
            print(f"发送响应失败: {e}")

    def broadcast(self, message: str):
        """向所有客户端广播消息"""
        for client in self.clients[:]:
            try:
                client.send((message + "\n").encode('utf-8'))
            except Exception as e:
                print(f"广播失败: {e}")
                if client in self.clients:
                    self.clients.remove(client)

    def stop(self):
        """停止服务器"""
        self.running = False
        for client in self.clients:
            try:
                client.close()
            except:
                pass
        if self.server_socket:
            try:
                self.server_socket.close()
            except:
                pass
        print("服务器已关闭")


def main():
    import sys

    host = "127.0.0.1"
    port = 8888

    if len(sys.argv) >= 2:
        host = sys.argv[1]
    if len(sys.argv) >= 3:
        port = int(sys.argv[2])

    server = GameServer(host, port)
    server.start()


if __name__ == "__main__":
    main()
