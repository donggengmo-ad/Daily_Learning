#include <stdio.h>
#include <stdlib.h>
typedef struct NODE{
    int data;
    struct NODE *next;
}node;
int main(){
    node a={1,NULL};
    node b={2,NULL};
    node *p,*H;
    p = &a;
    H = &a;
    printf("%d\n",p -> data);
    a.next = &b; //相当于p -> next = &b;
    for(p = H;p != NULL;p = p -> next){
        printf("%d ",p -> data);
    }
    return 0;
}