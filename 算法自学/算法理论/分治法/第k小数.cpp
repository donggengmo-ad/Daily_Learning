#include <iostream>
#include <vector>
#include <time.h>
#include <algorithm>
using namespace std;
using ll = long long;

//@ 洛谷P1923

// 分开数组，基准左边的数都比它小，右边都比它大
int partition(vector<ll> &a, int start, int end, int pivot){
    ll a_p = a[pivot];
    // 先把基准放到最左边
    swap(a[pivot], a[start]);
    // 分界点，左小右大，界本身是大值
    int sep = start + 1;
    for(int i = start;i <= end;i++){
        // 如果当前数比基准小
        if(a[i] < a_p){
            // 把小数换到分界点，界点本身是大数所以正好放后面
            swap(a[i], a[sep]);
            // 分界点右移，保证界点本身仍然是大数
            sep++;
        }
    }
    // 基准换回分界点前，界点前一个正好是最后一个小数
    swap(a[start], a[sep-1]); // sep从start开始++，sep-1不会越界
    // 返回基准位置
    return sep - 1;
}

// 返回第k小数的位置
int getak(vector<ll> &a, int l, int r, int k){
    int pivot = (l + r) / 2;
    // 以pivot为基准分开数组，得到基准位置，即基准前有几个数比他小，即基准数为第几小
    int pos = partition(a, l, r, pivot);
    // 正好
    if(pos == k) return pos;
    // 基准偏大，说明第k小在左边，从左边找
    else if(pos > k) return getak(a, l, pos - 1, k);
    // 基准偏小，从右边找
    else return getak(a, pos + 1, r, k);
}

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    srand((int)time(NULL));
    int n, k;
    cin >> n >> k;
    k++; // 题目规定最小是第0小，但我们习惯第1小，所以k++一下

    vector<ll> a(n+2);
    for(int i = 1;i <= n;i++) cin >> a[i];

    // 直接排序，O(nlogn)
    // sort(a.begin(), a.end());
    // int ans = k;

    // 分治法，理想O(n)，最坏O(n^2)
    int ans = getak(a, 1, n, k);

    cout << a[ans] << endl;
    
}
