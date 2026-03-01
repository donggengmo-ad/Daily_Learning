#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <math.h>

#define WIDTH 200
#define HIGH 60 //定义画面尺寸
#define PI 3.14159265358979323846

typedef struct{
    int life;
    int area;
    int role;
    int age;
}CELL;
typedef struct{
    int x;
    int y;//中心坐标
    double radius;//区域半径
}AREA;

CELL cells[WIDTH][HIGH];
int speed,paused;
int showLife;//是否显示生命
int round_num;//记录进化轮数和生命年龄

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
int countNeibour(int ,int ,int);
void cellsInit();
double dist(int ,int ,int ,int);
int checkArea(int x,int y,AREA *areas,int area_count);

int main(){
    startup();
    while(1){//循环执行游戏
        withInput();
        withoutInput();
        show();
    }
}

void startup(){
    srand((unsigned int) time(NULL));
    HideCursor();
    speed = 100;
    paused = 0;
    showLife = 1;
    round_num = 0;
    cellsInit();
}
void withInput(){
    char input;
    if(kbhit()){//判断是否有输入
        input = getch();
        //根据输入操作
        switch(input){
            case '+':// 加速（减小延时）
                if(speed > 2)
                speed /= 2;
                break;
            case '-':// 减速（增大延时）
                if(speed < 2000)
                speed *= 2;
                break;
            case ' ': // 切换暂停/继续
                paused = !paused;
                break;
            case 'r': // 重新随机化画面
                cellsInit();
                break;
            case 'h': // 隐藏、显示生命
                showLife = !showLife;
                break;
        }
        //限制速度边界
        if(speed < 2){
            speed = 2;
        }
        else if(speed > 2000){
            speed = 2000;
        }
    }
}
void withoutInput(){
    int x,y;
    CELL tempCells[WIDTH][HIGH];
    if(!paused){//未暂停则计算
        //记录轮数增长
        round_num++;
        //临时数组和原数组匹配
        for(y = 0;y < HIGH;y++){
            for(x = 0;x < WIDTH;x++){
                tempCells[x][y] = cells[x][y];
            }
        }
        //正式计算
        for(y = 0;y < HIGH;y++){
            for(x = 0;x < WIDTH;x++){
                //年龄变化与老死
                if(cells[x][y].age > 10){//十岁时死亡
                    tempCells[x][y].life = 0;
                }
                if(cells[x][y].life){
                    tempCells[x][y].age++;
                }
                else{
                    tempCells[x][y].age = 0;
                }
                //统计各种邻居数量
                int plants, herbivores, predators;
                plants = countNeibour(x,y,1);
                herbivores = countNeibour(x,y,2);
                predators = countNeibour(x,y,3);
                //分类地形
                switch(cells[x][y].area){
                    case 1://普通地形
                        if(!cells[x][y].life){//空地
                            if(plants == 3){
                                tempCells[x][y].life = 1;
                                tempCells[x][y].role = 1;//变为植物
                            }
                            else if(herbivores == 2 && plants){
                                tempCells[x][y].life = 1;
                                tempCells[x][y].role = 2;//变为草食动物
                            }
                            else if(predators == 2 && herbivores){
                                tempCells[x][y].life = 1;
                                tempCells[x][y].role = 3;//变为肉食动物
                            }
                        }
                        else{
                            if(cells[x][y].role == 1){//植物
                                if(plants < 2 || plants > 3 || herbivores){
                                    tempCells[x][y].life = 0;//原版规则+被吃掉
                                }
                            }
                            else if(cells[x][y].role == 2){//草食动物
                                if(plants < 2 || herbivores > 3 || predators){
                                    tempCells[x][y].life = 0;//饿死+拥挤+被吃掉
                                }
                            }
                            else if(cells[x][y].role == 3){//肉食动物
                                if(herbivores < 2 || predators > 3){
                                    tempCells[x][y].life = 0;//饿死+拥挤
                                }
                            }
                        }
                        break;
                    case 2://水源（易于繁殖）
                         if(!cells[x][y].life){//空地
                            if(plants == 2){
                                tempCells[x][y].life = 1;
                                tempCells[x][y].role = 1;//变为植物
                            }
                            else if(herbivores == 2 && plants){
                                tempCells[x][y].life = 1;
                                tempCells[x][y].role = 2;//变为草食动物
                            }
                            else if(predators == 2 && herbivores){
                                tempCells[x][y].life = 1;
                                tempCells[x][y].role = 3;//变为肉食动物
                            }
                        }
                        else{
                            if(cells[x][y].role == 1){//植物
                                if(plants < 1 ||plants > 4 || herbivores){
                                    tempCells[x][y].life = 0;//原版规则+被吃掉
                                }
                            }
                            else if(cells[x][y].role == 2){//草食动物
                                if(plants < 1 || herbivores > 4 || predators){
                                    tempCells[x][y].life = 0;//饿死+拥挤+被吃掉
                                }
                            }
                            else if(cells[x][y].role == 3){//肉食动物
                                if(herbivores < 1 || predators > 4){
                                    tempCells[x][y].life = 0;//饿死+拥挤
                                }
                            }
                        }
                        break;
                    case 3://沙漠（不易繁殖）
                         if(!cells[x][y].life){//空地
                            if(plants == 4){
                                tempCells[x][y].life = 1;
                                tempCells[x][y].role = 1;//变为植物
                            }
                            else if(herbivores == 2 && plants){
                                tempCells[x][y].life = 1;
                                tempCells[x][y].role = 2;//变为草食动物
                            }
                            else if(predators == 2 && herbivores){
                                tempCells[x][y].life = 1;
                                tempCells[x][y].role = 3;//变为肉食动物
                            }
                        }
                        else{
                            if(cells[x][y].role == 1){//植物
                                if(plants < 2 || plants > 2 || herbivores){
                                    tempCells[x][y].life = 0;//原版规则+被吃掉
                                }
                            }
                            else if(cells[x][y].role == 2){//草食动物
                                if(plants < 3 || herbivores > 2 || predators){
                                    tempCells[x][y].life = 0;//饿死+拥挤+被吃掉
                                }
                            }
                            else if(cells[x][y].role == 3){//肉食动物
                                if(herbivores < 3 || predators > 2){
                                    tempCells[x][y].life = 0;//饿死+拥挤
                                }
                            }
                        }
                        break;
                    default:
                        break;
                }             
            }
        }
        //更新原数组
        for(y = 0;y < HIGH;y++){
            for(x = 0;x < WIDTH;x++){
                cells[x][y] = tempCells[x][y];
            }
        }
        Sleep(speed);
    }
}
void show(){
    gotoxy(0,0);
    int x,y;
    for(y = 0;y < HIGH;y++){
        for(x = 0;x < WIDTH;x++){
            if(cells[x][y].life && showLife){
                switch (cells[x][y].role){
                case 1:
                    printf("M");//植物
                    break;
                case 2:
                    printf("Y");//草食动物
                    break;
                case 3:
                    printf("C");//肉食动物
                    break;
                default:
                    break;
                }
            }
            else{
                switch(cells[x][y].area){
                    case 1:
                        printf(" ");//普通地形
                        break;
                    case 2:
                        printf("~");//水源
                        break;
                    case 3:
                        printf(".");//沙漠
                        break;
                    default:
                        break;
                }
            }
        }
        printf("\n");
    }
    printf("Year %d.",round_num);
}
int countNeibour(int x,int y,int role){
    int count = 0;
    int i,j;
    //处理角落情况
    if(x == 0 && y == 0){//左上角
        for(j = y;j <= y + 1;j++){
            for(i = x;i <= x + 1;i++){
                if(i == x && j == y){
                    continue;
                }
                if(cells[i][j].role == role){
                    count += cells[i][j].life;
                }
            }
        }
    }
    else if(x == WIDTH - 1 && y == 0){//右上角
        for(j = y;j <= y + 1;j++){
            for(i = x - 1;i <= x;i++){
                if(i == x && j == y){
                    continue;
                }
                if(cells[i][j].role == role){
                    count += cells[i][j].life;
                }
            }
        }
    }
    else if(x == 0 && y == HIGH - 1){//左下角
        for(j = y - 1;j <= y;j++){
            for(i = x;i <= x + 1;i++){
                if(i == x && j == y){
                    continue;
                }
                if(cells[i][j].role == role){
                    count += cells[i][j].life;
                }
            }
        }
    }
    else if(x == WIDTH - 1 && y == HIGH - 1){//右下角
        for(j = y - 1;j <= y;j++){
            for(i = x - 1;i <= x;i++){
                if(i == x && j == y){
                    continue;
                }
                if(cells[i][j].role == role){
                    count += cells[i][j].life;
                }
            }
        }
    }
    //处理边缘情况
    else if(x == 0 && y != 0 && y != HIGH - 1){//左边缘
        for(j = y - 1;j <= y + 1;j++){
            for(i = x;i <= x + 1;i++){
                if(i == x && j == y){
                    continue;
                }
                if(cells[i][j].role == role){
                    count += cells[i][j].life;
                }
            }
        }
    }
    else if(x == WIDTH - 1 && y != 0 && y != HIGH - 1){//右边缘
        for(j = y - 1;j <= y + 1;j++){
            for(i = x - 1;i <= x;i++){
                if(i == x && j == y){
                    continue;
                }
                if(cells[i][j].role == role){
                    count += cells[i][j].life;
                }
            }
        }
    }
    else if(y == 0 && x != 0 && x != WIDTH - 1){//上边缘
        for(j = y;j <= y + 1;j++){
            for(i = x - 1;i <= x + 1;i++){
                if(i == x && j == y){
                    continue;
                }
                if(cells[i][j].role == role){
                    count += cells[i][j].life;
                }
            }
        }
    }
    else if(y == HIGH - 1 && x != 0 && x != WIDTH - 1){//下边缘
        for(j = y - 1;j <= y;j++){
            for(i = x - 1;i <= x + 1;i++){
                if(i == x && j == y){
                    continue;
                }
                if(cells[i][j].role == role){
                    count += cells[i][j].life;
                }
            }
        }
    }
    //正常情况
    else{
        for(j = y - 1;j <= y + 1;j++){
            for(i = x - 1;i <= x + 1;i++){
                if(i == x && j == y){
                    continue;
                }
                if(cells[i][j].role == role){
                    count += cells[i][j].life;
                }
            }
        }
    }
    return count;
    /*
        |(i-1,j-1)|(i-1,j)|(i-1,j+1)|
        | (i,j-1) | (i,j) | (i,j+1) |
        |(i+1,j-1)|(i+1,j)|(i+1,j+i)|
    */
}
void cellsInit(){
    int x,y;
    int i;
    round_num = 0;
    //按面积分配地形（普通地形5：水源3：沙漠2）
    int water_patches = rand() % 3 + 2;//2~4个水源区域
    int desert_patches = rand() % 3 + 1;//1~3个沙漠区域
    AREA *water = (AREA *)malloc(water_patches * sizeof(AREA));
    AREA *desert = (AREA *)malloc(desert_patches * sizeof(AREA));
    for(i = 0;i < water_patches;i++){
        water[i].x = rand() % WIDTH;
        water[i].y = rand() % HIGH;
        water[i].radius = rand() % (int) sqrt(WIDTH * HIGH / (3 * PI)) + 1;
    }
    for(i = 0;i < desert_patches;i++){
        desert[i].x = rand() % WIDTH;
        desert[i].y = rand() % HIGH;
        desert[i].radius = rand() % (int) sqrt(WIDTH * HIGH / (5 * PI)) + 1;
    }
    /* 
        ∵ s * ratio  = PI * r^2 
        ∴ r = sqrt(s / (PI * ratio) 
    */
    //初始化所有细胞
    for(y = 0;y < HIGH;y++){
        for(x = 0;x < WIDTH;x++){
            //年龄初始化
            cells[x][y].age = 0;
            //按位置分配地形
            if(checkArea(x,y,water,water_patches)){
                cells[x][y].area = 2;//水源（易于繁殖）
            }
            else if(checkArea(x,y,desert,desert_patches)){
                cells[x][y].area = 3;//沙漠（不易繁殖）
            }
            else{
                cells[x][y].area = 1;//普通地形
            }
            //随机生死
            if(cells[x][y].area == 1){
                cells[x][y].life = rand() % 2;
            }
            else if(cells[x][y].area == 2){//清空水源地，填满沙漠
                cells[x][y].life = 0;
            }
            else{
                cells[x][y].life = 1;
            }
            //有生命时按比例随机角色（植物5：草食动物3：肉食动物2）
            if(cells[x][y].life){
                int weight = rand() % 10 + 1;
                if(weight < 6){
                    cells[x][y].role = 1;//植物
                }
                else if(weight < 9){
                    cells[x][y].role = 2;//草食动物
                }
                else{
                    cells[x][y].role = 3;//肉食动物
                }
            }
        }
    }
}
double dist(int x1,int y1,int x2,int y2){
    double result;
    result = sqrt(pow((x1 - x2),2) + pow((y1 - y2),2));
    return result;
}
int checkArea(int x,int y,AREA *areas,int area_count){
    int i;
    for(i = 0;i < area_count;i++){
        if(dist(areas[i].x,areas[i].y,x,y) < areas[i].radius){
            return 1;
        }
    }
    return 0;
}