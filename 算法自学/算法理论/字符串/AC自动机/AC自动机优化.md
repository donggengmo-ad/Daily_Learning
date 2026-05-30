# AC自动机优化
[toc]
## Trie 图优化
### 优化
现在每次要遍历 fail 链，时间复杂度高，可以使用 Trie 图优化
- 每个节点的空 next 指向 fail 链上第一个能匹配该节点字符的节点
- `p = trie[p].next[c]` 可以直接跳转，无需再沿 fail 链找
> 其实就是路径压缩 :rofl:

### 示例
绿色箭头演示部分空 next 引导，其余大多导向根节点
```tikz
\begin{tikzpicture}[
    gray,
    node/.style={
        circle, draw,
        minimum size=9mm,
        inner sep=1pt,
    },
    edge/.style={
        ->, >=stealth,
        black!70, thick
    },
    fail/.style={
        ->, >=stealth,
        thick, dashed,
        red!70!black
    },
    trie-edge/.style={
        ->, >=stealth,
        thick, dashed,
        green!50!black
    }
]

\node[node]   (R)    at (0,   0)    {Root};
\node[node]   (h1)   at (1.5, 1.5)  {h};
\node[node]   (e1)   at (3,   1.8)  {e};
\node[node]   (r1)   at (4.5, 1.8)  {r};
\node[node]   (s1)   at (1.5,-1.5)  {s};
\node[node]   (h2)   at (3,  -1.8)  {h};
\node[node]   (e2)   at (4.5,-1.8)  {e};
\node[node]   (i1)   at (3,   0)    {i};
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
    node[above=2, font=\color{red!70!black}] {fail};
\draw[fail, bend left=25]
    (e2.west) to (e1.east)
    node[above=2, font=\color{red!70!black}] {fail};

\draw[trie-edge, bend left=40]
    (h1.south) to (s1.north)
    node[above=20, right=10, font=\color{red!70!black}] {s};
\draw[trie-edge, bend left=40]
    (s1.north) to (h1.south)
    node[above=-40, left=10, font=\color{red!70!black}] {h};
\draw[trie-edge, bend right=20]
    (e2.east) to (r1.east)
    node[above=-40, right=10, font=\color{red!70!black}] {r};
\draw[trie-edge, bend right=20]
    (i1.north) to (h1.east)
    node[above=-20, right=10, font=\color{red!70!black}] {h};
\draw[trie-edge, bend left=30]
    (h2.south) to (s1.south)
    node[above=-2, right=15, font=\color{red!70!black}] {s};

\end{tikzpicture}
```

### 构建 fail 指针
```cpp
void get_fail(){
    trie[0].fail = 0;
    queue<int> q;
    // 处理第一层
    for(int v: trie[0].next){
        if(v){
            // 不为空正常处理
            trie[v].fail = 0;
            q.push(v);
        }
        // 空 next 直接指向根，这里不用改
    }
    while(!q.empty()){
        int u = q.front();
        q.pop();
        // 遍历 u 的所有子节点
        for(int c = 0;c < 26;c++){
            int v = trie[u].next[c];
            int fp = trie[u].fail;
            if(v){
                // fp 的空 next 已经处理过，直接跳转
                trie[v].fail = trie[fp].next[c];
                q.push(v);
            }
            // 空 next 指向 fp 的 c 子节点，会一路引导向 c 第一次出现的地方
            else trie[u].next[c] = trie[fp].next[c];
        }
    }
}
```
### 匹配
直接按 next 跳转即可，会自动引导向 fail 链上能匹配的节点
```cpp
int match(string &s){
    int p = 0, res = 0;
    for(char c: s){
        // 直接按 next 跳转
        p = trie[p].next[c-'a'];
        // 在 fail 链上统计，这个不能省
        for(int i = p;i;i = trie[i].fail) 
            res += trie[i].end;
    }
    return res;
}
```

## last 优化
### 优化
每次匹配时统计 fail 链上，会经过很多不为模式串结尾的无意义节点，可以使用 last 指针优化

### 定义
last[u] 表示 u 的fail 链上第一个为模式串结尾
$$last[u] = \begin{cases}
    fail[u] & \text{fail[u] 为模式串结尾} \\
    last[fail[u]] & \text{fail[u] 不为模式串结尾}
\end{cases}$$

### 构建 last 指针
BFS时顺便构建 last 指针
```cpp
void get_fail_last(){
    trie[0].fail = 0;
    queue<int> q;
    for(int v: trie[0].next){
        if(v){
            trie[v].fail = 0;
            // 第一次 last 指向根
            trie[v].last = 0;
            q.push(v);
        }
    }
    while(!q.empty()){
        int u = q.front();
        q.pop();
        for(int c = 0;c < 26;c++){
            int v = trie[u].next[c];
            int fp = trie[u].fail;
            if(v){
                trie[v].fail = trie[fp].next[c];
                // 如果 fail[u] 为模式串结尾，last[u] 为 fail[u]
                int fv = trie[v].fail;
                if(trie[fv].end) trie[v].last = fv;
                // 不是结尾，则继承 fail[u] 的 last，引导向第一个结尾
                else trie[v].last = trie[fv].last;
                q.push(v);
            }
            else trie[u].next[c] = trie[fp].next[c];
        }
    }
}
```

### 匹配
统计时沿 last 链统计就好
```cpp
int match(string &s){
    int p = 0, res = 0;
    for(char c: s){
        p = trie[p].next[c-'a'];
        // 沿 last 链统计，也可维护其他信息
        for(int i = p;i;i = trie[i].last)
            res += trie[i].end;
    }
    return res;
}
```
