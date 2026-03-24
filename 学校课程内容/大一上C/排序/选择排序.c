#include <stdio.h>
int main(){
int i,j,k,temp,min;
int data[10];
    for(i = 0;i < 10;i++){
        scanf("%d",&data[i]);
    }
    for(j = 0;j < 10;j++){
        min = j;
        for(k = j + 1;k < 10;k++){
            if(data[k] < data[min]){
                min = k;
            }
        }
        if(min != j){
            temp = data[j];
            data[j] = data[min];
            data[min] = temp;
        }        
    }
    for(i = 0;i < 10;i++){
        printf("%d ",data[i]);
    }
    return 0;
}