#include <iostream>
using namespace std;

//# 归并排序
//$ 时间复杂度：O(nlogn)，空间复杂度：O(n)
// 合并部分
void merge(int arr[], int start, int mid, int end){
    int l = start, r = mid + 1; // 双指针法
    vector<int> temp; // 临时数组
    while(l <= mid && r <= end){
        // 比较子数组首，小的移入新数组
        if(arr[l] <= arr[r]){
            temp.push_back(arr[l]);
            l++;
        }
        else{
            temp.push_back(arr[r]);
            r++;
        }
    }
    // 剩余部分移入新数组
    while(l <= mid){
        temp.push_back(arr[l]);
        l++;
    }
    while(r <= end){
        temp.push_back(arr[r]);
        r++;
    }
    // copy
    for(int i = 0;i <= end - start;i++){
        arr[start + i] = temp[i];
    }
}
// 递归部分
void mergeSort(int arr[], int start, int end){
    // 递归终点
    if(start == end) return;
    // 递归排序左右两子数组
    int mid = (start + end) / 2;
    mergeSort(arr, start, mid);
    mergeSort(arr, mid + 1, end);
    // 合并左右子数组
    merge(arr, start, mid, end);
}



int main(){
    int arr[] = {9,3,6,1,2,6,9,10};
    radixSort(arr, 8);
    for(int a:arr) cout << a << ' ';
}