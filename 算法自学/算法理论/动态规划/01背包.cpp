#include <iostream>
#include <vector>
using namespace std;

//@ 落谷P1048

/*
    01背包问题
#  状态定义
    dp[i][j]表示 有i件物品 背包剩余j 能获得的最大价值
    v[i]表示第i件物品的价值
    t[i]表示第i件物品的重量
    
#  状态转移
%    act = dp[i-1][j-t[i]] + v[i] 
        选择第i件物品，背包剩余下降t[i]，获得价值v[i]
%    rest = dp[i-1][j] 
        不选择第i件物品，背包剩余不变，获得价值不变
%    dp[i][j] = max(act, rest)

*/
void dp_2dim(){
    int T,M;
    cin>>T>>M;
    // dp数组
    vector<vector<int>> dp(M+1, vector<int>(T+1, 0));
    vector<int> t(M+1), v(M+1);
    // 读取草药信息
    for(int i = 1;i <= M;i++){
        cin>>t[i]>>v[i];
    }
    // 外层循环枚举剩余物品
    for(int i = 1;i <= M;i++){
        // 内层循环枚举剩余背包
        for(int j = 1;j <= T;j++){
            if(t[i] > j){
                // 背包不够，不能选
                dp[i][j] = dp[i-1][j];
            }
            else{
                // 选或不选，取最大
                int act = dp[i-1][j-t[i]] + v[i];
                int rest = dp[i-1][j];
                dp[i][j] = max(act, rest);
            }
        }
    }
    cout<<dp[M][T];
}

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
    内层循环必须倒序枚举剩余背包（从大到小)
    否则会重复选择物品
    /详见完全背包
*/
void dp_1dim(){
    int T,M;
    cin>>T>>M;
    // dp数组
    vector<int> dp(T+1, 0), t(M+1), v(M+1);
    // 读取草药信息
    for(int i = 1;i <= M;i++){
        cin>>t[i]>>v[i];
    }
    // 外层循环枚举剩余物品
    for(int i = 1;i <= M;i++){
        // 内层循环枚举剩余背包
        for(int j = T;j >= t[i];j--){
            dp[j] = max(dp[j], dp[j-t[i]] + v[i]);
        }
    }
    cout<<dp[T];
}

int main(){
    dp_1dim();
    return 0;
}