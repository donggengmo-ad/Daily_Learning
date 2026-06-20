# KMP算法
[toc]

## 介绍
KMP是通过复用已匹配段，避免重复遍历的单模式匹配算法

## 原理
### 前缀函数 $\pi$
$\pi$ 表示字符串中，最长公共前后缀的长度（不包含自身）
|字符|A|B|C|D|A|B|C|
|-|-|-|-|-|-|-|-|
|匹配|$\checkmark$|$\checkmark$|$\checkmark$|$\times$|$\checkmark$|$\checkmark$|$\checkmark$|

### next 数组
`next[i]` 记录长度为 `i` 的前缀的 $\pi$ 值
|字符|空|A|B|C|D|A|B|C|
|-|-|-|-|-|-|-|-|-|
|前缀|空|A|AB|ABC|ABCD|ABCDA|ABCDAB|ABCDABC|
|$\pi$|无|0|0|0|0|1|2|3|
|next|-1|0|0|0|0|1|2|3|

这里的 $\pi$ 值其实就是前缀第一个不匹配位置的下标
> 我觉得AC自动机里 `fail` 指针的解释理念比这里的 `next` 数组更直观，可以借用一下。

<svg viewBox="0 0 250 110" width="100%" height="100%" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <style>
      .cell { stroke: #666; stroke-width: 1.5; }
      .text { font-family: monospace; font-size: 14px; fill: #333; text-anchor: middle; dominant-baseline: central; }
      .arrow { fill: none; stroke-width: 2; }
    </style>
    <!-- 箭头 marker -->
    <marker id="arrowRed" markerWidth="6" markerHeight="6" refX="6" refY="3" orient="auto">
      <path d="M0,0 L6,3 L0,6 Z" fill="#d32f2f" />
    </marker>
    <marker id="arrowYellow" markerWidth="6" markerHeight="6" refX="6" refY="3" orient="auto">
      <path d="M0,0 L6,3 L0,6 Z" fill="#fbc02d" />
    </marker>
    <marker id="arrowGreen" markerWidth="6" markerHeight="6" refX="6" refY="3" orient="auto">
      <path d="M0,0 L6,3 L0,6 Z" fill="#388e3c" />
    </marker>
  </defs>
  <!-- 单元格 -->
  <g transform="translate(10, 50)">
    <!-- a0: A (匹配) 红色背景 -->
    <rect class="cell" x="0" y="0" width="30" height="30" fill="#d32f2f" /><text class="text" x="15" y="15">A</text>
    <!-- a1: B (匹配) 黄色背景 -->
    <rect class="cell" x="30" y="0" width="30" height="30" fill="#fbc02d" /><text class="text" x="45" y="15">B</text>
    <!-- a2: C (匹配) 绿色背景 -->
    <rect class="cell" x="60" y="0" width="30" height="30" fill="#388e3c" /><text class="text" x="75" y="15">C</text>
    <!-- a3: D 保持灰色 -->
    <rect class="cell" x="90" y="0" width="30" height="30" fill="#f5f5f5" /><text class="text" x="105" y="15">D</text>
    <!-- a4: A (匹配) 红色背景 -->
    <rect class="cell" x="120" y="0" width="30" height="30" fill="#d32f2f" /><text class="text" x="135" y="15">A</text>
    <!-- a5: B (匹配) 黄色背景 -->
    <rect class="cell" x="150" y="0" width="30" height="30" fill="#fbc02d" /><text class="text" x="165" y="15">B</text>
    <!-- a6: C (匹配) 绿色背景 -->
    <rect class="cell" x="180" y="0" width="30" height="30" fill="#388e3c" /><text class="text" x="195" y="15">C</text>
  </g>
  <!-- 下标 -->
  <g font-family="sans-serif" font-size="10" fill="#555" text-anchor="middle">
    <text x="25" y="90">0</text>
    <text x="55" y="90">1</text>
    <text x="85" y="90">2</text>
    <text x="115" y="90">3</text>
    <text x="145" y="90">4</text>
    <text x="175" y="90">5</text>
    <text x="205" y="90">6</text>
  </g>
  <!-- 曲线箭头：红、黄、绿 -->
  <path class="arrow" stroke="#d32f2f" marker-end="url(#arrowRed)"
        d="M 145,50 C 145,10  60,50  40,60" />
  <path class="arrow" stroke="#fbc02d" marker-end="url(#arrowYellow)"
        d="M 175,50 C 175,10  85,50  70,60" />
  <path class="arrow" stroke="#388e3c" marker-end="url(#arrowGreen)"
        d="M 205,50 C 205,10  115,50  100,60" />
</svg>

> 这张图里箭头指向最后一个匹配位置与第一个不匹配位置的交界

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

<svg viewBox="0 0 320 280" width="100%" height="100%" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <style>
      .cell      { stroke: #666; stroke-width: 1.5; }
      .text      { font-family: monospace; font-size: 14px; fill: #333; text-anchor: middle; dominant-baseline: central; }
      .label     { font-family: sans-serif; font-size: 13px; fill: #555; text-anchor: end; dominant-baseline: central; }
      .badge     { font-family: sans-serif; font-size: 11px; text-anchor: middle; dominant-baseline: central; }
      .dashed    { stroke: #999; stroke-dasharray: 5,3; }
      .dotted    { stroke: #999; stroke-dasharray: 2,3; fill: none; }
      .arrow-line { fill: none; stroke: #777; stroke-width: 1.5; }
      .failed    { font-family: serif; font-size: 16px; fill: #d32f2f; text-anchor: middle; dominant-baseline: central; }
      .pointer   { fill: none; stroke: #777; stroke-width: 1.8; }
    </style>
    <marker id="arrowHead" markerWidth="7" markerHeight="5" refX="7" refY="2.5" orient="auto">
      <polygon points="0,0 7,2.5 0,5" fill="#777" />
    </marker>
    <marker id="iArrow" markerWidth="7" markerHeight="5" refX="8" refY="2.5" orient="auto">
      <polygon points="1,0 7,2.5 1,5" fill="#d32f2f" />
    </marker>
    <marker id="jArrow" markerWidth="7" markerHeight="5" refX="7" refY="2.5" orient="auto">
      <polygon points="0,0 7,2.5 0,5" fill="#777" />
    </marker>
  </defs>
  <!-- ===== Text T ===== -->
  <text class="label" x="30" y="42">T:</text>
  <rect class="cell" x="40" y="30" width="30" height="26" fill="#31ec31" /><text class="text" x="55" y="43">A</text>
  <rect class="cell" x="70" y="30" width="30" height="26" fill="#55adff" /><text class="text" x="85" y="43">B</text>
  <rect class="cell" x="100" y="30" width="30" height="26" fill="#31ec31" /><text class="text" x="115" y="43">A</text>
  <rect class="cell" x="130" y="30" width="30" height="26" fill="#55adff" /><text class="text" x="145" y="43">B</text>
  <rect class="cell" x="160" y="30" width="30" height="26" fill="#31ec31" /><text class="text" x="175" y="43">A</text>
  <rect class="cell" x="190" y="30" width="30" height="26" fill="#fafafa" /><text class="text" x="205" y="43">B</text>
  <rect class="cell" x="220" y="30" width="30" height="26" fill="#fafafa" /><text class="text" x="235" y="43">C</text>
  <!-- i pointer -->
  <line class="pointer" stroke="#d32f2f" x1="175" y1="30" x2="175" y2="8" marker-end="url(#iArrow)" />
  <text class="badge" font-weight="bold" fill="#d32f2f" x="175" y="4">i</text>
  <!-- ===== Pattern P ===== -->
  <text class="label" x="30" y="108">P:</text>
  <rect class="cell" x="40" y="96" width="30" height="26" fill="#31ec31" /><text class="text" x="55" y="109">A</text>
  <rect class="cell" x="70" y="96" width="30" height="26" fill="#55adff" /><text class="text" x="85" y="109">B</text>
  <rect class="cell" x="100" y="96" width="30" height="26" fill="#31ec31" /><text class="text" x="115" y="109">A</text>
  <rect class="cell" x="130" y="96" width="30" height="26" fill="#55adff" /><text class="text" x="145" y="109">B</text>
  <rect class="cell" x="160" y="96" width="30" height="26" fill="#f63e3e" /><text class="text" x="175" y="109">C</text>
  <!-- Mismatch cross -->
  <text class="failed" x="175" y="75">✕</text>
  <!-- Separator -->
  <line class="dashed" x1="10" y1="140" x2="270" y2="140" />
  <!-- ===== Pattern P' (shifted) ===== -->
  <text class="label" x="30" y="182">P′:</text>
  <rect class="cell" x="100" y="170" width="30" height="26" fill="#31ec31" /><text class="text" x="115" y="183">A</text>
  <rect class="cell" x="130" y="170" width="30" height="26" fill="#55adff" /><text class="text" x="145" y="183">B</text>
  <rect class="cell" x="160" y="170" width="30" height="26" fill="#31ec31" /><text class="text" x="175" y="183">A</text>
  <rect class="cell" x="190" y="170" width="30" height="26" fill="#fafafa" /><text class="text" x="205" y="183">B</text>
  <rect class="cell" x="220" y="170" width="30" height="26" fill="#fafafa" /><text class="text" x="235" y="183">C</text>
  <!-- j pointer -->
  <line class="pointer" x1="175" y1="196" x2="175" y2="230" marker-end="url(#jArrow)" />
  <text class="badge" font-weight="bold" x="175" y="245" fill="#727272">j</text>
  <!-- Alignment dotted lines -->
  <line class="dotted" x1="115" y1="56" x2="115" y2="170" />
  <line class="dotted" x1="145" y1="56" x2="145" y2="170" />
  <!-- Shift arrow -->
  <path class="arrow-line" marker-end="url(#arrowHead)" fill="none"
        d="M 85,130 C 85,150  125,150  150,165" />
  <text class="badge" x="85" y="145" fill="#777" font-size="10">shift</text>
</svg>

### next 数组构建
模式串自己的前缀、后缀相匹配
- 后缀部分视为主串，指针 `i`，负责更新 `next` 数组
- 前缀部分视为模式串，指针 `j`，负责检查匹配
- `j < i` 恒成立，只依赖之前的结果
> 注意，这样构建的 `next` 数组，`next[i]` 代表的是 `i` 不包含自身前缀的 $\pi$ 值

## 代码
上述移动模式串（或者说匹配点跳转），在代码中就是 `j = next[j]`
### next 数组构建
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
### 匹配
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

