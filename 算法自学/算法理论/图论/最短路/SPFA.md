# SPFA
[toc]

## 介绍
队列优化版的 Bellman-Ford 算法。

## 原理
### 优化
Bellman-Ford 算法每次松弛所有边，有很多无效操作
- $d[u]$ 不变 $\Rightarrow$ 所有出边松弛后均不变
- $u \rightarrow v$ 可松弛 $\Rightarrow$ $d[u]$ 变小

所以只需要维护一个队列，记录更新过的点，每次从队列中取点更新所有出边

### 队列去重
一个节点可能被多个入边更新，此时要防止被重复加入队列
- 维护 `in_q` 数组，记录节点是否在队列中

### 负环检测
节点被松弛次数超过 $V-1$ 次，说明存在负环
- 维护 `cnt` 数组，记录节点被松弛次数

## 步骤
1. 初始化 `dist[u] = ∞`，`dist[s] = 0`
2. 初始化队列 `q`，将起点 `s` 加入队列
3. 循环直到队列为空
    - 从队列中取出一个点 $u$
    - 遍历所有出边 $u \rightarrow v$
    - 对比 $d[u] + w(u, v)$ 和 $d[v]$
    - 检查负环和队列去重，更新

## 代码
[Node定义](./最短路径.md#node定义)
```cpp
// 邻接表
vector<vector<Node>> adj;
// 结果
vector<int> dist, prev;
// SPFA
bool SPFA(int n, int s){
    // 初始化
    dist.assign(n + 1, INT_MAX);
    prev.assign(n + 1, -1);
    vector<bool> in_q(n + 1, false);
    vector<int> cnt(n + 1, 0);
    queue<int> q;
    // 初始化起点
    q.push(s), in_q[s] = true, dist[s] = 0;
    // 迭代
    while(!q.empty()){
        // 取出队首
        int u = q.front();
        q.pop();
        in_q[u] = false;
        // 遍历所有出边
        for(auto [v, w]: adj[u])
            // 可以更新
            if(dist[u] + w < dist[v]){
                dist[v] = dist[u] + w, prev[v] = u;
                // 队列去重
                if(!in_q[v]){
                    in_q[v] = true;
                    q.push(v);
                    // 入队次数超过 n-1 存在负环
                    if(++cnt[v] >= n) return true;
                }
            }
    }
    // 没有负环
    return false;
}
```

## 复杂度
- **时间复杂度**：平均 $O(E)$，最坏 $O(VE)$
- **空间复杂度**：$O(V)$

## 优化
### 原理
SPFA 遵循 BFS 顺序，如果先沿较长路径更新，再沿较短路径更新，之前的更新是没意义的
- 让较短路径优先更新

### SLF：Small Label First
入队时把距离小的点放队首，距离大的放队尾。
```cpp
// 入队时：
// 距离比队首还小，插到最前面优先处理
if (!q.empty() && dist[v] < dist[q.front()]) q.push_front(v);  
// 否则正常入队
else q.push_back(v);
```

### LLL：Large Label Last
队列中所有待处理的点反映松弛进度。如果某点的距离大于平均距离，它很可能是被一条长路径先于短路径发现的，所以把它扔到队尾。
```cpp
// 维护 dist 的和，入队出队时顺手更新
int sum;

// 入队时：
q.push_back(v);
sum += dist[v];

// 出队时：
sum -= dist[q.front()];
q.pop_front();
// 队首点的距离大于平均值，不应该优先处理
while (!q.empty() && dist[q.front()] > sum / q.size()){
    // 挪到队尾
    q.push_back(q.front());   
    q.pop_front();
}
```

