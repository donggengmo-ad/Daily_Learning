#include <iostream>
#include <vector>
using namespace std;
using ll = long long;

struct DSU{
    int n;
    vector<int> p, h, sz;
    vector<ll> w;
    // 输入真实大小，自动转1-base
    DSU(int n): 
        n(n), p(n + 1), h(n + 1, 0), sz(n + 1, 1), w(n + 1, 0){
        for(int i = 1; i <= n; i++) p[i] = i;
    }
    int find(int x){
        if(p[x] != x){
            int fa = p[x];
            int rt = find(fa);
            w[x] += w[fa];
            p[x] = rt;
        }
        return p[x];
    }
    // w0: y->x 权值
    void unite(int x, int y, ll w0 = 0){
        int rx = find(x), ry = find(y);
        if(rx != ry){
            if(h[rx] < h[ry]){
                p[rx] = ry;
                w[rx] = w[y] - w[x] - w0;
                sz[ry] += sz[rx];
            }
            else{
                p[ry] = rx;
                w[ry] = w[x] - w[y] + w0;
                h[rx] += h[rx] == h[ry];
                sz[rx] += sz[ry];
            }
        }
    }
    bool same(int x, int y){
        return find(x) == find(y);
    }
    // 返回 y->x 权值，不连通返回 -1
    ll query(int x, int y){
        int rx = find(x), ry = find(y);
        if(rx != ry) return -1;
        return w[y] - w[x];
    }
    void reset(){
        for(int i = 1; i <= n; i++) 
            p[i] = i, w[i] = 0, h[i] = 0, sz[i] = 1;
    }
};

int main(){
    return 0;
}