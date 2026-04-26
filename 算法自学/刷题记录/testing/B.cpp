#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
using ll = long long;
void solve(){
    int n, m, xodd = 0, xeven = 0;
    ll sum = 0;
    cin >> n >> m;
    vector<int> odd, even;
    for(int i = 1;i <= n;i++) {
        int a;
        cin >> a;
        if(i & 1) odd.push_back(a);
        else even.push_back(a);
    }
    for(int i = 0;i < m;i++) {
        int x;
        cin >> x;
        if(x & 1) xodd++;
        else xeven++;
    }
    sort(odd.begin(), odd.end());
    sort(even.begin(), even.end());
    for(int i = 0;i < (int)odd.size() - xodd || 
        (i < (int)odd.size() - 1 && odd[i] < 0);i++) 
            sum += odd[i];
    for(int i = 0;i < (int)even.size() - xeven || 
        (i < (int)even.size() - 1 && even[i] < 0);i++) 
            sum += even[i];
    cout << sum << endl;
}
int main(){
    int t;
    cin >> t;
    while(t--) solve();
    return 0;
}