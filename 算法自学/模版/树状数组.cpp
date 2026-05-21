#include <bits/stdc++.h>
using namespace std;
using ll = long long;

struct BIT{
    vector<ll> t;
    BIT(int n): t(n+1, 0) {}
    void build(const vector<int>& a){
        for(int i = 1; i < a.size(); i++) add(i, a[i]);
    }
    void add(int x, ll k){
        while(x < t.size()) t[x] += k, x += x & -x;
    }
    ll query(int x){
        ll res = 0;
        while(x > 0) res += t[x], x -= x & -x;
        return res;
    }
    ll query(int l, int r){
        return query(r) - query(l-1);
    }
};

int main() {
    return 0;
}