# Bellman-Ford
[toc]

## 介绍
单源最短路算法，适用于有负权边的图，也可检测负环。

## 原理
### 路径搜索
进行 $V-1$ 次迭代，每轮对所有边松弛一次，$V$ 为图中顶点数
> 第 $i$ 次迭代，检查步数 $\leq i$ 的路径

### 负环检测
如果 $V-1$ 次迭代后，还能继续更新，说明存在负环
> 证明：
> - 最短路径一定是简单路径（无环）
>   - **正环**：去掉环路径长度更短
>   - **零环**：没有影响
>   - **负环**：不应该存在
> - 简单路径长度 $\leq V-1$
> 
> 有长度 $\gt V-1$ 的路径，说明存在负环

## 步骤
1. 初始化 `dist` 和 `prev` 数组
2. 进行 $V-1$ 次迭代
    - 枚举所有边
    - 比较新旧路径，更新 `dist` 和 `prev`
3. 最后检查负环

## 代码
[Edge定义](最短路径.md#Edge定义)
```cpp
// 边集数组
vector<Edge> edge;
// 结果
vector<int> dist, prev;
// Bellman-Ford
// n: 顶点数
// s: 起点
bool bellmanFord(int n, int s) {
    // 初始化
    dist.assign(adj.size(), INT_MAX);
    prev.assign(adj.size(), -1);
    dist[s] = 0;
    // 迭代 V-1 次
    for(int k = 0;k < n - 1;k++){
        // 检查是否更新
        bool changed = false;
        // 枚举所有边
        for(auto [u, v, w]: edge)
            // 松弛
            if(dist[u] != INT_MAX && dist[u] + w < dist[v])
                dist[v] = dist[u] + w, // 更新距离
                prev[v] = u, // 记录前驱
                changed = true;
        // 没有更新说明收敛，结束
        if(!changed) break;
    }
    // 检查是否存在负环
    for(auto [u, v, w]: edge)
        if(dist[u] != INT_MAX && dist[u] + w < dist[v])
            return true; // 存在负环
    return false; // 没有负环
}
```

## 复杂度
- **时间**：$O(V \cdot E)$
- **空间**：$O(V)$
