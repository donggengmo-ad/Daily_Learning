#include <iostream>
using namespace std;

//# 三个简单排序
//$ 时间复杂度：O(n^2)，空间复杂度：O(1)
// 冒泡排序
void bubbleSort(int arr[], int n){
    for(int i = 1;i < n;i++){
        for(int j = 0;j < n - i;j++){
            if(arr[j] > arr[j + 1]){
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}
// 选择排序
void selectionSort(int arr[], int n){
    for(int i = 0;i < n - 1;i++){
        int min_idx = i;
        for(int j = i;j < n;j++){
            if(arr[min_idx] > arr[j]) min_idx = j;
        }
        swap(arr[i], arr[min_idx]);
    }
}
// 插入排序
void insertionSort(int arr[], int n){
    for(int i = 1;i < n;i++){
        for(int j = i - 1;j >= 0;j--){
            if(arr[j] > arr[j + 1]){
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

//# 桶排序
//$ 时间复杂度：O(n+k)，空间复杂度：O(n+k)，n-数组长度，k-桶数量
void bucketSort(int arr[], int n){
    if(n <= 0) return;
    // 创建桶（下标代表数值，值代表了该数值出现的次数）
    int bucket[10001] = {0}; /// 只能处理小于10000的数
    // 将元素放入桶中
    int max = 0;
    for(int i = 0; i < n; i++){
        bucket[arr[i]]++;
        max = max > arr[i] ? max : arr[i];
    }
    // 将桶中的元素放回原数组
    int idx = 0;
    for(int i = 0; i <= max; i++){/// 只需要遍历到最大值即可
        // 一个数值放完
        for(int j = 0; j < bucket[i]; j++){
            // 从前往后放（升序）
            arr[idx++] = i;
        }
    }
}

void printArr(int arr[], int n){
    for(int i = 0; i < n; i++){
        cout << arr[i] << " ";
    }
    cout << endl;
}
int main(){
    int arr[] = {5, 3, 2, 4, 1};
    int n = 5;
    
    bucketSort(arr, n);
    printArr(arr, n);

    bubbleSort(arr, n);
    printArr(arr, n);

    selectionSort(arr, n);
    printArr(arr, n);

    insertionSort(arr, n);
    printArr(arr, n);

    return 0;
}