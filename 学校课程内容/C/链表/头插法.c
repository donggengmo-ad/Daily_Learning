#include <stdio.h>
#include <stdlib.h>
#define NUM 5
typedef struct node{
    int data;
    struct node *next;
}NODE;
void print(NODE *h){
    while(h != NULL){
        printf("%d ",h -> data);
        h = h -> next;
    }
}
NODE *create(){
    NODE *h=NULL,*new_p,*p;
    int i;
    for(i = 0;i < NUM;i++){
        if(h == NULL){
            h = (NODE*) malloc(sizeof(NODE));
            h -> next = NULL;
            scanf("%d",&h -> data);
        }
        else{
            new_p = (NODE*) malloc(sizeof(NODE));
            scanf("%d",&new_p -> data);
            new_p -> next = h;
            h = new_p;
        }//省略free
    }
    return h;
}
int main(){
    NODE *H = NULL;
    H = create();
    print(H);
    return 0;
}