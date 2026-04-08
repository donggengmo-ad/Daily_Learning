#include <iostream>
#include <vector>
using namespace std;
int main(){
    int T,m;
    cin>>T>>m;
    int dp[101][1001];
    int t[101], v[101];
    for(int i = 0;i < 101;i++){
        for(int j = 0;j < 1001;j++){
            dp[i][j] = 0;
        }
    }
    for(int i = 1;i <= m;i++){
        cin>>t[i]>>v[i];
    }
    for(int i = 1;i <= m;i++){
        for(int j = 1;j <= T;j++){
            if(t[i] > j){
                dp[i][j] = dp[i-1][j];
            }
            else{
                int act = dp[i-1][j-t[i]] + v[i];
                int rest = dp[i-1][j];
                dp[i][j] = max(act, rest);
            }
        }
    }
    for(int i = 0;i <= m;i++){
        for(int j = 0;j <= T;j++){
            cout<<dp[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<dp[m][T];
    return 0;
}