#include <vector>
using namespace std;
using ll = long long;

// 维护区间和为例
struct SegTree{
    struct Node{
        ll v = 0, lz = 0;
        int l = 0, r = 0;
    };
    vector<Node> t;
    SegTree(int n): t(n * 4 + 1){}
    // a 必须是 1-based long long 数组
    void build(vector<ll> &a, int l = 0, int r = 0, int u = 1){
        if(l == 0 && r == 0) l = 1, r = a.size() - 1;
        if(l == r){
            t[u].v = a[l];
            t[u].l = l, t[u].r = r;
            return;
        }
        int mid = (l + r) >> 1;
        build(a, l, mid, u*2);
        build(a, mid + 1, r, u*2+1);
        t[u].v = t[u*2].v + t[u*2+1].v;
        t[u].l = l, t[u].r = r;
    }
    void pushup(int u, ll k){
        t[u].v += k * (t[u].r - t[u].l + 1);
        t[u].lz += k;
    }
    void pushdown(int u){
        if(!t[u].lz) return;
        t[u*2].v += t[u].lz * (t[u*2].r - t[u*2].l + 1);
        t[u*2].lz += t[u].lz;
        t[u*2+1].v += t[u].lz * (t[u*2+1].r - t[u*2+1].l + 1);
        t[u*2+1].lz += t[u].lz;
        t[u].lz = 0;
    }
    // 单点加
    void update(int idx, ll k, int u = 1){
        if(t[u].l == t[u].r){
            t[u].v += k;
            return;
        }
        pushdown(u);
        int mid = (t[u].l + t[u].r) / 2;
        if(idx <= mid) update(idx, k, u*2);
        else update(idx, k, u*2+1);
        t[u].v = t[u*2].v + t[u*2+1].v;
    }
    // 区间加
    void update(int ul, int ur, ll k, int u = 1){
        auto [v, lz, l, r] = t[u];
        if(ul > r || ur < l) return;
        if(ul <= l && ur >= r){
            pushup(u, k);
            return;
        }
        pushdown(u);
        update(ul, ur, k, u*2);
        update(ul, ur, k, u*2+1);
        t[u].v = t[u*2].v + t[u*2+1].v;
    }
    // 区间和查询
    ll query(int ql, int qr, int u = 1){
        if(ql > qr) return 0;
        auto [v, lz, l, r] = t[u];
        if(ql > r || qr < l) return 0;
        if(ql <= l && qr >= r) return v;
        pushdown(u);
        return query(ql, qr, u*2) + query(ql, qr, u*2+1);
    }
    // 单点查询
    ll query(int idx){
        return query(idx, idx);
    }
};
