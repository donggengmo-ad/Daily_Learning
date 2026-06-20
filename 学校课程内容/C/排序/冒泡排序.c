#include <stdio.h>
#include <string.h>
void sort(int value[],int n){
    int i,j,temp;
    for(i = n - 1;i >= 1;i--){
        for(j = 0;j < i;j++){
            if(value[j] < value[j + 1]){
                temp = value[j];
                value[j] = value[j + 1];
                value[j + 1] = temp;
            }
        }
    }
}
void input(int value[],int n){
    int i;
    for(i = 0;i < n;i++){
        scanf("%d",&value[i]);
    }
} 
void output(int value[],int n){
    int i;
    for(i = 0;i < n;i++){
        printf("%d",value[i]);
    }
} 

int main(){
    int n;
    scanf("%d",&n);
    int a[n];
    input(a,n);
    sort(a,n);
    output(a,n);
    
    return 0;
}