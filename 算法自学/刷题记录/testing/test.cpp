#include <iostream>
#include <vector>
using namespace std;

/*
    dp[i] 取到第i次时的最大收益
*/

int main(){
    int n,m;
    cin >> n >> m; // n行m列
    vector<vector<long long>> M(n, vector<long long>(m, 0));
    vector<long long> dp(n * m, 0);
    //输入矩阵
    for(int y = 0;y < n;y++){
        for(int x = 0;x < m;x++) cin >> M[y][x];
    }
    int count = 1;
    
}