#include <iostream>
#include <vector>
using namespace std;

//# 二分法，适用于单调函数，O(logn)
// 常见目标：找第一个或者最后一个满足条件的，即左右边界
/* 分类：
    - 二分查找：已有有序数组，在数组里找满足条件的数
    - 二分答案：没有数组，在答案可能范围内二分，找满足条件的数
*/

// 定义一个单调函数（这就属于二分答案了）
int f(int x){
    if(-10 <= x && x <= 10) return 0;
    else if(x < -10) return x;
    else return 2 * x;
}

// 第一个 > x 的数
int first_greater(int x){
    int l = -100, r = 100, mid;
    while(l < r){
        //* 向下取整
        // 用区间长防止溢出
        mid = l + (r - l) / 2;
        // 满足条件，继续往左找，包括mid
        // 这里改成 >= 就是第一个 >= x 的数了
        if(f(mid) > x) r = mid; 
        // 不满足条件，往右找，不包括mid
        else l = mid + 1; 
    }
    // 循环结束时l==r
    return l;
}

// 最后一个 < x 的数
int last_less(int x){
    int l = -100, r = 100, mid;
    while(l < r){
        //* 向上取整
        mid = l + (r - l + 1) / 2;
        // 满足条件，继续往右找，包括mid
        if(f(mid) < x) l = mid; 
        // 不满足条件，往左找，不包括mid
        else r = mid - 1; 
    }
    // 循环结束时l==r
    return l;
}

/* 边界控制：
    - 满足条件 -> 包括mid
    - 不满足条件 -> 不包括mid

    - 找左边界 -> r包括mid -> 向下取整
    - 找右边界 -> l包括mid -> 向上取整
*/
/* 取整方式：
    - 向上取整：(a + b - 1) / b
    - 向下取整：a / b
*/

int main(){
    int x;
    cin >> x;
    cout << first_greater(x) << endl;
    cout << last_less(x) << endl;
    return 0;
}