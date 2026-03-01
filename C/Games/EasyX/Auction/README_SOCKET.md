# 拍卖游戏 Socket联机功能

## ? 文件夹结构

```
Auction/
├── source/              # 源代码目录
│   ├── main.cpp         # 原版游戏入口（无Socket）
│   ├── main_with_socket.cpp  # 带Socket功能的游戏入口
│   ├── socket.cpp       # Socket通信实现
│   ├── AI.cpp
│   ├── interface.cpp
│   ├── gaming.cpp
│   ├── menu.cpp
│   └── auction.h        # 头文件
├── output/              # 编译输出目录
│   ├── images/          # 游戏图片资源
│   ├── auction.exe      # 原版游戏可执行文件
│   └── auction_with_socket.exe  # 带Socket的游戏可执行文件
├── server.py            # Python服务器
├── build.bat            # 编译带Socket的版本
├── build_original.bat   # 编译原版
├── run_server.bat       # 启动Python服务器
└── run_game.bat         # 运行游戏
```

---

## ? 快速开始

### 方式一：使用批处理脚本（推荐）

1. **启动服务器**
   ```
   双击运行 run_server.bat
   ```

2. **编译游戏**
   ```
   双击运行 build.bat
   ```

3. **运行游戏**
   ```
   双击运行 run_game.bat
   ```

---

### 方式二：使用命令行

#### 编译带Socket的版本
```powershell
g++ -std=c++11 -o output/auction_with_socket.exe source/main_with_socket.cpp source/socket.cpp source/AI.cpp source/interface.cpp source/gaming.cpp source/menu.cpp -leasyx -lws2_32 -lgdi32 -limm32 -lmsimg32 -lole32 -loleaut32 -lwinmm -luuid
```

#### 编译原版
```powershell
g++ -std=c++11 -o output/auction.exe source/main.cpp source/AI.cpp source/interface.cpp source/gaming.cpp source/menu.cpp -leasyx -lws2_32 -lgdi32 -limm32 -lmsimg32 -lole32 -loleaut32 -lwinmm -luuid
```

#### 启动Python服务器
```powershell
python server.py
```

#### 运行游戏
```powershell
cd output
.\auction_with_socket.exe
```

---

## ? Socket API说明

| 函数 | 说明 |
|------|------|
| `initSocket(ip, port)` | 初始化Socket连接 |
| `sendData(data)` | 发送字符串数据 |
| `receiveData()` | 接收数据并返回 |
| `closeSocket()` | 关闭Socket连接 |
| `sendGameState()` | 发送游戏状态（JSON） |
| `receiveGameState()` | 接收并打印游戏状态 |

---

## ? 配置说明

默认服务器地址：`127.0.0.1:8888`

如需修改，编辑 `source/main_with_socket.cpp` 中的：
```cpp
const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8888;
```
