#include <iostream>
#include <list>
#include <unordered_map>
using namespace std;

//# 链表STL
// list是一个双向链表，可以O(1)插入和删除。
// 原生访问O(n)，但可以用哈希表存储迭代器来实现O(1)访问。

int main(){
    list<int> myList;
    // 添加元素
    myList.push_back(1);
    myList.push_front(0);
    // 插入元素
    auto it = myList.begin();
    myList.insert(it, -1);
    // 删除元素
    myList.pop_back();
    myList.pop_front();
    myList.erase(myList.begin()); // 按位置删除
    myList.remove(1); // 按值删除 
    // 访问元素
    myList.front();
    myList.back();
    find(myList.begin(), myList.end(), 1); // 查找元素
    // 迭代器移动
    auto it2 = myList.begin();
    advance(it2, 3); 
    // 遍历链表
    for (int x : myList) cout << x << " ";
    for (auto it = myList.begin(); it != myList.end(); ++it) cout << *it << " ";
    // 其他操作
    myList.clear(); // 清空链表
    myList.size(); // 链表大小
    myList.empty(); // 是否为空
    myList.sort(); // 排序
    myList.reverse(); // 反转链表
    myList.unique(); // 去重
    list <int> otherList = {2, 3, 4};
    myList.merge(otherList); // 合并两个链表（要求有序）

    // 配合哈希表实现O(1)访问
    unordered_map<int, list<int>::iterator> idxMap; // 存储元素值到迭代器的映射
    list<int> myList2 = {10, 20, 30};
    for (auto it = myList2.begin(); it != myList2.end(); ++it) {
        idxMap[*it] = it; // 元素值作为键，迭代器作为值存储在哈希表中
    }
    // 检查是否存在
    idxMap.count(20); // 返回1表示存在，0表示不存在
    idxMap.find(20) != idxMap.end(); // 另一种检查方式
    // 访问迭代器
    auto it3 = idxMap[20]; 

    return 0;
}