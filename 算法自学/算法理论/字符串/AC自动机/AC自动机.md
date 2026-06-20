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

<svg viewBox="0 0 380 430" width="100%" height="100%" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <style>
      .node      { fill: #fafafa; stroke: #999; stroke-width: 1.5; }
      .node-text { font-family: sans-serif; font-size: 14px; fill: #333; text-anchor: middle; dominant-baseline: central; }
      .edge      { fill: none; stroke: #999; stroke-width: 1.5; }
      .fail-edge { fill: none; stroke: #c0392b; stroke-width: 2; stroke-dasharray: 6,4; }
      .fail-label{ font-family: sans-serif; font-size: 12px; fill: #c0392b; }
    </style>
    <!-- Regular arrowhead (gray) -->
    <marker id="arrowGray" markerWidth="8" markerHeight="6" refX="8" refY="3" orient="auto">
      <polygon points="0,0 8,3 0,6" fill="#999" />
    </marker>
    <!-- Fail arrowhead (dark red) -->
    <marker id="arrowRed" markerWidth="8" markerHeight="6" refX="8" refY="3" orient="auto">
      <polygon points="0,0 8,3 0,6" fill="#c0392b" />
    </marker>
  </defs>
  <!-- ===== Fail edges (drawn behind nodes) ===== -->
  <!-- fail 1: h2.west → h1.east, bend left -->
  <path class="fail-edge" marker-end="url(#arrowRed)" d="M 196,318 C 130,318 65,210 110,135" />
  <text class="fail-label" x="110" y="222">fail 1</text>
  <!-- fail 2: e2.west → e1.east, bend left -->
  <path class="fail-edge" marker-end="url(#arrowRed)" d="M 286,318 C 220,318 130,200 195,110" />
  <text class="fail-label" x="200" y="245">fail 2</text>
  <!-- ===== Regular edges ===== -->
  <!-- R → h1 -->
  <line class="edge" marker-end="url(#arrowGray)" x1="30" y1="210" x2="120" y2="120" />
  <!-- R → s1 -->
  <line class="edge" marker-end="url(#arrowGray)" x1="30" y1="210" x2="120" y2="300" />
  <!-- h1 → e1 -->
  <line class="edge" marker-end="url(#arrowGray)" x1="120" y1="120" x2="210" y2="102" />
  <!-- e1 → r1 -->
  <line class="edge" marker-end="url(#arrowGray)" x1="210" y1="102" x2="300" y2="102" />
  <!-- s1 → h2 -->
  <line class="edge" marker-end="url(#arrowGray)" x1="120" y1="300" x2="210" y2="318" />
  <!-- h2 → e2 -->
  <line class="edge" marker-end="url(#arrowGray)" x1="210" y1="318" x2="300" y2="318" />
  <!-- h1 → i1 -->
  <line class="edge" marker-end="url(#arrowGray)" x1="120" y1="120" x2="210" y2="210" />
  <!-- i1 → s2 -->
  <line class="edge" marker-end="url(#arrowGray)" x1="210" y1="210" x2="300" y2="174" />
  <!-- i1 → m1 -->
  <line class="edge" marker-end="url(#arrowGray)" x1="210" y1="210" x2="300" y2="246" />
  <!-- ===== Nodes ===== -->
  <circle class="node" cx="30"  cy="210" r="15" /><text class="node-text" x="30"  y="210">Root</text>
  <circle class="node" cx="120" cy="120" r="15" /><text class="node-text" x="120" y="120">h</text>
  <circle class="node" cx="210" cy="102" r="15" /><text class="node-text" x="210" y="102">e</text>
  <circle class="node" cx="300" cy="102" r="15" /><text class="node-text" x="300" y="102">r</text>
  <circle class="node" cx="120" cy="300" r="15" /><text class="node-text" x="120" y="300">s</text>
  <circle class="node" cx="210" cy="318" r="15" /><text class="node-text" x="210" y="318">h</text>
  <circle class="node" cx="300" cy="318" r="15" /><text class="node-text" x="300" y="318">e</text>
  <circle class="node" cx="210" cy="210" r="15" /><text class="node-text" x="210" y="210">i</text>
  <circle class="node" cx="300" cy="174" r="15" /><text class="node-text" x="300" y="174">s</text>
  <circle class="node" cx="300" cy="246" r="15" /><text class="node-text" x="300" y="246">m</text>
</svg>

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


