# KMP算法

## 介绍
通过复用已匹配段，避免重复遍历优化的BF算法

## 原理
### 前缀函数 $\pi$
$\pi$ 表示字符串中，最长公共前后缀的长度
|字符|A|B|C|D|A|B|C|
|-|-|-|-|-|-|-|-|
|匹配|$\checkmark$|$\checkmark$|$\checkmark$|$\times$|$\checkmark$|$\checkmark$|$\checkmark$|

### next数组
`next[i]` 记录长度为 `i` 的前缀的 $\pi$ 值
|前缀|空|A|AB|ABC|ABCD|ABCDA|ABCDAB|ABCDABC|
|-|-|-|-|-|-|-|-|-|
|$\pi$|无|0|0|0|0|1|2|3|
|next|-1|0|0|0|0|1|2|3|

### 失配处理
- 匹配失败时，失配位置之前可匹配。
- 模式串 $\pi$ 长后缀与前缀相同，且可匹配
- 移动模式串使 $\pi$ 长前缀对准原后缀位置，可继续匹配
>本质上是，略过了模式串 $\pi$ 长前缀与主串逐一匹配的过程，因为必然匹配

|主串|A|B|A|B|A|B|C|
|-|-|-|-|-|-|:-:|-|
|模式串|A|B|A|B|C|-|
|移动后|-||A|B|A|B|C|
|匹配指针|-||||`j`|-|||

```tikz
\begin{tikzpicture}[
  gray,
  box/.style={draw, minimum size=8mm},
  mgreen/.style={draw, minimum size=8mm, fill=green!20},
  mred/.style={draw, minimum size=8mm, fill=red!20},
]
% === Text T: A B A B A B C ===
\node at (-0.6, 0) {T:};
\node[mgreen] (T0) at (0.0, 0) {A};
\node[mgreen] (T1) at (1.0, 0) {B};
\node[mgreen] (T2) at (2.0, 0) {A};
\node[mgreen] (T3) at (3.0, 0) {B};
\node[mred]   (T4) at (4.0, 0) {A};
\node[box]    (T5) at (5.0, 0) {B};
\node[box]    (T6) at (6.0, 0) {C};
% i pointer — appears once, fixed above text
\draw[->] (T4.north) -- +(0, 0.7) node[above] {i};
% === Pattern P (before shift): A B A B C ===
\node at (-0.6, -1.4) {P:};
\node[mgreen] (P0) at (0.0, -1.4) {A};
\node[mgreen] (P1) at (1.0, -1.4) {B};
\node[mgreen] (P2) at (2.0, -1.4) {A};
\node[mgreen] (P3) at (3.0, -1.4) {B};
\node[mred]   (P4) at (4.0, -1.4) {C};
\node at (4.0, -0.7) {$\times$};
% Separator
\draw[dashed] (-1.0, -2.2) -- (7.0, -2.2);
% === Pattern after shift (right by 2) ===
\node at (-0.6, -3.0) {P$'$:};
\node[mgreen] (S0) at (2.0, -3.0) {A};
\node[mgreen] (S1) at (3.0, -3.0) {B};
\node[box]    (S2) at (4.0, -3.0) {A};
\node[box]    (S3) at (5.0, -3.0) {B};
\node[box]    (S4) at (6.0, -3.0) {C};
% j pointer — appears once, on shifted pattern
\draw[->] (S2.south) -- +(0, -0.6) node[below] {j};
% Shift arrow
\draw[->] (1.5, -1.8) -- (3.5, -2.5) node[midway, left] {shift};
% Dotted lines: prefix-suffix alignment
\draw[dotted] (T2) -- (S0);
\draw[dotted] (T3) -- (S1);
\end{tikzpicture}
```

### next数组的计算
模式串自己的前缀、后缀相匹配
- 后缀部分视为主串，指针 `i`，负责更新 `next` 数组
- 前缀部分视为模式串，指针 `j`，负责检查匹配
- `j < i` 恒成立，只依赖之前的结果

## 代码
上述移动模式串，在代码中就是 `j = next[j]`
### get_next函数
```cpp
vector<int> get_next(string &p){
    int n = p.size();
    vector<int> next(n, -1);
    int i = 0, j = -1;
    while(i < n){
        // 没开始匹配，或匹配成功，进一位并记录
        if(j == -1 || p[i] == p[j]) next[++i] = ++j;
        // 匹配失败，回退模式串指针
        else j = next[j];
    }
    return next;
}
```
### kmp函数
```cpp
vector<int> kmp(string &s, string &p){
    int n = s.size(), m = p.size();
    vector<int> res;
    // 初始化next数组
    vector<int> next = get_next(p);
    // 主串指针，模式串指针
    int i = 0, j = 0;
    while(i < n && j < m){
        // 未开始匹配，或匹配成功，进一位
        if(j == -1 || s[i] == p[j]){
            i++, j++;
            // 匹配成功，记录位置
            if(j == m){
                res.push_back(i - j);
                j = next[j]; // 重置模式串指针
            }
        }
        // 匹配失败，回退模式串指针
        else j = next[j];
    }
    return res;
}
```

## 复杂度
主串指针不会退，线性扫描一次
- 时间复杂度：$O(n + m)$
- 空间复杂度：$O(m)$

