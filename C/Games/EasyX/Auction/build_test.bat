@echo off
chcp 65001 >nul
echo ========================================
echo 编译Socket测试程序
echo ========================================
echo.

g++ -std=c++11 -o output/socket_test.exe source/socket_test.cpp -lws2_32

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ? 编译成功！
    echo 可执行文件位置: output\socket_test.exe
) else (
    echo.
    echo ? 编译失败！
)

echo.
pause
