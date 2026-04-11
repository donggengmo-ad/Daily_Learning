#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

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

    vector<i128> bounas(m + 1);
    bounas[0] = 1;
    for(int i = 1;i <= m;i++) bounas[i] = bounas[i-1] * 2;
    
    i128 ans = 0;
    for(int i = 0;i < n;i++){
        vector<vector<i128>> dp(m + 1, vector<i128>(m + 1));
        for(int len = 1;len <= m;len++){
            for(int l = 0;l <= m - len;l++){
                int r = l + len - 1;
                if(l == r) {
                    dp[l][r] = M[i][l] * bounas[m-len+1];
                    continue;
                }
                i128 left = dp[l+1][r] + M[i][l] * bounas[m-len+1];
                i128 right = dp[l][r-1] + M[i][r] * bounas[m-len+1];
                dp[l][r] = max(left, right);
            }
        }
        ans += dp[0][m-1];
        
    }
    printi128(ans);
}
