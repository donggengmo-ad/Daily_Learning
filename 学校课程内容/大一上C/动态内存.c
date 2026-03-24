#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
    int n1,n2;
    scanf("%d %d",&n1,&n2);
    char *p1 = (char*)malloc(sizeof(char) * n1);
    char *p2 = (char*)malloc(sizeof(char) * n2);
    scanf("%s",p1);
    scanf("%s",p2);
    if(*p1 < sizeof(char) * strlen(p1) * strlen(p2)){
        *p1 = sizeof(char) * strlen(p1) * strlen(p2);
    }
    strcat(p1,p2);
    printf("%s",p1);
    free(p1);
    free(p2);
    return 0;
}