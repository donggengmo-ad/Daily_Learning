#include <bits/stdc++.h>
using namespace std;
/*
#高精度加法
    在数字过大时，普通数据类型无法存储，
    可以用数组按位存数字，逐位模拟运算。

    ?思路：
       ? 1. 将输入的字符串解析为int数组，按位存储
       ? 2. 从最低位开始逐位计算和，并处理进位
       ? 3. 得到的该位新数反向存入结果数组（因为从最低位开始计算）
       ? 4. 将结果数组反转以得到正确的顺序
*/
#define MAX 501
int main(){
    //按位存储
    int a[MAX] = {0};
    int b[MAX] = {0};
    int c[MAX+1] = {0};
    //获得字符串
    string s1,s2;
    cin>>s1>>s2;
    //将字符串解析为int数组
    for(int i=0;i<s1.size();i++) a[i]=s1[i]-'0';
    for(int i=0;i<s2.size();i++) b[i]=s2[i]-'0';
    //记录空段长度（bd —— back dist即从数组结尾到有效位数结尾的距离）
    int bd1=MAX-s1.size();
    int bd2=MAX-s2.size();
    int bit=0;//结果位数
    for(;bd1<MAX || bd2<MAX;bd1++, bd2++, bit++){
        //当前计算位的索引
        int idx1=MAX-bd1-1;
        int idx2=MAX-bd2-1;
        int at,bt;
        //超过有效位则视为0
        if(idx1<0) at=0;
        else at=a[idx1];
        if(idx2<0) bt=0;
        else bt=b[idx2];
        //计算当前位的和
        int sum=at+bt+c[bit];
        //将当前位的和中个位存入结果数组
        c[bit]=sum%10;
        //将当前位的和中十位存入结果数组的下一位（进位）
        c[bit+1]+=sum/10;
    }
    //如果最后进位了，位数+1
    if(c[bit]!=0) bit++;
    //将结果数组反转以得到正确的顺序
    reverse(c,c+bit);
    for(int i=0;i<bit;i++) cout<<c[i];
    return 0;
}