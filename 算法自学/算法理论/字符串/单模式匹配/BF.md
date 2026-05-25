# BF算法（Brute Force
[toc]
## 基本思想
通过在主串中逐个尝试可能的匹配位置，每次比较整个模式串。

## 步骤
- 初始化指针 `i` `j`，指向主串和模式串的起始位置。
- `i` 指针遍历主串，搜索匹配位置。
- `j` 指针遍历模式串，比较字符。
- 重置 `j` 指针，移动 `i` 指针，继续搜索。

## 代码
```cpp
vector<int> BF(string t, string p){
    vector<int> res;
    for(int i = 0;i < t.size();i++){
        for(int j = 0;j < p.size();j++) 
           if(t[i+j] != p[j]) break;
        if(j == p.size()) res.push_back(i);
    }
    return res;
}
```

## 复杂度
- 时间复杂度：$O(nm)$，其中 $n$ 为主串长度，$m$ 为模式串长度。
- 空间复杂度：$O(1)$，不使用额外空间。

## 应用
- 小数据量匹配
- 简单字符串匹配
- 模式串长度远小于主串长度
  