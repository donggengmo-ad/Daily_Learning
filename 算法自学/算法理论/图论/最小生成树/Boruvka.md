# Boruvka算法
[toc]

## 简介
以连通分量为基础的最小生成树算法，适合稠密图、完全图。

## 步骤
1. 初始每个点自成连通分量
2. 每个分量找出最小出边，合并分量
3. 重复步骤2，直到只剩下一个连通分量

## 实现
- 连通性问题，使用并查集
- 频繁找边，使用边集数组

## 代码
[Edge](最小生成树.md#edge定义)定义见下
```cpp
// 顶点数n，边数m
int n, m;
// 并查集（省略）
DSU dsu(n);
// 边集数组（省略构造过程）
vector<Edge> E(m);
// Boruvka算法
int boruvka(){
    // 初始化
    dsu.init(n);
    int res = 0, cnt = n;
    // 循环合并
    while(cnt > 1){
        // 找出每个连通分量的最小边下标
        vector<int> minI(n, -1);
        // 遍历所有边
        for(int i = 0;i < m;i++){
            // 找到边两端所属分量
            auto [u, v, w] = E[i];
            int pu = dsu.find(u), pv = dsu.find(v);
            // 在同一个分量，跳过
            if(pu == pv) continue;
            // 找出分量中权值最小的边
            if(minI[pu] == -1 || w < E[minI[pu]].w) minI[pu] = i;
            if(minI[pv] == -1 || w < E[minI[pv]].w) minI[pv] = i;
        }
        // 合并分量
        bool merged = false;
        // 遍历所有点
        for(int i = 0;i < n;i++){
            // 所属分量最小边不在此点，跳过
            if(minI[i] == -1) continue;
            // 所属分量最小边端点
            auto [u, v, w] = E[minI[i]];
            // 若不在同一连通分量，则合并
            int pu = dsu.find(u), pv = dsu.find(v);
            if(pu!= pv){
                dsu.unite(pu, pv);
                res += w, cnt--;
                merged = true;
            }
        }
        // 没有可合并分量，说明不连通
        if(!merged) break; 
    }
    // 剩余连通分量 > 1，不连通
    if(cnt > 1) return -1;
    return res;
}
```

## 复杂度
顶点数$n$，边数$m$
- 时间复杂度$O((n + m)\log n)$
- 空间复杂度$O(n+m)$
