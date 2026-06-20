#include <stdio.h>
int main(){
    int letter[26]={0},i = 0;
    char ch;
    while(1){
        ch = getchar();
        if(ch == '\n')
            break;
        if('A' <= ch && ch <= 'Z')
            letter[ch - 'A'] += 1;
        else if('a' <= ch && ch <= 'z')
            letter[ch - 'a'] += 1;
    }
    for(i=0;i<26;i++)
        printf("'%c':%d;\n",'A' + i,letter[i]);
    return 0;
}