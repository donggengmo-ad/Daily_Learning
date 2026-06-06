#include <vector>
using namespace std;
using ll = long long;

// 基础树状数组（维护前缀和为例）
struct BIT{
    int len;
    vector<ll> t;
    BIT(){}
    BIT(int n): t(n + 1, 0), len(n) {}
    // a 必须是 1-based
    void build(const vector<ll>& a){
        for(int i = 1;i <= len;i++) t[i] = a[i];
        for(int x = 1;x <= len;x++){
            int y = x + (x & -x);
            if(y <= len) t[y] += t[x];
        }
    }
    // 单点加
    void add(int x, ll k){
        while(x <= len) t[x] += k, x += x & -x;
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

// 差分树状数组
struct DiffBIT{
    BIT d;
    DiffBIT(int n): d(n) {}
    // a 必须是 1-based，且 a[0] = 0
    void build(const vector<ll>& a){
        vector<ll> dif(d.len + 1, 0);
        for(int i = 1;i <= d.len;i++) dif[i] = a[i] - a[i-1];
        d.build(dif);
    }
    // 区间加
    void add(int l, int r, ll k){
        d.add(l, k);
        d.add(r + 1, -k);
    }
    // 单点查询
    ll query(int x){
        return d.query(x);
    }
};

// 双树树状数组
struct DoubleBIT{
    // f 维护 d[i] 前缀和，g 维护 d[i] * i 前缀和（d为差分数组）
    BIT f, g;
    DoubleBIT(int n): f(n), g(n) {}
    // a 必须是 1-based，且 a[0] = 0
    void build(const vector<ll>& a){
        vector<ll> d(f.len + 1, 0), di(g.len + 1, 0);
        for(int i = 1; i <= f.len; i++)
            d[i] = a[i] - a[i-1], di[i] = i * d[i];
        f.build(d);
        g.build(di);
    }
    // 区间加
    void add(int l, int r, ll k){
        f.add(l, k);
        f.add(r + 1, -k);
        g.add(l, k * l);
        g.add(r + 1, -k * (r + 1));
    }
    // 单点加
    void add(int x, ll k){
        add(x, x, k);
    }
    // 前缀和查询
    ll prefix(int x){
        return (x + 1) * f.query(x) - g.query(x);
    }
    // 区间和查询
    ll range(int l, int r){
        return prefix(r) - prefix(l-1);
    }
    // 单点查询
    ll point(int x){
        return f.query(x);
    }
};



