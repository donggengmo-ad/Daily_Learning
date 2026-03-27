/*
? 题目
    #stem
    You are given an array 𝑎 of length 𝑛.
    For each index 𝑖, find the maximum number of indices 𝑗,
    such that 𝑗>𝑖 and |𝑎𝑖−𝑘|>|𝑎𝑗−𝑘|, over all possible integer values of 𝑘.

    #Input
    Each test contains multiple test cases. 
    The first line contains the number of test cases 𝑡 (1≤𝑡≤100). 
    The description of the test cases follows.

    The first line of each test case contains an integer 𝑛(1≤𝑛≤5000).
    The second line contains 𝑛 integers 𝑎1,𝑎2,…,𝑎𝑛(−109≤𝑎𝑖≤109).

    It is guaranteed that the sum of 𝑛 over all test cases does not exceed 5000.

    #Output
    For each test case, output 𝑛 integers denoting the answer.

? 思路
    *概括：对每个i，找k使|a[i]-k|>|a[j]-k|（j>i）的j最多，记j数
    $核心：k=a[i]是最劣解，k距a[i]越远越优，最优只有k>max或k<min两种情况
    !注意：区分距离和密度，距离远不一定区间内点多，所以不能找差最大的，要找区间内点最多的
    %步骤：
        1. 遍历每个i
        2. 找i后最大、最小值，分别统计j数
        3. 输出较大j数
    #复杂度：
        时间复杂度：O(n^2)，
        空间复杂度：O(n)
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(){
    //输入轮数
    int t;
    scanf("%d", &t);
    //每轮
    for(int rounds = 0; rounds < t; rounds++){
        //输入n
        int n;
        scanf("%d", &n);
        int *a = new int[n];
        //输入所有数
        for(int i=0; i<n; i++) scanf("%d", &a[i]);
        //遍历每个i
        for(int i=0; i<n; i++){
            //找i后最大、最小值
            int max = a[i];
            int min = a[i];
            for(int j=i+1; j<n; j++){
                if(a[j] > max) max = a[j];
                if(a[j] < min) min = a[j];
            }
            //分别统计j数
            int count_max = 0;
            int count_min = 0;
            for(int j=i+1; j<n; j++){
                if(abs(a[i]-max) > abs(a[j]-max)) count_max++;
                if(abs(a[i]-min) > abs(a[j]-min)) count_min++;
            }
            //输出较大j数
            int count = count_max > count_min ? count_max : count_min;
            printf("%d ", count);
        }
        printf("\n");
        delete[] a;
    }
}