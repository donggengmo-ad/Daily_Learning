# COW
——*[洛谷P3612](https://www.luogu.com.cn/problem/P3612)*
## 目录
[TOC]

## 题目
给定一个字符串 $s$
令 $F(s)$ 为 $s$ 后接 $s$ 向右“旋转”一个字符的结果
（在右旋转中，$s$ 的最后一个字符旋转并成为新的第一个字符）
给定初始字符串 $s$，重复用 $F$ 构建无限长度字符串


给定初始字符串和一个索引 $N$，计算无限代码字符串中第 $N$ 个位置的字符。

### 举例：

```
COW 8 -> C
```
#在这个例子中，初始字符串 COW 按以下方式扩展：
```
COW -> COWWCO -> COWWCOOCOWWC
```

## 思路
### 线性代数原理
*以COW为例:*
构造基向量  $P=(\text{C}, \text{O}, \text{W})$，得到坐标形式
$$
C=P
\begin{pmatrix}
1 \\
0 \\
0
\end{pmatrix}, \quad
O=P
\begin{pmatrix}
0 \\
1 \\
0
\end{pmatrix}, \quad
W=P
\begin{pmatrix}
0 \\
0 \\
1
\end{pmatrix}
$$
每一次延伸出的字符串，相当于原字符串做如下线性变换，记为A
$$ A=
\begin{pmatrix}
0 & 1 & 0 \\
0 & 0 & 1 \\
1 & 0 & 0
\end{pmatrix}
$$
对基向量进行线性变换 $A$
$$
P' = PA = (W,C,O)
$$
要保持字母不变，需要对其坐标做逆变换
$$
X = \begin{pmatrix}1\\0\\0\end{pmatrix}\ ,\ PX = C\\
X' = A^{-1}X = \begin{pmatrix}0\\1\\0\end{pmatrix}\\ 
P'X' = PAA^{-1}X = PX= C
$$
若已知经过 $i$ 次变换后，目标字母坐标为 $X'$
求目标字母在原字符串中的坐标 $X$
$$ 
X' = (A^{-1})^i X \rightarrow
X = A^i X'
$$


回顾线性变换A，发现是整体向上移一位，第一位移到最后
$$ A
\begin{pmatrix}
0 \\ 1 \\ 0
\end{pmatrix} =
\begin{pmatrix}
1 \\ 0 \\ 0
\end{pmatrix} \\
$$

这种坐标逆变换，映射到下标上
$$ 
(C,O,W)\to(s[0],s[1],s[2])\to(0,1,2)\\
(0,1,2)\begin{pmatrix}
0 \\ 1 \\ 0
\end{pmatrix} = 1 \to
(0,1,2)\begin{pmatrix}
1 \\ 0 \\ 0
\end{pmatrix} = 0
$$
也就是对应下标 $-1$

### 题目条件
每次变换，原字符串拼接新字符串，长度翻倍
$ s = s_0 + F(s_0) $
第 $n$ 个数下标为 $n-1$

设第 $i$ 次变换后，下标为 $n-1$ 的数产生
此时字符串长度为 $l = l_0 \cdot 2^i$，其中 $l_0$ 是 $s_0$ 长度

此时取$ x = (n-1) - \frac{l}{2} $
$x$为目标字符在后半段，即$F(s_0)$，内的相对下标

根据上述线性变换，$F(s_0)$ 中下标为 $x$ 的字符，与 $s_0$ 中的下标为 $x-1$ 的字符相同（$x-1<0$ 时，映射到 $s_0$ 中的最后一个字符）

因此，持续做 $ x = (x - \frac{l}{2}) - 1 $ 的变换
直到 $x < l_0$，此时 $x$ 就是目标字符在 $s_0$ 中的下标
输出 $s_0[x]$ 即可


## 代码实现
### 暴力模拟
```cpp
void F(string &s){
    ll len = s.length();
    s += s[len-1];
    for(ll i = 0;i < len - 1;i++) s += s[i];
}
int main(){
    string s;
    ll n;
    cin >> s >> n;
    while(s.length() < n) F(s);
    cout << s[n-1] << endl;
    return 0;
}
```
### 按上述思路实现
```cpp
ll halfpos(ll n, ll l0){
    // 找到当前下标所在 s 的长度
    ll pow2x = 1, x = 0;
    while(n >= l0 * pow2x){
        pow2x *= 2;
        x++;
    }
    // 取当前 s 长度的一半，即 s0 与 F(s0) 的长度
    ll len = l0 * (pow2x / 2);
    // 映射到 F(s0) 中的下标
    n -= len;
    // 再映射到 s0 中的下标
    n--;
    if(n < 0) n = len - 1;
    return n;
}
int main(){
    string s;
    ll n;
    cin >> s >> n;
    ll len = s.length();
    // 下标从0开始
    n--;
    // 重复映射，直到下标在原始字符串中
    while(n >= len) n = halfpos(n, len);
    cout << s[n] << endl;
    return 0;
}
```