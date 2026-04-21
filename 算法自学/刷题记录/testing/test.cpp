#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
using ll = long long;
vector<int> add(vector<int> a, vector<int> b){
    int n = max(a.size(), b.size());
    reverse(a.begin(), a.end());
    reverse(b.begin(), b.end());
    vector<int> res;
    for(int i = 0;i < n;i++){
        if(a.size() <= i) res.push_back(b[i]);
        else if(b.size() <= i) res.push_back(a[i]);
        else res.push_back(a[i] + b[i]);
    }
    for(int i = 0;i < res.size();i++){
        if(res[i] < 10) continue;
        res[i] %= 10;
        if(i < n - 1) res[i+1]++;
        else res.push_back(1);        
    }
    reverse(res.begin(), res.end());
    return res;
}
ll mod(vector<int> a, ll modn){
    ll res = 0;
    for(int i:a){
        res = (res * 10 + i) % modn;
    }
    return res;
}
int main(){
    vector<int> a = {1,0,0,0,0,0,7};
    ll res = mod(a, 1000000);
    cout << res;
    return 0;
}