#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
int x = 0,
    y = 0;
int v_x = 1,
    v_y = 1;
int top = 0,
    bottom = 14,
    left = 0,
    right = 31;
void printBall(int x,int y){
    int i;
    for(i = 0;i < y;i++){
            printf("\n");
        }
    for(i = 0;i < x;i++){
        printf(" ");
    }
    printf("o");
    printf("\r");
}
void bounce(int x,int y){
    if(x == left || x == right){
        v_x *= -1;
        printf("\a");
    }
    if(y == top || y == bottom){
        v_y *= -1;
        printf("\a");
    }
}
void printEdge(){
    int i,j;
    for(i = top;i <= bottom;i++){
        printf("|");
        for(j = left;j < right - 1;j++){
            if(i == top || i == bottom){
                printf("-");
            }
            else{
                printf(" ");
            }
        }
        printf("|");
        printf("\n");
    }
}
int main(){
    while(1){
        system("cls");
        x += v_x;
        y += v_y;
        bounce(x,y);
        printEdge();
        printf("\033[H");
        printBall(x,y);
        Sleep(50);
    }
    return 0;
}