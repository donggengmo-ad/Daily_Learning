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
    NODE *h=NULL,*tail_p,*new_p,*p;
    int i;
    for(i = 0;i < NUM;i++){
        if(h == NULL){
            h = (NODE*) malloc(sizeof(NODE));
            h -> next = NULL;
            tail_p = h;
            scanf("%d",&tail_p -> data);
        }
        else{
            new_p = (NODE*) malloc(sizeof(NODE));
            new_p -> next = NULL;
            scanf("%d",&new_p -> data);
            tail_p -> next = new_p;
            tail_p = new_p;
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