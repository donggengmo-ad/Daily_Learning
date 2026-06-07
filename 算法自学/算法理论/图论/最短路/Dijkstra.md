# Dijkstra算法
[toc]

## 介绍
单源最短路径算法，适用于稠密、无负边图。

## 步骤
1. 选择源点 $s$，$s$ 的距离设为 $0$，其他点设为$\infty$
2. 选择距 $s$ 最近的点 $u$，更新最小距离
>$d_u = \min(d_s + w_{s\rightarrow u}, \space d_u)$。
3. 将 $s$ 设为 $u$，重复步骤

## 复杂度
- 时间复杂度：$O(n^2)$
- 空间复杂度：$O(n^2)$

## 代码
```cpp
// 图的邻接矩阵（不连接的边用 INT_MAX 表示）
vector<vector<int>> G;
// 结果（最短路长度，和路径上每个点的前驱点）
vector<int> dist;
vector<int> prev;
// Dijkstra算法
void dijkstra(int s){
    // 数据准备
    int n = G.size();
    vector<bool> vis(n, false);
    // 初始化距离
    dist.assign(n, INT_MAX);
    for(int i = 0; i < n; i++) 
        dist[i] = G[s][i];
    dist[s] = 0; 
    // 初始化前驱点
    prev.assign(n, -1);
    for(int i = 0; i < n; i++)
        if(G[s][i] < INT_MAX) 
            prev[i] = s;
    // 再找 n-1 次
    for(int i = 0; i < n-1; i++){
        // 找离s最近的点u
        int u = -1, min_dist = INT_MAX;
        for(int j = 0;j < n;j++)
            if(!vis[j] && dist[j] < min_dist) 
                min_dist = dist[j], u = j;
        // 没找到，说明其余点不可达
        if(u == -1) break;
        // u的最短距离可以确定（其他点都远，不用绕行）
        vis[u] = true;
        // 最后一轮就不用更新了
        if(i == n - 2) break;
        // 更新其他没确定，且有边连接u的点
        for(int j = 0; j < n; j++) 
            if(!vis[j] && G[u][j] < INT_MAX) 
                // 如果从u绕行到j更近，更新距离和前驱点
                if(dist[u] + G[u][j] < dist[j])
                    dist[j] = dist[u] + G[u][j], prev[j] = u;
    }
}
```

## 优化Dijkstra
用堆和邻接表优化的Dijkstra算法。

### 优化
|算法缺陷|原因|优化方式|
|----|----|----|
|$O(n^2)$时间|找最小距离点$O(n)$|堆优化到$O(\log n)$|
|$O(n^2)$空间|邻接矩阵|稀疏图用邻接表$O(n+m)$|

### 代码
[Node](最短路径.md#node定义)定义见下
```cpp
// 图的邻接表
vector<vector<Node>> adj;
// 结果（最短路长度，和路径上每个点的前驱点）
vector<int> dist;
vector<int> prev;
// Dijkstra算法
void dijkstra(int s){
    int n = adj.size();
    // 其实 vis 可以省略
    vector<bool> vis(n, false);
    dist.assign(n, INT_MAX);
    prev.assign(n, -1);
    // 小顶堆存 距离-编号 对
    priority_queue<
       pair<int, int>, 
       vector<pair<int, int>>, 
       greater<>
    > pq;
    // 初始化距离
    dist[s] = 0;
    pq.emplace(0, s);
    // 队列不空，找最小距离点
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        // 确定过的点，跳过
        if (vis[u]) continue;
        vis[u] = true;
        // 如果 d > dist[u]，说明这条记录已过时
        if (d > dist[u]) continue;
        // 更新其他没确定，且有边连接u的点
        for (auto [v, w] : adj[u]) {
            if (!vis[v] && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                // 新距离压入堆
                pq.emplace(dist[v], v); 
            }
        }
    }
}
```
