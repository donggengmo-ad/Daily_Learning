#include <iostream>
#include <vector>
using namespace std;
vector<int> a;
int f(int len){
    int res = 0;
    for(int i = 0;i < a.size();i++) res += a[i] / len;
    return res;
}
int main(){
    int n, k, maxl = 0;
    cin >> n >> k;
    a.assign(n, 0);
    for(int i = 0;i < n;i++) cin >> a[i], maxl = max(maxl, a[i]);
    int l = 1, r = maxl, mid;
    while(l < r){
        mid = l + (r - l + 1) / 2;
        if(f(mid) >= k) l = mid;
        else r = mid - 1;
    }
    if(f(l) != k) l = 0;
    cout << l << endl;
    return 0;
}