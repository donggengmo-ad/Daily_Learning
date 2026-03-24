/*
? 题目
    #stem
    OtterZ set up a battle with 𝑛 monsters in order to increase his combat power. 
    Each monster has a combat power 𝑎𝑖 
    and OtterZ has a combat power of 𝑐.
    He has 𝑘 flip flops and can perform the following operations:

    Kill an alive monster 𝑖 if 𝑎𝑖≤𝑐; then 𝑐 becomes 𝑐+𝑎𝑖.
    Throw a flip flop at an alive monster 𝑖; 
    the flip-flop will be broken and the monster will become angrier, then 𝑎𝑖 becomes 𝑎𝑖+1.
    Help OtterZ obtain the maximum possible 𝑐 after the battle.
    
    #Input
    Each test contains multiple test cases. 
    The first line contains the number of test cases 𝑡 (1≤𝑡≤500). 
    The description of the test cases follows.

    The first line of each test case contains three integers
     𝑛, 𝑐 and 𝑘 (1≤𝑛≤100, 0≤𝑐,𝑘≤10^9).

    The second line contains 𝑛
    integers 𝑎1,𝑎2,…,𝑎𝑛 (0≤𝑎𝑖≤10^9).

    #Output
    For each test case, output an integer — the maximum possible combat power.

? 思路
    *概括：杀怪可吞战力，扔拖鞋提升怪物战力，凑出最大战力
    $核心：本质上是要杀死尽可能多的怪物，并且将怪物战力提升到与自己最小差距
    %步骤：
        1. 将怪物战力排序
        2. 从小到大遍杀怪
        3. 杀怪前扔出尽可能多拖鞋（尽量提高战力）
        4. 直到战力不足或怪物杀完
    #复杂度：
        时间复杂度：O(nlogn)
        空间复杂度：O(n)
*/

#include <stdio.h>
#include <stdlib.h>

int cmp(const void *a, const void *b){
    return *(long long*)a - *(long long*)b;
}

int main(){
    //输入轮数
    int t;
    scanf("%d", &t);
    //每轮
    for(int rounds = 0; rounds < t; rounds++){
        //输入仨数
        long long n,c,k;
        scanf("%lld %lld %lld", &n, &c, &k);
        long long *a = new long long[n];
        //输入所有战力
        for(int i=0; i<n; i++) scanf("%lld", &a[i]);
        //排序
        qsort(a, n, sizeof(long long), cmp);
        //从小到大遍历
        for(int i=0; i<n; i++){
            //能打过
            if(a[i] <= c) {
                //扔拖鞋 (战力差&剩余拖鞋数的较小值)
                long long delta = (c-a[i]) > k ? k : (c-a[i]);
                a[i] += delta;
                k -= delta;
                //打怪
                c += a[i];
            }
            //如果战力不足，停止
            else break; 
        }
        //输出结果
        printf("%lld\n", c);
        delete[] a;
    }
    return 0;
}