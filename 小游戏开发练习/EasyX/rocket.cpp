#include <graphics.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600
#define PI 3.1415

typedef struct{
    long x;
    long y;
}Vector;
typedef struct{
    double m;//质量
    double w,h;//尺寸
    long x,y;//中心位置
    Vector v;//速度
    double angle;//角度
    double angV;//角速度
    POINT point[5];//顶点
}Object;

Object rocket;

void drawRocket(int push = 0){
    long x = rocket.x;
    long y = rocket.y;
    double ang = rocket.angle;
    POINT p[3] = {{x,y},{x,y},{x,y}};
    //左下点
    double dx = -rocket.w / 2;
    double dy = rocket.h / 3;
    p[0].x += dx * cos(ang) - dy * sin(ang);
    p[0].y += dx * sin(ang) + dy * cos(ang);
    //右下点
    dx = rocket.w / 2;
    p[1].x += dx * cos(ang) - dy * sin(ang);
    p[1].y += dx * sin(ang) + dy * cos(ang);
    //顶部点
    dx = 0;
    dy = -rocket.h * 2 / 3;
    p[2].x += dx * cos(ang) - dy * sin(ang);
    p[2].y += dx * sin(ang) + dy * cos(ang);
    //打印
    setfillcolor(YELLOW);
    setlinecolor(BLACK);
    fillpolygon(p, 3);
    //喷火
    if(push){
        dx = 0;
        dy = rocket.h * 2 / 3;
        p[0].x += dx * cos(ang) - dy * sin(ang);
        p[0].y += dx * sin(ang) + dy * cos(ang);
        setlinecolor(YELLOW);
        setfillcolor(RED);
        fillcircle(p[0].x, p[0].y, 5);
    }
}
void initRocket(){
    rocket.m = 10.0;
    rocket.w = 20;
    rocket.h = 40;
    rocket.x = 50;
    rocket.y = 70;
    rocket.v.x = 0;
    rocket.v.y = 0;
    rocket.angle = 0;
    rocket.angV = 0;
}
void startGame(){
    //初始化画面
    initgraph(WIDTH, HEIGHT);
    setbkcolor(BLACK);
    cleardevice();
    BeginBatchDraw();
    //初始化物体
    initRocket();
}
void endGame(){
    EndBatchDraw();
    closegraph();
}
void show(int push){
    cleardevice();
    drawRocket(push);
    FlushBatchDraw();
    Sleep(100);
}
void update(){
    //更新火箭角度
    rocket.angle += rocket.angV;
    rocket.angV -= rocket.angV * 0.1;//角速度衰减
    //火箭重力加速
    rocket.v.y += 1.5;
    //更新火箭位置
    rocket.x += rocket.v.x;
    rocket.y += rocket.v.y;
    rocket.v.x -= rocket.v.x * 0.01;//速度衰减
    rocket.v.y -= rocket.v.y * 0.01;//速度衰减
    //边界检测
    if(rocket.x < 0){
        rocket.x = 0;
        rocket.v.x = 0;
    }
    if(rocket.x > WIDTH){
        rocket.x = WIDTH;
        rocket.v.x = 0;
    }
    if(rocket.y < 0){
        rocket.y = 0;
        rocket.v.y = 0;
    }
    if(rocket.y > HEIGHT){
        rocket.y = HEIGHT;
        rocket.v.y = 0;
    }
}
int input(){
    int push = 0;
    ExMessage msg;
    if(peekmessage(&msg, EX_KEY)){
        if(msg.vkcode == VK_SPACE){
            push = 1;
                /*
                    外力做功w，按作用力角度分配w给x和y方向
                    Ek = 1/2 * m * (vx^2 + vy^2)
                    w = Ek1 - Ek0
                    Ek1 = w + Ek0

                    vx1^2 + vy1^2 = 2 * w / m + (vx0^2 + vy0^2)
                    tan(theta) * vy1 = vx1
                    => (tan(theta) * vy1)^2 + vy1^2 = 2 * w / m + (vx0^2 + vy0^2)
                    => tan(theta)^2 * vyi^2 + vyi^2 = 2 * w / m + (vx0^2 + vy0^2)
                    => vyi^2 (tan(theta)^2 + 1) = 2 * w / m + (vx0^2 + vy0^2)
                    => vyi = sqrt((2 * w / m + (vx0^2 + vy0^2)) / (tan(theta)^2 + 1))
                    => vxi = tan(theta) * vyi
                */
            double w = 200;
            double theta = rocket.angle;
            double vx0 = rocket.v.x;
            double vy0 = rocket.v.y;
            double m = rocket.m;
            double vy1 = sqrt((2 * w / m + (vx0 * vx0 + vy0 * vy0)) / (tan(theta) * tan(theta) + 1));
            if(tan(theta) < 0){
                vy1 *= -1;
            }
            double vx1 = tan(theta) * vy1;
            rocket.v.x += vx1 - vx0;
            rocket.v.y -= vy1 - vy0;
        }
        if(msg.vkcode == VK_RIGHT){
            /*
                外力做功w，原角速度v0，新角速度v1
                w = m * r^2 * (v1^2 - v0^2)
                v1 = sqrt(w / (m * r^2) + v0^2)
                dv = v1 - v0 = sqrt(w / (m * r^2) + v0^2) - v0
            */
            double w = 10;
            double r = rocket.h * 2 / 3;
            double v0 = rocket.angV;
            double m = rocket.m;
            rocket.angV += sqrt(w / (m * r * r) + v0 * v0) - v0;
        }
        if(msg.vkcode == VK_LEFT){
            double w = 10;
            double r = rocket.h * 2 / 3;
            double v0 = rocket.angV;
            double m = rocket.m;
            rocket.angV -= sqrt(w / (m * r * r) + v0 * v0) - v0;
        }
    }
    return push;
}
int main(){
    startGame();
    while(1){
        int push = input();
        update();
        show(push);
    }
    endGame();
    return 0;
}
