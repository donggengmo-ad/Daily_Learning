#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
int m, n;
vector<int> a;
bool takable(int x){
    int taken = 0, cur = 0;
    for(int i = 1;i <= n + 1;i++){
        if(a[i] - a[cur] >= x) cur = i;
        else taken++;
    }
    return m >= taken;
}
int main(){
    int len;
    cin >> len >> n >> m;
    a.assign(n + 2, 0);
    for(int i = 1;i <= n;i++) cin >> a[i];
    a[n + 1] = len;
    int l = 1, r = n + 1, mid = n / 2;
    while(l < r){
        mid = l + (r - l + 1) / 2;
        if(takable(mid)) l = mid;
        else r = mid - 1;
    }
    cout << l;
    return 0;
}