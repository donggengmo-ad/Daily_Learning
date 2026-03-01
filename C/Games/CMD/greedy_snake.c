#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define HIGH 25
#define WIDTH 60

int canvas[WIDTH][HIGH] = {0};
int moveDirection;//移动方向：1上2下3左4右

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
void moveSnake();

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
    int x,y;
    //边框标记为-1
    for(y = 0;y < HIGH;y++){
        canvas[0][y] = -1;
        canvas[WIDTH-1][y] = -1;
    }
    for(x = 0;x < WIDTH;x++){
        canvas[x][0] = -1;
        canvas[x][HIGH-1] = -1;
    }
    //初始化蛇头（1）蛇身（2345...）
    canvas[WIDTH/2][HIGH/2] = 1;
    for(x = 1;x <= 4;x++){
        canvas[WIDTH/2-x][HIGH/2] = x + 1;
    }
    //初始化食物（-2）
    int foodx,foody;
    do{
        foodx = rand() % (WIDTH - 2) + 1;
        foody = rand() % (HIGH - 2) + 1;
    }while(canvas[foodx][foody] != 0);//确保食物不生成在蛇身上
    canvas[foodx][foody] = -2;
    moveDirection = 4;
}
void withInput(){
    char input;
    if(kbhit()){//判断是否有输入
        input = getch();
        switch(input){
            case 'a':
                if(moveDirection != 4)//不能直接向反方向移动
                    moveDirection = 3;
                break;
            case 'w':
                if(moveDirection != 2)
                    moveDirection = 1;
                break;
            case 's':
                if(moveDirection != 1)
                    moveDirection = 2;
                break;
            case 'd':
                if(moveDirection != 3)
                    moveDirection = 4;
                break;
        }
    }
}
void withoutInput(){
    static int speed = 0;
    if(speed < 10){
        speed++;
    }
    else{
        moveSnake();
        speed = 0;
    }
}
void show(){
    gotoxy(0,0);
    int x,y;
    for(y = 0;y < HIGH;y++){
        for(x = 0;x < WIDTH;x++){
            if(canvas[x][y] == 0){
                printf(" ");//输出空格
            }
            else if(canvas[x][y] == -1){
                printf("#");//输出边框
            }
            else if(canvas[x][y] == 1){
                printf("@");//输出蛇头
            }
            else if(canvas[x][y] == -2){
                printf("$");//输出食物
            }
            else{
                printf("*");//输出蛇身
            }
        }
        printf("\n");
    }
}
void moveSnake(){
    int x,y;
    int max = 0;
    int old_tailx,old_taily;
    int old_headx,old_heady;
    int new_headx,new_heady;
    for(y = 1;y < HIGH - 1;y++){
        for(x = 1;x < WIDTH - 1;x++){
            if(canvas[x][y] > 0){
                //所有元素大于零处（即有蛇处）加一
                canvas[x][y]++;
                //求出最大值（即蛇尾）
                if(canvas[x][y] > max){
                    max = canvas[x][y];
                    old_tailx = x;
                    old_taily = y;
                }
                //保存旧蛇头
                if(canvas[x][y] == 2){
                    old_headx = x;
                    old_heady = y;
                }
            }
        }
    }
    //根据方向放置新蛇头
    switch (moveDirection){
    case 1://向上
        new_headx = old_headx;
        new_heady = old_heady - 1;
        break;
    case 2://向下
        new_headx = old_headx;
        new_heady = old_heady + 1;
        break;
    case 3://向左
        new_headx = old_headx - 1;
        new_heady = old_heady;
        break;
    case 4://向右
        new_headx = old_headx + 1;
        new_heady = old_heady;
        break;
    default:
        break;
    }
     //判断新蛇头位置是否有食物
    if(canvas[new_headx][new_heady] == -2){
        //食物原始位置归零
        canvas[new_headx][new_heady] = 0;
        //生成新食物
        int foodx,foody;
        do{
            foodx = rand() % (WIDTH - 2) + 1;
            foody = rand() % (HIGH - 2) + 1;
        }while(canvas[foodx][foody] != 0);//确保食物不生成在蛇身上
        canvas[foodx][foody] = -2;
    }
    else{
        //清除旧蛇尾
        canvas[old_tailx][old_taily] = 0;
    }
    //判断新蛇头位置是否撞墙或撞自己
    if(canvas[new_headx][new_heady] == -1 || canvas[new_headx][new_heady] > 0){
        printf("Game Over!");
        exit(0);
    }
    else{
        canvas[new_headx][new_heady] = 1;
    }
}