# Floyd-Warshall算法
[toc]

## 简介
多源最短路径算法，适用于稀疏图。

## 原理
### 路径搜索
尝试每个点作为中转点，更新路径长度
> 以 $k$ 点为中转点比较：
> - $i \to j$ 路径
> - $i \to k \to j$ 路径

### 负环检测
对角线上有负数的点，说明存在负环
> 绕一圈回到自己，距离反而小了

## 步骤
1. 初始化 `dist` 和 `prev` 矩阵
2. 进行 $V$ 次中转遍历
    - 以 $k$ 点为中转，枚举所有点 $i$
    - 枚举点 $i$ 的所有邻接点 $j$
    - 比较 $i \to j$ 和 $i \to k \to j$
    - 更新距离和终点前驱
3. 最后检查负环

## 代码
```cpp
// 图的邻接矩阵
vector<vector<int>> G;
// 结果（最短路长度和终点前驱）
vector<vector<int>> dist;
vector<vector<int>> prev;
// Floyd算法
void floyd(){
    int n = G.size();
    // 初始化 dist 和 prev
    dist.assign(n, vector<int>(n, INT_MAX));
    prev.assign(n, vector<int>(n, -1));
    for(int i = 0;i < n;i++>)
        for(int j = 0;j < n;j++){
            dist[i][j] = G[i][j];
            // 按有向边初始化 prev
            if(i != j && G[i][j] < INT_MAX) prev[i][j] = i;
            // 对角线和无边的点，前驱设为-1
            else prev[i][j] = -1;
        }
    // 遍历中转点
    for(int k = 0;k < n;k++)
        // 枚举所有位置
        for(int i = 0;i < n;i++)
            for(int j = 0;j < n;j++){
                // 路径不通，跳过
                if(dist[i][k] == INT_MAX || dist[k][j] == INT_MAX) continue;
                // 对比 i->j 和 i->k->j
                if(dist[i][j] > dist[i][k] + dist[k][j])
                    dist[i][j] = dist[i][k] + dist[k][j],
                    prev[i][j] = prev[k][j];
            }
}
```

## 复杂度
- 时间复杂度：$O(n^3)$
- 空间复杂度：$O(n^2)$


