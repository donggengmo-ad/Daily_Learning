#include <iostream>
#include <vector>
#include <cstring>
#define MAXN int(1e4+10)
using namespace std;

//@ 落谷P3383
/* 
_维护一素数列，将素数列中的每个素数的每个倍数筛掉
%   p = {pi | pi 是素数且 pi <= n}（所有素数）
%   t = {ti | ti 属于N*且 ti <= n}（所有倍数）
# 双层循环逻辑
    筛选掉所有p * t
    然而有些数被筛选两次
@   eg.
/   p = {2,3,5,7,11,...}
/   t = {1,2,3,4,5,...}
/       p[1] * t = 2 * {1,2,3,4,5,...} = {2,4,6,8,10,...}
/       p[3] * t = 5 * {1,2,3,4,5,...} = {5,10,15,20,25,...}
!   10出现了两次
/   这是因为：
/       p[1] = t[2] = 2
/       p[3] = t[5] = 5
/       p[1] * t[5] = 2 * 5 = 10
/       p[3] * t[2] = 5 * 2 = 10
%   当存在 a,b,c,d (a<c,b<d) 满足 p[a] = t[b], p[c] = t[d] 时，
%   会导致 p[a] * t[d] = p[c] * t[b]，该数被筛掉两次
*    因此外层遍历t，内层遍历p，外层边遍历边添加素数
    遍历到 t[b] 时，p[c] 还没有被添加到p数列，t[b] * p[c] 不起作用
    遍历到 t[d] 时，p[a] 已经被添加到p数列，t[d] * p[a] 起作用
    只筛选一次 
# 跳出时机
_   i * p[j] > n 时，跳出循环
*   要筛掉的数超过了搜索范围
    肯定不用继续了呀

_   i % p[j] == 0 时，跳出循环
*   p[j] 是 i 的因数，
%   设i = p[j] * m，取p[k] (k>j) 
%   p[k] * i = p[k] * (p[j] * m) = p[j] * (p[k] * m)，
    该数已经被p[j]筛掉了，不需要再被p[k]筛选

*/
int main(){
    ios::sync_with_stdio(0);
    int n,q;
    cin >> n >> q;
    // 筛选(0,n]素数
    int prime[MAXN];
    bool is_prime[MAXN];
    int idx = 0; // prime数列的长度
    // 全部设为素数
    memset(is_prime, true, sizeof(is_prime));
    // 遍历所有倍数t
    for(int i = 2;i < n;i++){
        // 记录素数
        if(is_prime[i]){
            prime[idx++] = i;
        }
        // 遍历所有素数p
        for(int j = 0;j < idx;j++){
            int p = prime[j];
            if(i * p > n) break;
            is_prime[i * p] = false;
            if(i % p == 0) break;
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
 