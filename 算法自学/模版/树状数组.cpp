#include <iostream>
#include <vector>
using namespace std;
using ll = long long;

// 维护前缀和模板
struct BIT{
    vector<ll> t;
    BIT(int n): t(n + 1, 0) {}
    // a 必须是 1-based
    void build(const vector<int>& a){
        for(int i = 1; i < a.size(); i++) add(i, a[i]);
    }
    void add(int x, ll k){
        while(x < t.size()) t[x] += k, x += x & -x;
    }
    // 单点前缀
    ll query(int x){
        ll res = 0;
        while(x > 0) res += t[x], x -= x & -x;
        return res;
    }
    // 区间
    ll query(int l, int r){
        return query(r) - query(l-1);
    }
};

int main() {
    return 0;
}