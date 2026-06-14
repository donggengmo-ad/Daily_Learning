#include <vector>
using namespace std;
using ll = long long;
// 维护区间和为例
struct BlockArray{
    struct Block{
        ll sum = 0, add = 0;
        int l = 0, r = 0;
    };
    vector<Block> b;
    vector<ll> a;
    vector<int> in;
    int num = 0, len = 0;
    // 理论大小 n，1-based 数组 a
    BlockArray(int n, vector<ll> &a): a(a), in(n + 1, 0){
        len = sqrt(n * 1.0);
        num = n / len + (n % len != 0);
        b.resize(num + 1);
        for(int i = 1; i <= num; i++)
            b[i].l = (i - 1) * len + 1, 
            b[i].r = i * len;
        b[num].r = n;
        for(int i = 1;i <= num;i++)
           for(int j = b[i].l; j <= b[i].r; j++)
                b[i].sum += a[j], 
                in[j] = i;
    }
    // 单点加
    void add(int x, ll k){
        a[x] += k, 
        b[in[x]].sum += k;
    }
    // 区间加
    void add(int l, int r, ll k){
        int p = in[l], q = in[r];
        if(p == q){
            for(int i = l; i <= r; i++) add(i, k);
            return;
        }
        for(int i = p + 1; i < q; i++) b[i].add += k;
        for(int i = l; i <= b[p].r; i++) add(i, k);
        for(int i = b[q].l; i <= r; i++) add(i, k);
    }
    // 单点查询
    ll query(int x){
        return a[x] + b[in[x]].add;
    }
    // 区间查询
    ll query(int l, int r){
        int p = in[l], q = in[r];
        ll res = 0;
        if(p == q){
            for(int i = l; i <= r; i++) res += query(i);
            return res;
        }
        for(int i = p + 1; i < q; i++) res += b[i].sum + b[i].add * len;
        for(int i = l; i <= b[p].r; i++) res += query(i);
        for(int i = b[q].l; i <= r; i++) res += query(i); 
        return res; 
    }
};


