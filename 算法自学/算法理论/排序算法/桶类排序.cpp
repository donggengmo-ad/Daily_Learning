#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

/*
?   方法：
?       不比较元素之间的大小，而是靠值域下标定位
?   特点：
?       复杂度与值域正相关，时间复杂度多为O(n+k)（较低），空间复杂度较高
?   适用场景：
?       数据规模较大，值域较小，数据分布均匀（桶内元素较少）
*/

//# 桶排序
//$ 时间复杂度：O(n+k)，空间复杂度：O(n+k)，k-桶数量，稳定
//? 将元素分布到k个桶中，要求整体递增，对各桶内部排序，再按顺序放回原数组
void bucketSort(int arr[], int n, int k){
    // 创建k个桶
    vector<vector<int>> bucket(k);
    // 找最值，算桶内元素数
    int max = arr[0], min = arr[0];
    for(int i = 0;i < n;i++){
        if(max < arr[i]) max = arr[i];
        if(min > arr[i]) min = arr[i]; 
    }
    // +k向上取整
    int per = (max - min + k) / k;
    // 数组扔进各个桶，要求整体递增（第n+1个桶中任意数 比 第n个桶中任意数 大）
    for(int i = 0;i < n;i++){
        // 区间等比
        int idx = (arr[i] - min) / per;
        bucket[idx].push_back(arr[i]);
    }
    // 各桶内部排序
    for(int i = 0;i < k;i++) sort(bucket[i].begin(), bucket[i].end());
    // 按顺序放回原数组
    int idx = 0;
    for(int i = 0;i < k;i++){
        for(int b:bucket[i]) arr[idx++] = b;
    }
}

//# 计数排序（每个桶只放相同数的 桶排序）
//$ 时间复杂度：O(n+k)，空间复杂度：O(k)，k-数值范围，稳定
//? 创建一个计数器，下标代表数值，统计每个数出现的次数，再按顺序放回原数组
void countingSort(int arr[], int n){
    // 创建计数器
    int min = arr[0], max = arr[0];
    for(int i = 0;i < n;i++){
        if(max < arr[i]) max = arr[i];
        if(min > arr[i]) min = arr[i]; 
    }
    vector<int> counter(max - min + 1, 0);
    // 对元素计数
    for(int i = 0; i < n; i++) counter[arr[i] - min]++;// 统一平移下标，避免负数索引
    // 将桶中的元素放回原数组
    int idx = 0;
    for(int i = 0; i <= max - min; i++){
        // 从前往后，一个数值一次性放完
        for(int j = 0; j < counter[i]; j++) arr[idx++] = i + min;// 反向平移下标
    }
}

//# 基数排序（进制数个桶的 桶排序 做位数次）
//$ 时间复杂度：O(d*(n+k))，空间复杂度：O(n+k)，d-位数，k-数值范围，稳定
//? 按位数依次排序，从低位到高位，每次对当前位数使用计数排序
void radixSort(int arr[], int n){
    // 找到最大值
    int max = arr[0];
    for(int i = 0;i < n;i++) max = (max > arr[i]) ? max : arr[i];
    // 尝试比最大值小的基数
    int base = 1;
    while(base <= max){
        // 创建10个桶
        vector<vector<int>> bucket(10);
        // 根据当前位数值放入桶
        for(int i = 0;i < n;i++){
            int radix = (arr[i] / base) % 10;
            bucket[radix].push_back(arr[i]);
        }
        // 按顺序放回原数组
        int idx = 0;
        for(int i = 0;i < 10;i++){
            for(int b:bucket[i]) arr[idx++] = b;
        }
        // 尝试更高位
        base *= 10;
    }
}

int main(){
    int arr[] = {9,3,6,1,2,6,9,10};
    
    radixSort(arr, 8);
    for(int a:arr) cout << a << ' ';

    return 0;
}