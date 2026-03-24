#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
typedef struct Point{
    int x;
    int y;
}point;
double get_distance(point p1,point p2);
int main(){
    point p1,p2;
    scanf("%d %d %d %d",&p1.x,&p1.y,&p2.x,&p2.y);
    printf("The distance is %.2lf\n",get_distance(p1,p2));
    return 0;
}
double get_distance(point p1,point p2){
    double d;
    d = sqrt(pow((p1.x - p2.x),2) + pow((p1.y - p2.y),2));
    return d;
}