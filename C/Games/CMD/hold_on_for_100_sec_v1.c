#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define ENEMY_NUM  10
#define MAX_LEVEL 50

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

typedef struct{
    int x;
    int y;
}ENEMY;
 
int top,bottom,left,right;
int player_x,player_y;
ENEMY enemy[ENEMY_NUM];
unsigned long long start_ms;

void startup();//数据初始化
void withInput();
void withoutInput();
void show();//显示画面
int checkEnemy(int x,int y);

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
    int i;
    //初始化计时时间
    start_ms = GetTickCount64();
    //初始化屏幕边界
    top = 0;
    bottom = 25;
    left = 0;
    right =60;
    //初始化玩家信息
    player_x = (right - left) / 2;
    player_y = (bottom - top) / 2;
    //初始化敌人
    int section;
    for(i = 0;i < ENEMY_NUM;i++){
        //分段随机分布，避开玩家周围防止秒死
        section = rand() % 2;
        switch(section){//分左右
            case 0:
                enemy[i].x = rand() % ((player_x - 1) - left - 1) + left + 1;
                break;
            case 1:
                enemy[i].x = rand() % (right - (player_x + 1) - 1) + player_x + 1;
                break;
        }
        section = rand() % 2;
        switch(section){//分上下
            case 0:
                enemy[i].y = rand() % ((player_y - 1) - top - 1) + top + 1;
                break;
            case 1:
                enemy[i].y = rand() % (bottom - (player_y + 1) - 1) + player_y + 1;
                break;
        }
    }
    //防止敌人重合
    for(i = 0;i < ENEMY_NUM;i++){
        while(checkEnemy(enemy[i].x,enemy[i].y) >= 2){
            enemy[i].x++;
            enemy[i].y++;
        }
    }
}
void withInput(){
    char input;
    if(kbhit()){//判断是否有输入
        input = getch();
        //根据输入移动玩家
        switch(input){
            case 'a':
                player_x--;
                break;
            case 'w':
                player_y--;
                break;
            case 's':
                player_y++;
                break;
            case 'd':
                player_x++;
                break;
        }
        //玩家碰边不能移动
        if(player_y > bottom - 1){
            player_y = bottom - 1;
        }
        else if(player_y < top + 1){
            player_y = top + 1;
        }
        if(player_x > right - 1){
            player_x = right - 1;
        }
        else if(player_x < left + 1){
            player_x = left + 1;
        }
    }
}
void withoutInput(){
    int i;
    //控制敌人移速
    static int speed = 0,level = 0;
    if(speed < MAX_LEVEL - level / 2){//每两步升级一次
        speed++;
    }
    else{
        for(i = 0;i < ENEMY_NUM;i++){
            //使敌人靠近玩家
            if(enemy[i].x < player_x){
                enemy[i].x++;
            }
            else{
                enemy[i].x--;
            }
            if(enemy[i].y < player_y){
                enemy[i].y++;
            }
            else{
                enemy[i].y--;
            }
            //防止敌人重合
            while(checkEnemy(enemy[i].x,enemy[i].y) >= 2){
                enemy[i].x++;
                enemy[i].y++;
            }
            //敌人碰边不能动
            if(enemy[i].y > bottom - 1){
                enemy[i].y = bottom - 1;
            }
            else if(enemy[i].y < top + 1){
                enemy[i].y = top + 1;
            }
            if(enemy[i].x > right - 1){
                enemy[i].x = right - 1;
            }
            else if(enemy[i].x < left + 1){
                enemy[i].x = left + 1;
            }
        }
        speed = 0;
        //没满级前升级
        if(level < MAX_LEVEL - 5){
            level++;
        }
    }
    //检查是否被击中
    if(checkEnemy(player_x,player_y)){
        exit(0);
    }
}
void show(){
    gotoxy(left,top);
    int i,j;
    for(i = top;i <= bottom;i++){
        for(j = left;j <= right;j++){
            if(i == player_y && j == player_x){
                printf("*");//打印玩家
            }
            else if(checkEnemy(j,i)){
                printf("#");//打印敌人
            }
            else if(i == top || i == bottom){
                printf("-");//打印上下边界
            }
            else if(j == left || j == right){
                printf("|");//打印左右边界
            }
            else{
                printf(" ");
            }
        }
        printf("\n");
    }
    //获取并打印时间
    unsigned long long elapsed_ms = GetTickCount64() - start_ms;
    unsigned long long sec = elapsed_ms / 1000;
    unsigned long long msec = elapsed_ms % 1000;
    printf("You've Held On For: %llu.%llu s!\n",sec,msec);
}
int checkEnemy(int x,int y){//检查某一位置有多少敌人
    int i,count = 0;
    for(i = 0;i < ENEMY_NUM;i++){
        if(enemy[i].x == x && enemy[i].y == y){
            count++;
        }
    }
    return count;
}
