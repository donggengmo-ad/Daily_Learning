#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define HIGH 20
#define WIDTH 50
#define EnemyNum 10

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

int canvas[WIDTH][HIGH] = {0};//画面元素，0为空，1为飞机，2为子弹，3为敌机
int plane_x,plane_y;
int enemy_x[EnemyNum],enemy_y[EnemyNum];
int score;
int bullet_w;

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
    //初始化飞机
    plane_x = WIDTH / 2;
    plane_y = HIGH / 2;
    canvas[plane_x][plane_y] = 1;//记录飞机位置
    //初始化敌机
    int i;
    for(i = 0;i < EnemyNum;i++){
        enemy_x[i] = rand() % WIDTH;
        enemy_y[i] = rand() % 3;
        canvas[enemy_x[i]][enemy_y[i]] = 3;
    }
    score = 0;
    bullet_w = 1;
}
void withInput(){
    char input;
    if(kbhit()){//判断是否有输入
        input = getch();
        switch(input){
            case 'a':
                if(plane_x > 0){
                    canvas[plane_x][plane_y] = 0;
                    plane_x--;
                    canvas[plane_x][plane_y] = 1;
                }
                break;
            case 'w':
                if(plane_y > 0){
                    canvas[plane_x][plane_y] = 0;
                    plane_y--;
                    canvas[plane_x][plane_y] = 1;
                }
                break;
            case 's':
                if(plane_y < HIGH - 1){
                    canvas[plane_x][plane_y] = 0;
                    plane_y++;
                    canvas[plane_x][plane_y] = 1;
                }
                break;
            case 'd':
                if(plane_x < WIDTH - 1){
                    canvas[plane_x][plane_y] = 0;
                    plane_x++;
                    canvas[plane_x][plane_y] = 1;
                }
                break;
            case ' ':
                if(plane_y > 0){
                    int left,right;
                    left = plane_x - bullet_w;
                    if(left < 0){
                        left = 0;
                    }
                    right = plane_x + bullet_w;
                    if(right > WIDTH - 1){
                        right = WIDTH - 1;
                    }
                    for(int x = left;x <= right;x++){
                        canvas[x][plane_y - 1] = 2;
                    }
                }
                break;
        }
    }
}
void withoutInput(){
    int x,y,i;
    //子弹变宽
    bullet_w = score / 5;
    if(bullet_w >= WIDTH / 2){
        bullet_w = WIDTH / 2;
    }
    //子弹向前
    for(y = 0;y < HIGH;y++){
        for(x = 0;x < WIDTH;x++){
            if(canvas[x][y] == 2){
                canvas[x][y] = 0;
                if(y > 0){
                    // 先判断目标格是否有敌机（命中检测应看 targetY）
                    int targetY = y - 1;
                    int hitIndex = -1;
                    for(i = 0; i < EnemyNum; i++){
                        if(enemy_x[i] == x && enemy_y[i] == targetY){
                            hitIndex = i;
                            break;
                        }
                    }
                    if(hitIndex != -1){
                        // 击中敌机：清理旧位置并重生敌机
                        if(enemy_x[hitIndex] >= 0 && enemy_x[hitIndex] < WIDTH &&
                        enemy_y[hitIndex] >= 0 && enemy_y[hitIndex] < HIGH){
                            canvas[enemy_x[hitIndex]][enemy_y[hitIndex]] = 0;
                        }
                        enemy_y[hitIndex] = rand() % 3;
                        enemy_x[hitIndex] = rand() % WIDTH;
                        canvas[enemy_x[hitIndex]][enemy_y[hitIndex]] = 3;
                        score++;
                    } 
                    else{
                        canvas[x][targetY] = 2;
                    }
                }
            }
        }
    }
    // 敌机下移并在越界时安全重生
    static int speed = 0;
    int level = 50 - score;
    if(level < 1){
        level = 1;
    }  
    if(speed < level){
        speed++;
    }
    if(speed == level){
        for(i = 0;i < EnemyNum;i++){
            // 清除当前格（若在屏内）
            if(enemy_x[i] >= 0 && 
                enemy_x[i] < WIDTH &&
                enemy_y[i] >= 0 && 
                enemy_y[i] < HIGH){
                canvas[enemy_x[i]][enemy_y[i]] = 0;
            }
            enemy_y[i]++; // 下移
            if(enemy_y[i] >= HIGH){
                // 已到底部，重生到顶部若干行
                enemy_y[i] = rand() % 3;
                enemy_x[i] = rand() % WIDTH;
                if(score > 0){
                    score--;
                }
                // 写入新位置（保证合法）
                canvas[enemy_x[i]][enemy_y[i]] = 3;
            }
            else {
                // 在屏内正常写入
                canvas[enemy_x[i]][enemy_y[i]] = 3;
            }
        }
        speed = 0;
    } 
}
void show(){
    gotoxy(0,0);
    int x,y;
    for(y = 0;y < HIGH;y++){
        for(x = 0;x < WIDTH;x++){
            if(canvas[x][y] == 1){
                printf("A");//输出飞机
            }
            else if(canvas[x][y] == 2){
                printf("|");
            }
            else if(canvas[x][y] == 3){
                printf("@");
            }
            else{
                printf(" ");//输出空位
            }
        }
        printf("\n");
    }
    printf("Score: %d",score);
}