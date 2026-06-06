# Kruskal算法（加边法）
[toc]

## 介绍
以边为基础的最小生成树算法，适合稀疏图。

## 步骤
1. 按权值从小到大排序所有边
2. 选择权值最小的边，若其终点不在树中则加入，在树中则跳过
3. 重复步骤2，直到树的边数等于$n-1$

## 实现
- 按权值排序，使用边集数组
- 连通性问题，使用并查集

## 代码
[Edge](最小生成树.md#edge定义)定义
```cpp
// 顶点数n，边数m
int n, m;
// 并查集（省略）
DSU dsu(n);
// 边集数组（省略构造过程）
vector<Edge> E(m);
// Kruskal算法
int kruskal(){
    // 边权和、边数
    int res = 0, cnt = 0;
    // 按边权排序
    sort(E.begin(), E.end());
    // 初始化并查集
    dsu.init(n);
    // 从小到大枚举边
    for(int i = 0;i < m;i++){
        auto [u, v, w] = E[i];
        // 找出边两端的根节点
        int pu = dsu.find(u), pv = dsu.find(v);
        // 若两端不在同一树中，合并树
        if(pu!= pv){
            dsu.unite(pu, pv);
            // 更新边权和、边数
            res += w, cnt++;
            // 在这里可以输出边信息
        }
        // 若已有n-1条边，则停止枚举
        if(cnt == n - 1) break;
    }
    // 加入边不到n-1，不连通
    if(cnt < n - 1) return -1;
    return res;
}
```

## 复杂度
顶点数$n$，边数$m$
- 时间复杂度$O(m\log m)$
- 空间复杂度$O(n+m)$
