#include <iostream>
#include <vector>
using namespace std;

//@ 落谷P1616

/*
    一维dp版本：
#  状态定义
    dp[j]表示背包剩余j 能获得的最大价值
#  状态转移
%   act = dp[j-t[i]] + v[i]
    选择第i件物品，背包剩余下降t[i]，获得价值v[i]
%   rest = dp[j]
    不选择第i件物品，背包剩余不变，获得价值不变
%   dp[j] = max(act, rest)
#  注意事项
    内层循环必须正序枚举剩余背包（从小到大)
    否则物品只会选取一次
@   eg.
%    j1 < j2
    dp[j1] = max(dp[j1], dp[j1-t[i]] + v[i])
    dp[j2] = max(dp[j2], dp[j2-t[i]] + v[i])
%    正序枚举背包:
        dp[j1]在dp[j2]之前被更新，
*        j1选择时可能拿物品i，j2选择时可以再拿
%    倒序枚举背包：
        dp[j1]在dp[j2]之后才更新，
*        j2选择时j1还没选，还是上次状态

*/

int main(){
    long long M, T;
    cin >> T >> M;

    vector<long long> dp(T+1, 0), t(M+1), v(M+1);
    for(long long i = 1;i <= M;i++) cin >> t[i] >> v[i];
    
    for(long long i = 1;i <= M;i++){
        for(long long j = t[i];j <= T;j++){
            dp[j] = max(dp[j], dp[j-t[i]] + v[i]);
        }
    }

    cout << dp[T];
    
    return 0;
}