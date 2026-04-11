#include <iostream>
using namespace std;

//# 归并排序
//$ 时间复杂度：O(nlogn)，空间复杂度：O(n)，稳定
//? 每次把数组分成两半，递归直到每个子数组只有一个元素，再按顺序合并
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
    if(start >= end) return;
    // 递归排序左右两子数组
    int mid = (start + end) / 2;
    mergeSort(arr, start, mid);
    mergeSort(arr, mid + 1, end);
    // 合并左右子数组
    merge(arr, start, mid, end);
}

//# 快速排序
//$ 时间复杂度：O(nlogn)，空间复杂度：O(logn)，不稳定
//? 每次选一个基准数，将数组划分成两部分，递归排序两部分
// 根据基准数划分数组
int splitPivot(int arr[], int start, int end){
    // 随机选取基准数并放到数组首
    int rand_idx = start + rand() % (end - start + 1);
    swap(arr[start], arr[rand_idx]);
    int pivot = start;
    // l为比pivot大的左边界
    int l = start + 1;
    for(int i = start + 1;i < end + 1;i++){
        // l以前的元素都比pivot小，l以后的元素都比pivot大
        if(arr[i] <= arr[pivot]){
            swap(arr[i], arr[l]);
            l++;
        }
    } 
    // 将pivot放到l的左边界
    swap(arr[pivot], arr[l - 1]);
    pivot = l - 1;
    return pivot;
}
// 递归部分
void quickSort(int arr[], int start, int end){
    // 递归终点
    if(start >= end) return;
    // 按基准数划分数组并找到基准数
    int pivot = splitPivot(arr, start, end);
    // 递归处理基准数左右的子数组
    quickSort(arr, start, pivot - 1);
    quickSort(arr, pivot + 1, end);
}

int main(){
    int arr[] = {9,3,6,1,2,6,9,10};

    quickSort(arr, 0, 7);
    for(int a:arr) cout << a << ' ';

    return 0;
}