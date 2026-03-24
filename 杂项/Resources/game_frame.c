#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

void gotoxy(int x,int y){//移动光标到xy位置
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(handle,pos);
}
void HideCursor(){//隐藏光标
    CONSOLE_CURSOR_INFO cursor_info = {1,0};//第二个值为0表示隐藏光标
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);
}

void startup();//数据初始化
void withInput();
void withoutInput();
void show();//显示画面

int main(){
    startup();
    while(1){//循环执行游戏
        withInput();
        withoutInput();
        show();
    }
}

void startup(){
    HideCursor();
    srand((unsigned int) time(NULL));
}
void withInput(){
    char input;
    if(kbhit()){//判断是否有输入
        input = getch();
        switch(input){
             case 'a':
                break;
            case 'w':
                break;
            case 's':
                break;
            case 'd':
                break;
        }
    }
}
void withoutInput(){}
void show(){
    gotoxy(0,0);
}