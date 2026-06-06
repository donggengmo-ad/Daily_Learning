# Prim算法（加点法）
[toc]

## 介绍
以点为基础的最小生成树算法，适合稠密图。

## 步骤
1. 选择根节点加入树
2. 选择已连通点中权值最小的边，边终点加入树
3. 重复步骤2，直到树的边数等于$n-1$

## 实现
- 频繁访问任意两点的边，使用邻接矩阵

## 代码
```cpp
// 邻接矩阵（省略构造过程）
int G[MAXN][MAXN];
// 最小生成树的边集，记录每个节点的父节点下标
vector<int> fa(n, -1);
// Prim算法，u为起始点
int prim(int u, int n){
    // 记录每个点距树最小边权
    vector<int> dis(n, INF);
    vector<bool> vis(n, false);
    // 初始化
    fa.assign(n, -1);
    dis[u] = 0;
    int res = 0;
    // 加入n-1条边
    for(int i = 0;i < n - 1;i++){
        // 找到离树最小权值的点
        int v = -1;
        // 遍历所有点
        for(int j = 0;j < n;j++){
            // 已在树中跳过
            if(vis[j]) continue;
            // 更新最小权值
            if(v == -1 || dis[j] < dis[v]) v = j;
        }
        // 没有可加入的点，说明不连通
        if(v == -1) return -1;
        // 加入点
        res += dis[v];
        vis[v] = true;
        // 更新dis
        for(int j = 0;j < n;j++){
            // 更新最小权值
            if(!vis[j] && G[v][j] < dis[j]){
                dis[j] = G[v][j];
                // 更新连接边
                fa[j] = v;
            }
        }
    }
    return res;
}
```

## 复杂度
顶点数$n$
- 时间复杂度$O(n^2)$
- 空间复杂度$O(n^2)$

## 优化Prim
用堆和邻接表优化的Prim算法。

### 优化
|Prim算法缺陷|原因|优化方式|
|----|----|----|
|$O(n^2)$时间|找最小权值点$O(n)$|堆优化到$O(\log n)$|
|$O(n^2)$空间|邻接矩阵|稀疏图用邻接表$O(n+m)$|

### 代码
[Node](最小生成树.md#node定义)定义见下
```cpp
// 邻接表（省略构造过程）
vector<vector<Node>> adj;
// Prim算法，u为起始点
int prim(int u, int n){
    // 堆存储候选顶点
    priority_queue<
        Node,
        vector<Node>,
        greater<Node> // 小顶堆
    > pq;
    // 初始化数据
    vector<int> dis(n, INF);
    vector<bool> vis(n, false);
    int res = 0, cnt = 0;
    // 起始点加入候选（权值0）
    pq.emplace(u, 0);
    dis[u] = 0;
    // 循环加入最小权值点
    while(!pq.empty() && cnt < n){
        // 取出最小权值点
        auto [v, w] = pq.top();
        pq.pop();
        // 若已在树中，跳过
        if(vis[v]) continue;
        // 加入树
        res += w, cnt++;
        vis[v] = true;
        // 加入邻接点
        for(auto [to, nw] : adj[v]){
            // 更新每点最小边权（不在树中的）
            if(!vis[to] && nw < dis[to]){
                pq.emplace(to, nw);
                dis[to] = nw;
            }
        }
    }
    // 加入点不到n，不连通
    if(cnt < n) return -1;
    return res;
}
```

### 复杂度
顶点数$n$，边数$m$
- 时间复杂度$O((n+m)\log n)$
- 空间复杂度$O(n+m)$
