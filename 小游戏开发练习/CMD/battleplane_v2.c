#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define HIGH 24
#define WIDTH 114
#define TOP 2
#define MAX_BULLETS 30
#define PLANE_W 9
#define ENEMY_W 6

typedef struct{
    int x;
    int y;
    int activ; // 1表示激活，0表示未激活
}Bullet;

Bullet bullets[MAX_BULLETS];
int plane_x = 10,
    plane_y = 10;
int enemy_x = WIDTH / 2,
    enemy_y = 0;
int score = 0,
    level = 1;

void withInput();
void withoutInput();
void printPlane(int x){
    int i;
    printf("   __^__\n");
    for(i = 0;i < x;i++){
        printf(" ");
    }
    printf("--@--A--@--\n");
    for(i = 0;i < x;i++){
        printf(" ");
    }
    printf("    / \\");
}


//信息显示函数
void printEdge(){
    int i;
    printf("\n");
    for(i = 0;i < WIDTH;i++){
        printf("-");
    }
    printf("\n");
}
void printInfo(){
    printf("Score: %d",score);
    printf("    Level: %d",level);
    printEdge();
}
void printResult(){
    printEdge();
    printf("You Have Gained %d Points!",score);
    printEdge();
}

//敌机相关函数
void printEnemy(int x){
    int i;
    printf("  \\|/\n");
    for(i = 0;i < x;i++){
        printf(" ");
    }
    printf("TT|X|TT\n");
    for(i = 0;i < x;i++){
        printf(" ");
    }
    printf("  \\-/\n");
}
void updateEnemy(){
    enemy_y++;
    if(enemy_y > HIGH){
        enemy_x = rand() % (WIDTH - ENEMY_W);
        enemy_y = 0;
    }
}

//子弹相关函数
void bulletsInit(){//初始化子弹数组
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        bullets[i].x = -1;
        bullets[i].y = -1;
        bullets[i].activ = 0;
    }
}
void updateBullet(){
    int i;
    for(i = 0;i < MAX_BULLETS;i++){
        if(bullets[i].activ){
            bullets[i].y--; // 子弹向上移动
            if(bullets[i].y <= TOP){ // 超出边界，取消激活
                bullets[i].activ = 0;
            }
        }
    }
}
int checkBullet(int x,int y){
    int i;
    for(i = 0;i < MAX_BULLETS;i++){
        if(bullets[i].y == y && bullets[i].x == x && bullets[i].activ){
            return 1; // 子弹位置匹配且激活
        }
    }
    return 0; // 没有匹配的子弹
}
void shootBullet(){
    int i;
    for(i = 0;i < MAX_BULLETS;i++){
        if(!bullets[i].activ){
            bullets[i].x = plane_x + 5; // 在飞机前方发射子弹
            bullets[i].y = plane_y;
            bullets[i].activ = 1; // 激活子弹
            break;
        }
    }
}

//重要函数
void show(){
    int i,j;
    system("cls");
    printInfo();
    for(i = 0;i < HIGH - TOP;i++){
        for(j = 0;j < WIDTH;j++){
            if(i == plane_y && j == plane_x){
                printPlane(plane_x); // 打印飞机
                j += PLANE_W - 3; // 跳过飞机宽度
            }
            else if(checkBullet(j,i)){
                printf("|"); // 打印子弹
            }
            else if(i == enemy_y && j == enemy_x){
                printEnemy(enemy_x);
                j += ENEMY_W - 1; // 跳过敌机宽度
            }
            else{
                printf(" "); // 打印空格
            }
        }
        printf("\n");
    }
}
void operate(){
    char op;
    if(kbhit()){
        op = getch();
        switch(op){
            case 'a':
                plane_x = (plane_x > 0) ? plane_x - 1 : plane_x;
                break;
            case 'd':
                plane_x = (plane_x < WIDTH - 9) ? plane_x + 1 : plane_x;
                break;
            case 'w':
                plane_y = (plane_y > TOP) ? plane_y - 1 : plane_y;
                break;
            case 's':
                plane_y = (plane_y < HIGH - 1) ? plane_y + 1 : plane_y;
                break;
            case ' ':
                shootBullet();
                break;
        }
    }
}
void checkHit(){
    int i;
    for(i = 0;i < MAX_BULLETS;i++){
        if(bullets[i].x >= enemy_x && bullets[i].x <= enemy_x + ENEMY_W){
            if(bullets[i].y == enemy_y){
                printf("\a");
                bullets[i].activ = 0;
                enemy_y = HIGH + 1;
                score += level * 100;
                level++;
            }
        }
    }
}

int main(){
    srand((unsigned int) time(NULL));
    bulletsInit();
    while(6 - level){
        withInput();
        withoutInput();
        show();
        Sleep(10);
    }
    level--;
    show();
    printResult();
}

void withoutInput(){
    static int speed = 0;
    if(speed < 20 - (level - 1) * 4){
        speed++;
    }
    if(speed == 20 - (level - 1) * 4){
        updateEnemy();
        speed = 0;
    }
    updateBullet();
    checkHit();
}
void withInput(){
    operate();
}

