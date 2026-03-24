#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <math.h>

#define ENEMY_NUM  10
#define LAZER_NUM 3
#define MAX_LEVEL 50
#define PI 3.14159265358979323846

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
typedef struct{
    int x;
    int y;
    int theta,theta0;
    int activ;
}LAZER;
 
int top,bottom,left,right;
int player_x,player_y;
ENEMY enemy[ENEMY_NUM];
LAZER lazer[LAZER_NUM];
unsigned long long start_ms;
int step;

void startup();//数据初始化
void withInput();
void withoutInput();
void show();//显示画面
int checkEnemy(int x,int y);
int checkLazer(int x,int y);

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
    step = 0;
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
    //初始化激光
    for(i = 0;i < LAZER_NUM;i++){
        lazer[i].x = 0;
        lazer[i].y = 0;
        lazer[i].theta = 0;
        lazer[i].theta0 = 0;
        lazer[i].activ = 0;
    }
}
void withInput(){
    char input;
    if(kbhit()){//判断是否有输入
        input = getch();
        //根据输入移动玩家
        switch(input){
            //小写普通移动
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
            //大写冲刺移动
            case 'A':
                player_x -= 3;
                break;
            case 'W':
                player_y -= 3;
                break;
            case 'S':
                player_y += 3;
                break;
            case 'D':
                player_x += 3;
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
    static int speed_enemy = 0;
    int level;

    level = (step / 2 < MAX_LEVEL) ? (step / 2) % MAX_LEVEL : MAX_LEVEL; //满级前，每两步升级一次
    if(speed_enemy < MAX_LEVEL - level + 5){//移速从MAX_LEVEL提升到5（每10级提升1）
        speed_enemy++;
    }
    else{
        //更新敌人
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
        //更新等级和重置速度
        speed_enemy = 0;
        step++;//记录敌人移动步数（将决定等级）
    }
    // 每10级触发新激光
    static int last_level = -1;//确保每级只触发一个激光
    if(level >= 10 && !(level % 10) && last_level != level){
        for(i = 0;i < LAZER_NUM;i++){
            if(!lazer[i].activ){
                lazer[i].activ = 1;
                lazer[i].x = rand() % (right - left - 2) + 1;
                lazer[i].y = rand() % (bottom - top - 2) + 1;
                lazer[i].theta = rand() % 360;
                lazer[i].theta0 = lazer[i].theta;
                break;
            }
        }
        last_level = level;
    }
    static int speed_lazer = 0;
    if(speed_lazer < 10 - level/10){//转速从10提升到5（每10级提升1）
        speed_lazer++;
    }
    else{
        //更新激光
        for(i = 0;i < LAZER_NUM;i++){
            if(lazer[i].activ){
                lazer[i].theta += 1; 
                if(lazer[i].theta >= lazer[i].theta0 + 360){
                    lazer[i].activ = 0;//转完一圈后消失
                }
            }
        }
        speed_lazer = 0;
    }
    //检查是否被击中
    if(checkEnemy(player_x,player_y) || checkLazer(player_x,player_y)){
        exit(0);
    }
}
void show(){
    gotoxy(left,top);
    int i,j;
    for(i = top;i <= bottom;i++){
        for(j = left;j <= right;j++){
            if(i == player_y && j == player_x){
                printf("$");//打印玩家
            }
            else if(checkEnemy(j,i)){
                printf("#");//打印敌人
            }
            else if(checkLazer(j,i) == 1){
                printf("*");//打印激光
            }
            else if(checkLazer(j,i) == 2){
                printf("o");//打印激光旋转点
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
int checkLazer(int x,int y){//检查某一位置是否有激光
    int i;
    for(i = 0;i < LAZER_NUM;i++){
        if(lazer[i].activ){
            //激光旋转点
            if(x == lazer[i].x && y == lazer[i].y){
                return 2;
            }
            //用方向向量计算点线距离
            double rad = lazer[i].theta * PI / 180.0;
            double xn = -sin(rad);
            double yn = cos(rad);
            double rx = x - lazer[i].x;
            double ry = y - lazer[i].y;
            double dist = fabs(rx * xn + ry * yn);
            if(dist < 0.5){//阈值0.5，可调整激光粗细
                return 1;
            }
            /*
            具体算法：
            ∵单位方向向量 d = (dr,dy) = (cosθ,sinθ)
            ∴单位法线向量 n = (xn,yn) = (-sinθ,cosθ)
            线固定点 p0 = (x0,y0) = (lazer[i].x,lazer[i].y)
            目标点到点固定点向量 r = (rx,ry) = (x - x0,y - y0)
            点到直线的距离dist = r在n方向的投影长度 = |r·n|/|n| = |r·n| (因为n是单位向量)
            ∴dist = |(x - x0)(-sinθ) + (y - y0)(cosθ)|
            当dist < 某个阈值时，认为点在线上
            */
        }
    }
    return 0;
}