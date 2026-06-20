# 最近公共祖先（LCA）
[toc]

## 定义
- **祖先**：可由某节点沿父指针追溯到的节点
- **公共祖先**：同为两个节点祖先的节点
- **最近公共祖先**：公共祖先中深度最大的节点

## 解法
### 朴素算法
#### 原理
> 设有节点 $A$、$B$，$dep_A \gt dep_B$
- $A$ 向上跳，直到 $dep_A = dep_B$
- 同时上跳 $A$ 和 $B$，直到 $A=B$

#### 代码
- 先 dfs 记录深度和父指针
- 再按照上述原理上跳
```cpp
// 邻接表
vector<vector<int>> adj;
// 深度和父指针
vector<int> dep, fa;
// u：当前，p：父节点，d：深度
void dfs(int u, int p, int d){
    dep[u] = d, fa[u] = p;
    for(int v: adj[u]) 
       if(v != p) dfs(v, u, d + 1);
}
int lca(int a, int b){
    // dfs 记录
    dfs(0, -1, 0);
    // 设置 a 深度大于 b
    if(dep[a] < dep[b]) swap(a, b);
    // 上跳 a 到与 b 同层
    while(dep[a] > dep[b]) a = fa[a];
    // 上跳 a 和 b 直到相遇
    while(a != b) a = fa[a], b = fa[b];
    return a;
}
```

#### 复杂度
- 时间复杂度：预处理 $O(n)$，查询 $O(n)$
- 空间复杂度：$O(n)$

### 树上倍增
#### 原理
- 记录每个节点的 $2^i$ 级祖先
- 每次将上跳步数拆分为 $2$ 的幂次和

#### 代码
- 先 dfs 记录每个节点的 $2^i$ 级祖先
> $2^i$ 级祖先 $=$ $2^{i-1}$ 级祖先的 $2^{i-1}$ 级祖先
- 再按照上述原理上跳
> 结束时要求 $A$ 和 $B$ 的父节点相遇且 $A$ 和 $B$ 不同，防止不是最近祖先
```cpp
// anc[u][i] 表示 u 的 2^p 级祖先
const int LOG = 20;
vector<vector<int>> anc;
// u：当前，p：父节点，d：深度
void dfs(int u, int p, int d){
    // 记录父亲
    anc[u][0] = p;
    // 记录 2^i 级祖先
    for(int i = 1;i <= LOG;i++) 
        anc[u][i] = anc[anc[u][i-1]][i-1];
    dep[u] = d;
    for(int v: adj[u]) 
        if(v != p) dfs(v, u, d + 1);
}
int lca(int a, int b){
    // dfs 记录
    dfs(1, 0, 0);
    // 设置 a 深度大于 b
    if(dep[a] < dep[b]) swap(a, b);
    // 上跳 a 到与 b 同层
    for(int i = LOG;i >= 0;i--)
        if(dep[anc[a][i]] >= dep[b]) 
            a = anc[a][i];
    // 特判 a = b
    if(a == b) return a;
    // 上跳 a 和 b 直到父节点相遇
    for(int i = LOG;i >= 0;i--)
        if(anc[a][i] != anc[b][i])
            a = anc[a][i], b = anc[b][i];
    // 返回父节点
    return anc[a][0];
}
```

#### 复杂度
- 时间复杂度：预处理 $O(n\log n)$，查询 $O(\log n)$
- 空间复杂度：$O(n\log n)$

### Tarjan 算法
#### 原理
- 存储所有查询到对应节点下
- 在 dfs 中处理已知的查询
- 处理过的节点用并查集合并
> 相当于把这些节点压缩到一个祖先节点中

#### 代码
```cpp
struct Node{
    vector<pair<int, int>> q; // id, v
    vector<int> ch;
};
vector<Node> adj;
vector<bool> vis;
vector<int> ans;
// 并查集需要做额外映射，使 find 方法返回真实根节点
struct DSU{
    void merge(int x, int y, int root){}
    int find(int x, bool proj=true){}
};
DSU dsu;
void dfs(int u, int fa){
    // 先递归处理子节点并合并
    for(int v: adj[u].ch) 
        if(v != fa) dfs(v, u), dsu.unite(u, v, u);
    // 处理已知查询
    for(auto [q, v]: adj[u].q) 
        if(vis[v]) ans[q] = dsu.find(v);
    // 标记已知（这种顺序下 lca(u, u) != u)
    vis[u] = true;
}
```

#### 复杂度
- 时间复杂度：$O(n + q\cdot \alpha(n))$
- 空间复杂度：$O(n + q)$

## 应用
### 路径差分
$$path_{u \to v} = path_{u \to root} + path_{v \to root} - 2 \cdot path_{lca \to root}$$
### 点权差分
区间更改时：
```cpp
diff[u]++, dif[v]++;
diff[lca]--, diff[fa[lca]]--;
```
查询时自底向上 dfs 累加

### 边权差分
区间更改时：
```cpp
diff[u]++, diff[v]++;
diff[lca] -= 2;
```
查询时自底向上 dfs 累加


