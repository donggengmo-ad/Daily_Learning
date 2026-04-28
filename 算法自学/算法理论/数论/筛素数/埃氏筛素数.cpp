#include <iostream>
#define MAXN int(1e4+10)
using namespace std;

//@ 落谷P3383
/*
_   维护一个素数标记列，将素数的所有倍数筛掉
%   p = {pi | pi 是素数且 pi <= n}（所有素数）
%   t = {ti | ti 属于N*且 ti <= n}（所有倍数）
#  双层循环逻辑
    筛选掉所有p * t
    外层遍历t，内层遍历p
    被外层筛掉的p不会被纳入内层遍历
*/
int main(){
    int n, q;
    cin >> n >> q;
    // 筛选(0,n]素数
    int prime[MAXN];
    bool is_prime[MAXN];
    int idx = 0; // prime数列的长度
    // 全部设为素数
    memset(is_prime, true, sizeof(is_prime));
    // 遍历所有数
    for(int i = 2;i < n;i++){
        // 当前是素数
        if(is_prime[i]){
            // 记录素数
            prime[idx++] = i;
            // 筛掉倍数
            for(int j = 2;i * j < n;j++){
                is_prime[i * j] = false;
            }
        }
    }
    // 输出查询结果
    for(int i = 0;i < q;i++){
        int x;
        cin >> x;
        if(x <= n) cout << prime[x-1] << "\n";
    }
    return 0;
}
