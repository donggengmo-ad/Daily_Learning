#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

//# 标准库二分
/* 
? 标准库的algorithm中提供了两个二分函数
/    1.lower_bound：第一个 >= target的元素位置
/    2.upper_bound：第一个 > target的元素位置
*/

//$ 这两个函数的时间复杂度都是O(logn)，适用于有序数组

/*
? 可以用于其他任务
/    1.lower_bound - 1：最后一个 < target的元素位置
/    2.upper_bound - 1：最后一个 <= target的元素位置
示意：
@ < < < < < = = = = = > > > > > > 
*         ^ ^       ^ ^
*         | |       | |
%      lb-1 lb   ub-1 ub
*/

int main(){
    int n, x;
    cin >> n >> x;
    vector<int> a(n);
    for(int i = 0;i < n;i++) cin >> a[i];
    sort(a.begin(), a.end());
    auto lb = lower_bound(a.begin(), a.end(), x);
    auto ub = upper_bound(a.begin(), a.end(), x);
    cout << "last  <  x: " << (lb - a.begin() - 1) << endl;
    cout << "first >= x: " << (lb - a.begin()) << endl;
    cout << "last  <= x: " << (ub - a.begin() - 1) << endl;
    cout << "first >  x: " << (ub - a.begin()) << endl;
    return 0;
}