#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

typedef struct{
    int x;
    int y;
    int exist;
}Target;

int top,bottom,left,right;//屏幕边界
int ball_x,ball_y,ball_vx,ball_vy;//小球位置和速度
int plate_x,plate_y,plate_radius,plate_left,plate_right;//挡板的中点位置、半径、左右端点
int blocks,count,life,score;//击中砖块数、反弹小球数、剩余生命值和总得分
int target_num;
Target *target;//砖块数量和信息数组

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
void closeDown();//游戏战后结算
void withInput();
void withoutInput();
void show();//显示画面
int chackTarget(int x);//检查某个位置是否有砖块
int howManyTargets();//检查还有多少个砖块剩余

int main(){
    startup();
    while(life && howManyTargets()){//还有剩余生命或剩余砖块时执行游戏
        withInput();
        withoutInput();
        show();
    }
    closeDown();
    return 0;
}

void startup(){
    int i;
    HideCursor();
    srand((unsigned int) time(NULL));
    //初始化屏幕边界
    top = 0;
    bottom = 15;
    left = 0;
    right = 30;
    //初始化玩家信息
    life = 3;
    blocks = 0;
    //初始化小球各项信息
    ball_x = rand() % (right - left - 2) + 1;
    ball_y = top;
    ball_vx = 1;
    ball_vy = 1;
    //初始化挡板各项信息
    plate_radius = 3;
    plate_x = left + plate_radius + 2;
    plate_y = bottom - 1;
    plate_left = plate_x - plate_radius;
    plate_right = plate_x + plate_radius;
    //初始化砖块信息
    target_num = rand() % 3 + 5;
    target = (Target *) malloc(sizeof(Target) * target_num);
    for(i = 0;i < target_num;i++){
        target[i].x = rand() % (right - left - 2) + 1;
        target[i].y = top + 1;
        target[i].exist = 1;
    }
}
void withInput(){
    char input;
    if(kbhit()){//判断是否有输入
        input = getch();
        switch(input){
             case 'a'://左移挡板
                plate_x--;
                plate_left = plate_x - plate_radius;
                plate_right = plate_x + plate_radius;//调整挡板位置和左右端点
                break;
            case 'd'://右移挡板
                plate_x++;
                plate_left = plate_x - plate_radius;
                plate_right = plate_x + plate_radius;
                break;
            case '1'://提前结束游戏d
                closeDown();
        }
        //挡板碰边不能动
        if(plate_left < left + 1){
            plate_x++;
        }
        if(plate_right > right - 1){
            plate_x--;
        }
    }
}
void withoutInput(){
    int i;
    //通过循环控制小球速度
    static int speed = 0;
    if(speed < 8){
        speed++;
    }
    else{
        //按照速度更新小球位置
        ball_x += ball_vx;
        ball_y += ball_vy;
        //碰到边界反弹
        if(ball_x == left || ball_x == right){
            ball_vx *= -1;
        }
        if(ball_y == top){
            ball_vy *= -1;
        }
        //碰到挡板反弹并记录
        if(ball_y == plate_y - 1){
            if(ball_x >= plate_left && ball_x <= plate_right){
                ball_vy += -1;
                count++;
            }
        }
        //碰到底线重置小球并扣除生命值
        if(ball_y == bottom){
            ball_y = top;
            life--;
        }
        //检查小球是否击中砖块
        if(ball_y == target[0].y){
            for(i = 0;i < target_num;i++){
                if(target[i].x == ball_x && target[i].exist){
                    target[i].exist = 0;
                    blocks += 1;
                }
            }
        }
        //重置速度"填充"过程
        speed = 0;
    }
}
void show(){
    gotoxy(left,top);
    int i,j;
    //遍历屏幕位置
    for(i = top;i <= bottom;i++){
        for(j = left;j <= right;j++){
            if(i == ball_y && j == ball_x){
                printf("o");//打印小球
            }
            else if(i == top || i == bottom){
                printf("-");//打印上下边界
            }
            else if(j == left || j == right){
                printf("|");//打印左右边界
            }
            else if(i == bottom - 1 && j >= plate_left && j <= plate_right){
                printf("*");//打印挡板
            }
            else if(i == target[0].y && chackTarget(j)){
                printf("H");//打印砖块
            }
            else{
                printf(" ");
            }
        }    
        printf("\n");
    }
    printf("life: %d   count: %d   blocks: %d",life,count / 2,blocks);//打印玩家信息 
}
int chackTarget(int x){
    int i;
    for(i = 0;i < target_num;i++){
        if(target[i].exist && target[i].x == x){
            return 1;
        }
    }
    return 0;
}
int howManyTargets(){
    int i,tc = 0;
    for(i = 0;i < target_num;i++){
        if(target[i].exist){
            tc++;
        }
    }
    return tc;
}
void closeDown(){
    //判断输赢
    if(!howManyTargets()){
        printf("\nYou Win!\n");
    }
    else{
        printf("\nGame Over!\n");
    }
    //计算得分并输出
    score = blocks * 100 * life / (count / 2);
    printf("You've Gained %d Points!\n",score);
    //退出程序
    exit(0);
}