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
//$ 时间复杂度：O(n^2)，空间复杂度：O(1)，稳定
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
//$ 时间复杂度：O(n^2)，空间复杂度：O(1)，不稳定
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
//$ 时间复杂度：O(n^2)，空间复杂度：O(1)，稳定
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

//# 希尔排序（插入排序的改进版）
//$ 时间复杂度：O(n^1.3)，空间复杂度：O(1)，不稳定
//? 先比较距离较远的元素，逐渐缩小比较距离
void shellSort(int arr[], int n){
    // gap为比较距离，初始为n/2，每次缩小一半
    for(int gap = n / 2;gap > 0;gap /= 2){
        // 以gap为间隔进行插入排序
        for(int i = gap;i < n;i++){
            for(int j = i;j >= gap;j -= gap){
                if(arr[j] < arr[j - gap]){
                    swap(arr[j], arr[j - gap]);
                }
            }
        }
    }
}

int main(){
    int arr[] = {9,3,6,1,2,6,9,10};

    shellSort(arr, 8);
    for(int a:arr) cout << a << ' ';
    
    return 0;
}
