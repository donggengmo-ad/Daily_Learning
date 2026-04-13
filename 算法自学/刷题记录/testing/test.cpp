#include <iostream>
#include <vector>
using namespace std;

int main(){
    int r;
    cin >> r;
    if(r <= 0){
        cout << 0;
        return 0;
    }

    vector<vector<int>> dp(r+1, vector<int>(r+1));
    for(int i = 1;i <= r;i++) 
        for(int j = 1;j <= i;j++) 
            cin >> dp[i][j];
    for(int i = r - 1;i >= 1;i--){
        for(int j = 1;j <= r;j++){
            int left = dp[i+1][j];
            int right = dp[i+1][j+1];
            dp[i][j] += max(left, right);
        }
    }
    cout << dp[1][1];
    
    return 0;
}
