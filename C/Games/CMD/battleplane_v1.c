#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

typedef struct TargetStruct{
    //待命靶子数组，包括与上一靶子间隔和是否被击中
    int x;
    int shot;
}target;

//重要的全局变量（6）
int x = 10,
    y = 10;
target *tar;
int target_n,
    target_remain;
int score = 0;

void gameRender(int f);//渲染游戏，来回调用不得不先声明
void planeFire(){
    int i,j;
    //定位飞机左上角顺便打印子弹
    for(i = 0;i < y;i++){
        for(j = 0;j < x;j++){
            printf(" ");
        }
        printf("  |");
        printf("\n");
    }
    for(i = 0;i < x;i++){
        printf(" ");
    }
    //打印飞机本体
    printf("  *  \n");
    for(i = 0;i < x;i++){
        printf(" ");
    }
    printf("*****\n");
    for(i = 0;i < x;i++){
        printf(" ");
    }
    printf(" * * ");
    system("cls");
    gameRender(0);
}
void planeRest(){
    int i;
    //定位飞机左上角
    for(i = 0;i < y;i++){
        printf("\n");
    }
    for(i = 0;i < x;i++){
        printf(" ");
    }
    //打印飞机本体
    printf("  *  \n");
    for(i = 0;i < x;i++){
        printf(" ");
    }
    printf("*****\n");
    for(i = 0;i < x;i++){
        printf(" ");
    }
    printf(" * * ");
}
int operate(){
    //记录移动操作与是否开火
    char inp;
    inp = getch();
    switch(inp){
        case 'a':
            x--;
            break;
        case 'w':
            y--;
            break;
        case 's':
            y++;
            break;
        case 'd':
            x++;
            break;
        case ' ':
            return 1;
        //作弊方法：按1强制结算
        case '1':
            target_remain = 0;
            break;
    }
    //防止超越边界
    if(x < 0){
        x = 0;
    }
    else if(x > 100){
        x = 100;
    }
    if(y < 1){
        y = 1;
    }
    return 0;
}
target *getTarget(int *n){
    int i;
    //获取随机的靶子数
    *n = rand() % 5 + 7;
    target_remain = *n;
    target *tx = (target *) malloc(sizeof(target) * (*n));
    //获取相邻靶子坐标间隔，并初始化靶子数组
    for(i = 0;i < *n;i++){
       tx[i].x = rand() % 10 + 2;
       tx[i].shot = 0;
    }
    return tx;
}
void printTarget(target *tar,int n){
    int i,j;
    for(i = 0;i < n;i++){
        //根据前一个靶子的位置和间隔找这个靶子的位置
        for(j = 0;j < tar[i].x;j++){
            printf(" ");
        }
        //判断靶子是否被打掉
        if(tar[i].shot){
            printf(" ");
        }
        else{
            printf("@");
        }
    }
}
void shotJudge(){
    int i;
    int hadShot = 0;
    int x_temp = 0;
    for(i = 0;i < target_n;i++){
        //把间隔累加成真正的x坐标
        if(i > 0){
            //+1补回@占的一位
            x_temp += (tar[i].x + 1);
        }
        else{
            x_temp += tar[i].x;
        }
        //击中记录并加分
        if(x + 2 == x_temp && !tar[i].shot){
            tar[i].shot = 1;
            hadShot = 1;
            target_remain--;
            score += 100;
            printf("\a");
            break;
        }
    }
    //没击中扣分
    if(!hadShot){
        score -= 10;
        //防止负分
        if(score < 0){
            score = 0;
        }
    }
}
void printEdge(){
    int i;
    printf("\n");
    for(i = 0;i < 100;i++){
        printf("-");
    }
    printf("\n");
}
void printInfo(){
    printf("Points gained: %d",score);
    printf("    Target left: %d",target_remain);
    printEdge();
}
void printResult(){
    //system("cls");
    printEdge();
    printf("You Have Gained %d Points!",score);
    printEdge();
}

int main(){
    int fire = 0;
    srand((unsigned int) time(NULL));
    tar = getTarget(&target_n);
    gameRender(0);
    while(target_remain){
        fire = operate();
        gameRender(fire);
    }
    printResult();
    return 0;
}

void gameRender(int f){
    system("cls");
    printInfo();
    printTarget(tar,target_n);
    if(f){
        shotJudge();
        planeFire();
    }
    else{
        planeRest();
    }
}