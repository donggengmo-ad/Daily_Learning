#include <vector>
#include <utility>
using namespace std;
using pii = pair<int,int>;

// 倍增法
struct LCA{
    static const int LOG = 20; // 按需调整
    struct Node{
        int dep = 0;
        // fa[i] 表示 2^i 级祖先
        vector<int> ch, fa;
        Node(): fa(LOG + 1, 0){} 
    };
    vector<Node> t;
    LCA(int n): t(n + 1){}
    // 构建倍增表，调用 dfs(1, 0);
    void dfs(int u, int fa){
        t[u].dep = t[fa].dep + 1;
        t[u].fa[0] = fa;
        for(int i = 1;i <= LOG;i++)
            t[u].fa[i] = t[t[u].fa[i-1]].fa[i-1];
        for(int v: t[u].ch) 
           if(v != fa) 
               dfs(v, u); 
    }
    // 查询 u 和 v 的 LCA
    int lca(int u, int v){
        if(t[u].dep < t[v].dep) swap(u, v);
        for(int i = LOG;i >= 0;i--)
            if(t[t[u].fa[i]].dep >= t[v].dep)
                u = t[u].fa[i];
        if(u == v) return u;
        for(int i = LOG;i >= 0;i--)
            if(t[u].fa[i] != t[v].fa[i])
                u = t[u].fa[i], v = t[v].fa[i]; 
        return t[u].fa[0];
    }
};

// 并查集，为tarjan准备，支持根节点映射
struct DSU{
    vector<int> p, h, rt;
    DSU(int n): p(n + 1), h(n + 1, 0), rt(n + 1, 0){
        for(int i = 1;i <= n;i++) p[i] = rt[i] = i;
    }
    int find(int u, bool proj){
        if(p[u] != u) p[u] = find(p[u], false);
        return proj ? rt[p[u]] : p[u]; 
    }
    void unite(int u, int v, int root){
        int ru = find(u, false), rv = find(v, false);
        if(ru != rv){
            if(h[ru] < h[rv]) swap(ru, rv);
            p[rv] = ru, h[ru] += h[ru] == h[rv];
            rt[ru] = root;
        }
    }
};

// tarjan算法
struct Tarjan{
    struct Node{
        vector<pii> q; // id, v
        bool vis = false;
        vector<int> ch;
    };
    vector<Node> t;
    vector<int> ans;
    DSU dsu;
    Tarjan(int n, int q): t(n + 1), dsu(n), ans(q + 1, 0){}
    // 离线处理所有查询
    void dfs(int u, int fa){
        for(int v: t[u].ch) 
            if(v != fa) 
                dfs(v, u), dsu.unite(u, v, u);
        t[u].vis = true;
        for(auto &[q, v]: t[u].q) 
            if(t[v].vis) 
                ans[q] = dsu.find(v, true);
    }
};
