# ST表
[toc]
## 介绍
基于倍增，用于解决可重复贡献问题的数据结构
- 区间最值
- 区间gcd
  
## 实现
$F_{i,j}$ 表示以 $i$ 为起点，长度为 $2^j$ 区间的最大值
### 构建
- **初始化**： $F_{i,0} = a_i$
- **转移方程**： 
$F_{i,j} = max(F_{i,j-1}, F_{i+2^{j-1},j-1})$ 

|$a_i$|1|2|3|4|5|
|---|---|---|---|---|---|
|$2^0$|1|2|3|4|5|
|$2^1$|2|3|4|5
|$2^2$|4|5|

### 查询
- **区间**：$[l, r],\space n = l - r + 1 $
- 用两个长度为 $\log n$ 的区间覆盖 $[l, r]$
- $ans = max(F_{l,\log n}, F_{r-2^{\log n} + 1, \log n})$

||$l$|-|-|-|$r$|
|-|-|-|-|-|-|
|1|$[l, l+2^{\log n} - 1$]||||-
|2|-|$[r-2^{\log n} + 1, r]$


## 代码
```cpp
// 目标数据
int a[MAXN];
// 预处理log2
int log2[MAXN] = {0};
void preprocess() {
    for(int i = 2;i < MAXN;i++) 
        log2[i] = log2[i/2] + 1;
}
// ST表
int st[MAXN][LOG];
void build(int n){
    // 初始化
    for(int i = 0;i < n;i++) st[i][0] = a[i];
    // 状态转移
    for(int k = 1;K < LOG;k++)
        // 区间尾不能越界
        for(int i = 0;i + (1<<k) - 1 < n;i++)
            // 从两个次级区间中选最大值
            st[i][k] = max(st[i][k-1], st[i+(1<<k)+1][k-1]);
}
void query(int l, int r){
    // 长度r-l+1，用两个长度log2(r-l+1)的区间覆盖
    int k = log2[r - l + 1];
    // 从两个覆盖区间中选最大值
    return max(st[l][k], st[r-(1<<k)+1][k]);
}
```
