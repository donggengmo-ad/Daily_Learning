# AC自动机
[toc]
## 介绍
AC自动机本质是“Trie 上 KMP”，同时利用 Trie 的共享前缀特性，和 KMP 的复用匹配段特性，用于多模式匹配。

## fail 指针
### 定义
`fail` 指针指向 Trie 中节点，满足以下性质：
- 起始节点层数 $\gt$ 目标节点层数
- 目标节点的 Trie 上前缀与起始节点的 Trie 上前缀的一段后缀相同
- 目标节点的 Trie 上前缀与主串的后缀匹配
- 此目标节点使 Trie 上前缀尽可能长
> 找不到满足条件的节点时，fail 指针指向根节点

### 示例
Trie 中存储模式串 `he`、`his`、`him`、`she`、`her`
- **fail 1** 指针两端节点共享前缀 `he`
- **fail 2** 指针两端节点共享前缀 `h`

其余节点 fail 指针指向根节点

```tikz
\begin{tikzpicture}[
    gray,
    node/.style={
        circle, draw,
        minimum size= 8mm,
        inner sep=1pt
    },
    edge/.style={
        ->, >=stealth,
    },
    fail/.style={
        ->, >=stealth,
        thick, dashed,
        red!70!black
    },
]

\node[node]   (R)    at (0,   0)    {Root};
\node[node]   (h1)   at (1.5, 1.5)  {h};
\node[node]   (e1)   at (3,   1.8)  {e};
\node[node]   (r1)   at (4.5, 1.8)  {r};
\node[node]   (s1)   at (1.5,-1.5)  {s};
\node[node]   (h2)   at (3,  -1.8)  {h};
\node[node]   (e2)   at (4.5,-1.8)  {e};
\node[node]   (i1)   at (3,   0)  {i};
\node[node]   (s2)   at (4.5, 0.6)  {s};
\node[node]   (m1)   at (4.5,-0.6)  {m};

\draw[edge] (R)  -- (h1);
\draw[edge] (R)  -- (s1);
\draw[edge] (h1) -- (e1);
\draw[edge] (e1) -- (r1);
\draw[edge] (s1) -- (h2);
\draw[edge] (h2) -- (e2);
\draw[edge] (h1) -- (i1);
\draw[edge] (i1) -- (s2);
\draw[edge] (i1) -- (m1);

\draw[fail, bend left=25]
    (h2.west) to (h1.east)
    node[above=2, font=\color{red!70!black}] {fail 1};
\draw[fail, bend left=25]
    (e2.west) to (e1.east)
    node[above=2, font=\color{red!70!black}] {fail 2};

\end{tikzpicture}
```
### 意义
沿 `fail` 指针跳转时，
- 选择前缀中包含已匹配段的模式串，排除了其他不合法的模式串
- 复用上一个模式串中匹配过的前缀，跳过了重复匹配

### 存储
存在每个 Trie 节点里
```cpp
struct Node{
    int fail = 0; // 默认指向根节点
    bool end = false;
    vector<int> next;
};
vector<Node> trie;
```

## 构建 fail 指针
### 步骤
- BFS 遍历 Trie，从根节点开始
- 对于每个节点，找到父节点 `fail` 指针指向的节点记为 `fp`
- 如果 `fp` 的子节点有当前节点字符，则 `fail` 指针指向 `fp` 的该子节点
- 如果没有，则继续找 `fp` 的 `fail` 指针指向的节点
> 父节点 Trie 上前缀的后缀与 `fp` 的 Trie 上前缀必然匹配，再找与当前节点字符匹配的 `fp` 子节点，仍然匹配

### 代码
```cpp
void get_fail(){
    queue<int> q;
    // 处理第一层（fail跳转层数不减，与后续逻辑不兼容）
    for(int v: trie[0].next)
        // fail 已默认为根节点，直接入队
        if(v) q.push(v);
    // BFS 遍历 Trie
    while(!q.empty()){
        int u = q.front();
        q.pop();
        // 遍历 u 的所有子节点（26是示范，实际看字符范围）
        for(int c = 0;c < 26;c++){
            int v = trie[u].next[c];
            if(v){
                // 沿 fail 链找到子节点含 c 的节点或根节点
                int fp = trie[u].fail;
                while(fp && !trie[fp].next[c]) 
                    fp = trie[fp].fail;
                // 如果找到，则 fail 指针指向该子节点（否则就是根节点）
                if(trie[fp].next[c]) 
                    fp = trie[fp].next[c];
                // 设置 fail 指针
                trie[v].fail = fp;
                // 将子节点加入队列
                q.push(v);
            }
        }
    }
}
```

## 匹配
### 步骤
- 从根节点开始，遍历主串，逐字匹配
- 如不能匹配，则沿 fail 链跳转直到能匹配（或根节点）
- 再沿 fail 链统计所有匹配模式串
> 此时 fail 链上的模式串都是当前模式串的子串，所以一定能匹配

### 代码
这里只写统计匹配个数，要维护更多信息再自行灵活调整吧
```cpp
int match(string &s){
    int p = 0, res = 0;
    // 从根开始逐字匹配
    for(char c: s){
        // 沿 fail 链找到子节点含 c 的节点或根节点
        while(p && !trie[p].next[c-'a']) 
            p = trie[p].fail;
        // 如果有含c的子节点则跳转，继续匹配（否则就是根节点）
        if(trie[p].next[c-'a']) 
            p = trie[p].next[c-'a'];
        // 沿 fail 链统计所有匹配模式串
        for(int i = p;i;i = trie[i].fail) 
            res += trie[i].end;
    }
    return res;
}
```

## 复杂度
### 时间复杂度
- 构建 fail 指针：$O(n)$，n 是 Trie 中节点数
- 匹配：$O(m)$，m 是主串长度
### 空间复杂度
- $O(n)$，n 是 Trie 中节点数


