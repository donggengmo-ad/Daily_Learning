@echo off
chcp 65001 >nul
echo ========================================
echo 启动Python服务器
echo ========================================
echo.
echo 服务器地址: 127.0.0.1:8888
echo 按 Ctrl+C 停止服务器
echo.

python server.py

pause
