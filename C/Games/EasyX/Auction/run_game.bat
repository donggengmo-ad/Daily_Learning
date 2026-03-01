@echo off
chcp 65001 >nul
echo ========================================
echo 运行拍卖游戏 (带Socket)
echo ========================================
echo.
echo 请确保Python服务器已启动！
echo.

cd output
auction_with_socket.exe

cd ..
pause
