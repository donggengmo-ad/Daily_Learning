#include <iostream>
using namespace std;

/*
?   方法：
?       比较相邻元素或整体比较，按照一定规则交换位置
?   特点：
?       时间复杂度多为O(n^2)（较高），空间复杂度多为O(1)（较低）
?   适用场景：
?       数据规模较小，或者数据基本有序（接近排序完成）
*/

//# 冒泡排序
//$ 时间复杂度：O(n^2)，空间复杂度：O(1)
//? 比较相邻，不合规则互换
void bubbleSort(int arr[], int n){
    for(int i = 1;i < n;i++){
        for(int j = 0;j < n - i;j++){
            if(arr[j] > arr[j + 1]){
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}
//# 选择排序
//$ 时间复杂度：O(n^2)，空间复杂度：O(1)
//? 每次选择最小的放到前面
void selectionSort(int arr[], int n){
    for(int i = 0;i < n - 1;i++){
        int min_idx = i;
        for(int j = i;j < n;j++){
            if(arr[min_idx] > arr[j]) min_idx = j;
        }
        swap(arr[i], arr[min_idx]);
    }
}
//# 插入排序
//$ 时间复杂度：O(n^2)，空间复杂度：O(1)
//? 每次将一个元素插入到前面已经排好序的数组中（像玩扑克牌）
void insertionSort(int arr[], int n){
    for(int i = 1;i < n;i++){
        for(int j = i - 1;j >= 0;j--){
            if(arr[j] > arr[j + 1]){
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

