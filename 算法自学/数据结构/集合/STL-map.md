# STL-map
[TOC]
## 介绍
红黑树实现的关联容器，存储键值对。

## 特点
- 允许值重复，不允许键重复
- 按键排序
- 插入、删除、查找O(log n)

## 方法
### 插入
- `mp[key] = value`
插入一个值，重复键覆盖，可能插入空值
- `insert({key, value})`
插入一个值，重复键不覆盖
- `emplace(key, value)`
构造并插入
> 返回值：pair<iterator, bool>
> - first 是插入位置的迭代器
> - second 是插入是否成功

### 查找
- `mp[key]`
访问一个值，不存在键自动插入空值并返回
- `mp.count(key)`
返回键的数量
- `mp.find(key)`
查找键，返回迭代器
- `mp.at(key)`
查找键，返回值，不存在键抛出异常

### 二分查找
- `lower_bound(key)`
返回第一个大于或等于键的迭代器
- `upper_bound(key)`
  返回第一个大于键的迭代器
- `equal_range(key)`
  返回一个pair，包含lower_bound和upper_bound的迭代器

### 删除
- `mp.erase(key)`
按键删除一个值
> 返回值`size_type`，删除的元素数量
- `mp.erase(it)`
按迭代器删除一个值
> 返回值`iterator`，指向下一个元素的迭代器
- `mp.erase(first, last)`
按范围删除元素
> 返回值`iterator`，指向下一个元素的迭代器

### 遍历
  `for (auto& [key, value] : mp)`
遍历键值对

### 其他
- `mp.empty()`
判断是否为空
- `mp.size()`
返回元素数量
- `mp.clear()`
清空元素
- `mp.swap(mp2)`
交换两个map

## 迭代器
不连续，不支持下标访问，不能与int运算
