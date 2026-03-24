#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define AoG 1//acceleration of gravity

typedef struct{
    int x;
    int up;
    int down;
}WALL;

int top,bottom,left,right;//屏幕边界
int bird_x,bird_y,bird_v;//小鸟位置和速度
int life,score;//剩余生命值和跨过墙数（得分）
int wall_num;
WALL *wall;//墙数量和数组
int pardon;//刚撞墙或得分时获得扣血和得分的豁免权（墙归位时解除）

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
int checkWall(int x,int y);
void restart();//扣血重启

int main(){
    startup();
    while(life){//没死时执行游戏
        withInput();
        withoutInput();
        show();
    }
}

void startup(){
    HideCursor();
    srand((unsigned int) time(NULL));
    //初始化屏幕边界
    top = 0;
    bottom = 20;
    left = 0;
    right = 60;
    //初始化玩家信息
    life = 3;
    score = 0;
    pardon = 0;
    //初始化小鸟各项信息
    bird_x = left + 1;
    bird_y = top + 1;
    bird_v = 0;
    //初始化墙信息
    wall_num = 4;
    wall = (WALL *) malloc(sizeof(WALL) * wall_num);
    for(int i = 0;i < wall_num;i++){
        wall[i].x = right - left - i * (right - left) / wall_num;
        wall[i].up = rand() % (bottom - top - 5);//up为随机位置
        wall[i].down = wall[i].up + 6;//down为up位置下移6格
    }
}
void withInput(){
    char input;
    if(kbhit()){//判断是否有输入
        input = getch();
        if(input == ' '){
            bird_y -= 2;
            bird_v = 0;
        }
    }
}
void withoutInput(){
    //通过循环控制小鸟速度
    static int speed_bird = 0;
    if(speed_bird < 20){
        speed_bird++;
    }
    else{
        //小鸟自由落体
        bird_v += AoG;
        bird_y += bird_v;
        //碰到边界不能动
        if(bird_y < top){
            bird_y = top;
        }
        if(bird_y > bottom){
            bird_y = bottom;
        }
        //重置速度"填充"过程
        speed_bird = 0;
    }
    //通过循环控制墙速度
    static int speed_wall = 0;
    if(speed_wall < 10){
        speed_wall++;
    }
    else{
        for(int i = 0;i < wall_num;i++){
            wall[i].x--;//墙向左移动
            if(wall[i].x < left){
                wall[i].x = right;//墙位置重置
                pardon = 0;//解除豁免权
            }
        }
        //重置速度"填充"过程
        speed_wall = 0;
    }
    //检查是否撞墙
    if(checkWall(bird_x,bird_y)){
        if(!pardon){
            life--;//无豁免权时撞墙扣血
            pardon = 1;//获得豁免权（扣血重启的版本下暂时没什么意义）
            restart();//扣血重启
        }
    }
    else{
        for(int i = 0;i < wall_num;i++){
            if(bird_x == wall[i].x){
                if(!pardon){
                    score++;//无豁免权时穿过得分
                    pardon = 1;//获得豁免权
                    break;
                }
            }
        }
    }
}
void show(){
    gotoxy(0,0);
    int i,j;
    //遍历屏幕位置
    for(i = top;i <= bottom;i++){
        for(j = left;j <= right;j++){
            if(i == bird_y && j == bird_x){
                printf("@");//打印小鸟
            }
            else if(checkWall(j,i)){
                printf("*");//打印墙
            }
            else{
                printf(" ");
            }
        }    
        printf("\n");
    }
    printf("life: %d   score: %d",life,score);//打印玩家信息 
}
int checkWall(int x,int y){
    for(int i = 0;i < wall_num;i++){
        if(x == wall[i].x && (y <= wall[i].up || y >= wall[i].down)){
            return 1;
        }
    }
    return 0;
}
void restart(){
    //初始化小鸟各项信息
    bird_x = left + 1;
    bird_y = top + 1;
    bird_v = 0;
    //初始化墙信息
    wall_num = 4;
    wall = (WALL *) malloc(sizeof(WALL) * wall_num);
    for(int i = 0;i < wall_num;i++){
        wall[i].x = right - left - i * (right - left) / wall_num;
        wall[i].up = rand() % (bottom - top - 5);//up为随机位置
        wall[i].down = wall[i].up + 6;//down为up位置下移6格
    }
}