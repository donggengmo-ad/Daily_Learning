# STL-set
[TOC]
## 介绍
STL中的set是一个基于红黑树实现的集合

## 特点
- 元素唯一
- 有序
- 插入、删除、查找的时间复杂度为O(log n)
- 支持二分查找

## 方法
### 插入元素
1. `insert(value)`
插入一个元素
2. `emplace(args...)`
利用构造函数构造一个元素并插入
> 返回值：`pair<iterator, bool>`，
> - first 为插入位置的迭代器
> - second 为插入是否成功
> 
### 删除元素
1. `erase(iterator)`
删除指定位置的元素
2. `erase(iterator1, iterator2)`
删除两个迭代器之间的元素
3. `erase(value)`
删除指定值对应的元素
> 返回值：`size_type`，为删除的元素个数

### 查找元素
1. `find(value)`
   查找指定值对应的元素，返回迭代器
2. `count(value)`
   统计指定值出现的次数
> 返回值：`size_type`，为元素个数

### 二分查找
1. `lower_bound(value)`
   返回第一个大于等于value的元素的迭代器
2. `upper_bound(value)`
   返回第一个大于value的元素的迭代器
3. `equal_range(value)`
> 返回值：`pair<iterator, iterator>`，
> - first 为 lower_bound 的迭代器
> - second 为 upper_bound 的迭代器

### 其他操作
1. `empty()`
判断集合是否为空
2. `size()`
返回集合的大小
3. `clear()`
清空集合
4. `swap(set)`
交换两个集合

## 迭代器
set的迭代器不连续，也不支持下标访问
### 移动
- `it++` / `++it` 前进一个元素
- `it--` / `--it` 后退一个元素  
- `prev(it)` 取前一个元素的迭代器
- `next(it)` 取后一个元素的迭代器
- `advance(it, n)` 前进n个元素
### 访问元素
- `*it` 取元素值
- `it->member` 取元素的成员变量
### 对比
- `it1 == it2` 判断是否指向同一位置
- `*it1 == *it2` 判断两元素是否相等
- `distance(it1, it2)` 计算两个迭代器的距离
