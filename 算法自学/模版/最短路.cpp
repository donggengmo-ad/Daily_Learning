#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
using ll = long long; 
using pii = pair<ll, int>; // (dist, node)
const ll INF = 1e18;

struct SingleSource{
    struct Node{
        int v = 0;
        ll w = 0;
    };
    vector<vector<Node>> adj;
    vector<ll> dis;
    vector<int> path;
    int s, n;
    SingleSource(int n, int s): n(n), s(s), adj(n + 1){}
    void addEdge(int u, int v, ll w){
        adj[u].emplace_back(v, w); 
    }
    // 初始化
    void init(int s){
        dis.assign(n + 1, INF);
        path.assign(n + 1, -1);
        dis[s] = 0;
    }
    // 松弛操作
    bool relax(int u, int v, ll w){
        if(dis[u] < INF && dis[v] > dis[u] + w){
           dis[v] = dis[u] + w;
           path[v] = u;
           return true; 
        }
        return false;
    }
    // Dijkstra 要求所有边权非负
    void dijkstra(){
        init(s);
        priority_queue<pii, vector<pii>, greater<>> q;
        q.emplace(0, s);
        while(!q.empty()){
            auto [d, u] = q.top();
            q.pop();
            if(d > dis[u]) continue;
            for(auto [v, w]: adj[u]) 
               if(relax(u, v, w)) q.emplace(dis[v], v);
        }
    }
    // Bellman-Ford 返回有没有负环
    bool bellman_ford(){
        init(s);
        for(int k = 0;k < n - 1;k++){
            bool update = false;
            for(int u = 1;u <= n;u++)
               for(auto [v, w]: adj[u]) 
                   update |= relax(u, v, w);
            if(!update) break;
        }
        for(int u = 1;u <= n;u++)
            for(auto [v, w]: adj[u]) 
                if(relax(u, v, w)) return true;
        return false;
    }
    // SPFA
    bool spfa(){
        init(s);
        vector<bool> in(n + 1, false);
        vector<int> cnt(n + 1, 0);
        queue<int> q;
        q.emplace(s);
        in[s] = true;
        cnt[s] = 1;
        while(!q.empty()){
            int u = q.front();
            q.pop();
            in[u] = false;
            for(auto [v, w]: adj[u])
                if(relax(u, v, w) && !in[v]){
                    in[v] = true;
                    q.emplace(v);
                    if(++cnt[v] >= n) return true;
                }
        }
        return false;
    }
    // 沿前驱获取路径
    void getPath(int t, vector<int> &res){
        while(t != -1) res.emplace_back(t), t = path[t];
        reverse(res.begin(), res.end());
    }
};

struct MultipleSource{
    vector<vector<ll>> g;
    vector<vector<ll>> dis;
    vector<vector<int>> path;
    int n;
    MultipleSource(int n): n(n), g(n + 1, vector<ll>(n + 1, INF)){}
    void addEdge(int u, int v, ll w){
        g[u][v] = min(g[u][v], w);
    }
    // 初始化
    void init(){
        dis = vector<vector<ll>>(g);
        path.assign(n + 1, vector<int>(n + 1, -1));
        for(int i = 1;i <= n;i++) dis[i][i] = 0;
        for(int i = 1;i <= n;i++)
            for(int j = 1;j <= n;j++)
                if(i != j && g[i][j] < INF) path[i][j] = i;
    }
    // 松弛操作
    bool relax(int u, int v, int k){
        if(dis[u][k] >= INF || dis[k][v] >= INF) return false;
        if(dis[u][v] > dis[u][k] + dis[k][v]){
            dis[u][v] = dis[u][k] + dis[k][v];
            path[u][v] = path[k][v];
            return true;
        }
        return false;
    }
    bool floyd(){
        init();
        for(int k = 1;k <= n;k++)
            for(int i = 1;i <= n;i++)
                for(int j = 1;j <= n;j++)
                   relax(i, j, k);
        for(int i = 1;i <= n;i++)
            if(dis[i][i] < 0) return true;
        return false;
    }
    // 沿前驱获取路径
    void getPath(int u, int v, vector<int> &res){
        if(path[u][v] == -1) return;
        while(v != u) res.emplace_back(v), v = path[u][v];
        res.emplace_back(u);
        reverse(res.begin(), res.end());
    }
};
