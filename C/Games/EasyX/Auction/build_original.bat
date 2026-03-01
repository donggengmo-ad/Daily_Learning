@echo off
chcp 65001 >nul
echo ========================================
echo 编译拍卖游戏 (原版)
echo ========================================
echo.

g++ -std=c++11 -o output/auction.exe source/main.cpp source/AI.cpp source/interface.cpp source/gaming.cpp source/menu.cpp -leasyx -lws2_32 -lgdi32 -limm32 -lmsimg32 -lole32 -loleaut32 -lwinmm -luuid

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ? 编译成功！
    echo 可执行文件位置: output\auction.exe
) else (
    echo.
    echo ? 编译失败！
)

echo.
pause
