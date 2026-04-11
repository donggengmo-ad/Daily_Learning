#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

//@ 洛谷P1005

// 高精度
using i128 = __int128;
void printi128(i128 x) {
    if(x == 0) {
        cout << 0;
        return;
    }
    string s;
    while(x > 0) {
        s += (x % 10) + '0';
        x /= 10;
    }
    reverse(s.begin(), s.end());
    cout << s;
}

int main(){
    int n,m;
    cin >> n >> m;//n行m列

    //输入矩阵
    vector<vector<int>> M(n, vector<int>(m));
    for(int i = 0;i < n;i++) 
        for(int j = 0;j < m;j++) cin >> M[i][j];
    // 奖励预处理
    vector<i128> bounas(m + 1);
    bounas[0] = 1;
    for(int i = 1;i <= m;i++) bounas[i] = bounas[i-1] * 2;
    // 区间DP
    i128 ans = 0;
    for(int i = 0;i < n;i++){
        // dp[l][r]表示在第i行，剩下区间[l,r]时能获得的最大奖励
        vector<vector<i128>> dp(m + 1, vector<i128>(m + 1));
        // 必须先枚举区间长度，从小到大，这样才能保证状态转移时需要的子问题已经被计算出来了
        for(int len = 1;len <= m;len++){
            // 枚举区间起点
            for(int l = 0;l <= m - len;l++){
                int r = l + len - 1;
                if(l == r) { // 只有一个元素了，直接拿这个元素就行了
                    dp[l][r] = M[i][l] * bounas[m-len+1];
                    continue;
                }
                // 选择拿左边还是右边
                i128 left = dp[l+1][r] + M[i][l] * bounas[m-len+1];
                i128 right = dp[l][r-1] + M[i][r] * bounas[m-len+1];
                dp[l][r] = max(left, right);
            }
        }
        // 最后剩下区间[0,m-1]，就是这一行能获得的最大奖励了
        ans += dp[0][m-1];
        
    }
    // 所有行奖励和
    printi128(ans);
}