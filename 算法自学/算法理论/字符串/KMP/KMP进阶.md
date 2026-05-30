# KMP进阶
[toc]

## nextval 优化
### 优化
普通 next 数组，如果跳转后到达位置的字符与跳转前相同，则必然不匹配，要继续跳转。
|i|0|1|2|3|4|5|
|-|-|-|-|-|-|-|
|str|a|a|a|a|a|b|
|next|-1|0|1|2|3|-1|
> 上例中，i = 4 时要经过 3、2、1、0、-1，途中全部不匹配

### 定义
nextval 数组记录跳转后第一个满足字符与跳转前字符不同的位置
|i|0|1|2|3|4|5|
|-|-|-|-|-|-|-|
|str|a|a|a|a|a|b|
|next|-1|0|1|2|3|-1|
|nextval|-1|-1|-1|-1|-1|-1|

沿 nextval 数组跳转，可一次抵达最终位置

### 构建 nextval 数组
基于 next 数组，如果跳转后字符相同则继承跳转后的 nextval 
> 代码省略普通next构建过程
```cpp
vector<int> next;
string p;
vector<int> get_nextval(){
    vector<int> nextval(p.size(), -1);
    for(int i = 1; i < p.size(); i++){
        // 跳转后字符相同，继承跳转后的 nextval
        if(p[i] == p[next[i]])
            nextval[i] = nextval[next[i]];
        // 跳转后字符不同，和普通 next 一样
        else nextval[i] = next[i];
    }
    return nextval;
}
```

### 匹配
匹配时沿 nextval 数组跳转即可
```cpp
vector<int> match(string s){
    int n = s.size(), m = p.size();
    vector<int> res;
    int i = 0, j = 0;
    while(i < n && j < m){
        if(j == -1 || s[i] == p[j]){
            i++, j++;
            if(j == m){
                res.push_back(i - j);
                // 注意，匹配成功必须沿 next 跳转，否则会漏掉匹配
                j = next[j];
            }
        }
        // 匹配失败沿 nextval 跳转
        else j = nextval[j];
    }
}
```


## 最小周期
### 定义
字符串中最短的子串，使得字符串可以由这个子串重复而构成

### 公式
#### 周期长度
$T = m - next[m]$
> `m` 为字符串长度，`next[i]` 定义为 `i` 位置不包含自身的前缀 $\pi$ 值
#### 纯周期
纯周期串表示字符串可以由最小周期子串重复整数次构成，满足：
$m\mod T = 0$

### 证明
#### 周期性
$s_i = s_{i+T}$
#### 关于 $\pi$ 的分类讨论
- $\pi \gt \frac{m}{2}$

|i|0|1|2|3|4|5|-||
|:-:|:-:|-|-|-|:-:|:-:|:-:|:-:|
|$str$|A|B|X|X|A|B|-
|$T_1$|A|B|X|X|-|
|$T_2$|-||||A|B|X|X|

- $\pi = \frac{m}{2}$

|i|0|1|2|3|4|5|
|:-:|:-:|-|-|:-:|:-:|:-:|
|$str$|A|B|C|A|B|C|
|$T_1$|A|B|C|-||
|$T_2$|-|||A|B|C|

- $\pi \lt \frac{m}{2}$

|i|0|1|2|3|4|5|
|:-:|:-:|-|-|-|:-:|:-:|
|$str$|A|B|A|B|A|B|
|$T_1$|A|B|A|B|-|
|$T_2$|-||A|B|A|B|
> 本表中的 $T_1$ 和 $T_2$ 不是严格最小周期，可观察到 $T_1$ 后半段与 $T_2$ 前半段 “AB” 重复，故可缩减周期

