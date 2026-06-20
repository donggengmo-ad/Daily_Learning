#include <vector>
using namespace std;
using ll = long long;

// 线段树（略）
struct SegTree{
    SegTree(int n){}
    void add(int l, int r, int val){}
    ll query(int l, int r){}    
};

// 重链剖分，维护点权路径和、子树和为例
struct TreeCut{
    struct Node{
        int fa = 0, sz = 0, dep = 0;
        int hv = 0, top = 0;
        vector<int> ch;
    };
    vector<Node> t;
    // dfs序 <-> 节点编号
    vector<int> dfn, rnk;
    SegTree seg;
    TreeCut(int n): 
        t(n + 1), dfn(n + 1, 0), rnk(n + 1, 0), seg(n){}    
    // 构建方法（两次dfs）
    // 调用 detect(root, 0);
    void detect(int u, int fa){
        t[u].fa = fa;
        t[u].dep = t[fa].dep + 1;
        t[u].sz = 1;
        int maxSz = 0;
        for(int v: t[u].ch){
            if(v == fa) continue;
            detect(v, u);
            t[u].sz += t[v].sz;
            if(t[v].sz > maxSz) maxSz = t[v].sz, t[u].hv = v;
        }
    }
    // 调用 cut(root, root);
    int cnt = 0;
    void cut(int u, int top){
        t[u].top = top;
        dfn[u] = ++cnt;
        rnk[cnt] = u;
        if(t[u].hv) cut(t[u].hv, top);
        for(int v: t[u].ch)
            if(v != t[u].hv && v != t[u].fa) cut(v, v);
    }
    // 查询 LCA
    int lca(int a, int b){
        while(t[a].top != t[b].top){
            if(t[a].dep < t[b].dep) swap(a, b);
            a = t[t[a].top].fa;
        }
        return t[a].dep < t[b].dep ? a : b;
    }
    // 路径操作
    void add(int u, int v, ll k){
        while(t[u].top != t[v].top){
            if(t[u].dep < t[v].dep) swap(u, v);
            seg.add(dfn[t[u].top], dfn[u], k);
            u = t[t[u].top].fa;
        }
        if(dfn[u] > dfn[v]) swap(u, v);
        seg.add(dfn[u], dfn[v], k);
    }
    ll query(int u, int v){
        ll res = 0;
        while(t[u].top != t[v].top){
            if(t[u].dep < t[v].dep) swap(u, v);
            res += seg.query(dfn[t[u].top], dfn[u]);
            u = t[t[u].top].fa;
        }
        if(dfn[u] > dfn[v]) swap(u, v);
        return res + seg.query(dfn[u], dfn[v]);        
    }    
    // 子树操作
    void add(int u, ll k){
        seg.add(dfn[u], dfn[u] + t[u].sz - 1, k);        
    }
    ll query(int u){
        return seg.query(dfn[u], dfn[u] + t[u].sz - 1);        
    }
};

