#include <stdio.h>
#include <stdlib.h>
typedef struct node{
    int data;
    struct node *next;
}NODE;
void print(NODE *h){
    while(h != NULL){
        printf("%d ",h -> data);
        h = h -> next;
    }
    printf("\n");
}
NODE *create(){
    NODE *tail,*new,*h = NULL;
    int x;
    while(1){
        scanf("%d",&x);
        if(x == -1){
            break;
        }
        if(h == NULL){
            h = (NODE*) malloc(sizeof(NODE));
            tail = h;
            tail -> next = NULL;
            tail -> data = x;
        }
        else{
            new = (NODE*) malloc(sizeof(NODE));
            new -> next = NULL;
            new -> data = x;
            tail -> next = new;
            tail = new;
        }
    }
    return h;
}
NODE *insert(NODE *h,int x){
    NODE *p,*q,*f = NULL;
    q = h;
    p = (NODE*) malloc(sizeof(NODE));
    p -> data = x;
    p -> next = NULL;
    while(q != NULL && q -> data < x){
        f = q;
        q = q -> next;
    }
    p -> next = q;
    if(h == q){
        h = p;
    }
    else{
        f -> next = p;
    }
    return h;
}
NODE *delete(NODE *h,int x){
    NODE *p,*f;
    p = h;
    while(p != NULL && p -> data != x){
        f = p;
        p = p -> next;
    }
    if(p != NULL){
        if(p == h){
            h = h -> next;
        }
        else{
            f -> next = p -> next;
        }
        free(p);
    }
    return h;
}
int main(){
    int x;
    NODE *H = NULL;
    H = create();
    print(H);
    scanf("%d",&x);
    H = insert(H,x);
    print(H);
    scanf("%d",&x);
    H = delete(H,x);
    print(H);
    return 0;
}