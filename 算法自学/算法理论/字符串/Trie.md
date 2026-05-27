# Trie 字典树（前缀树）
[toc]
## 介绍
用于快速检索字符串的树形结构。

也可以理解为有限状态自动机
- **状态**：每个节点
- **可接受状态**：标记节点
- **状态转移**：边

## 实现
每个节点
- 维护一个字符，表示当前的字符
- 记录一个标志位，表示是否可接受
- 维护一个指针，指向子节点
> 指针通常用 `unordered_mmap` 或下标 `s[i]-'a'` 的数组表示

## 代码
> 以 `unordered_map` 实现为例
### 定义
```cpp
struct Node{
    bool in = false;
    unordered_map<char, int> next;
};
vector<Node> trie(1);
```
### 插入
- 从根节点开始，遍历字符串中的每个字符
- 如果对应子节点不存在则创建新节点
- 移动到对应子节点
- 最后标记可接受状态
```cpp
void insert(string s){
    // 当前树上位置
    int p = 0;
    // 遍历字符
    for(char c : s){
        // 如子节点不存在则创建新节点
        if(!trie[p].next.count(c)){
            trie[p].next[c] = trie.size();
            trie.emplace_back(c);
        }
        // 移动到子节点
        p = trie[p].next[c];
    }
    // 标记可接受状态
    trie[p].in = true;
}
```
### 查找
- 从根节点开始，遍历字符串中的每个字符
- 如果对应子节点不存在则返回 false
- 移动到对应子节点
- 如果找到可接受状态则返回 true
```cpp
bool find(string s){
    // 当前树上位置
    int p = 0;
    // 遍历字符
    for(char c : s){
        // 如子节点不存在则返回 false
        if(!trie[p].next.count(c)) return false;
        // 移动到子节点
        p = trie[p].next[c];
    }
    // 返回可接受状态
    return trie[p].in;
}
```
### 删除
- 额外维护计数器 `pass` 表示经过某个节点的次数
- 删除时把路径上的计数器减一
```cpp
void del(const string &s){
    // 当前树上位置
    int p = 0;
    // 遍历字符
    for(char c : s){
        // 不存在则不用删除
        if(!trie[p].next.count(c)) return;
        // 移动到子节点
        p = trie[p].next[c];
        // 统计经过次数
        trie[p].pass--;
    }
    // 删除可接受状态
    trie[p].in = false;
}
```

## 复杂度
- 空间复杂度：$O(n)$，$n$为字符集的大小
- 时间复杂度：$O(m)$，$m$为待查询字符串的长度

# 应用
- **前缀匹配**：Trie前缀遍历可以快速检索字符串
- **01Trie**：存储二进制数，用于求解异或问题
- **字典序排序**：Trie前序遍历就是字典序
