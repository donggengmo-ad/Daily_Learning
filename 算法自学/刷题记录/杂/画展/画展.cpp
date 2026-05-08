#include <iostream>
#include <vector>
using namespace std;

//@洛谷P1638

int main(){
    int n, m;
    cin >> n >> m;
    // 画数组（对应作者编号）、画家组数（对应作品数量）
    vector<int> paint(n+1, 0), artist(m+1, 0);
    for(int i = 1;i <= n;i++) cin >> paint[i];
    // 双指针，维护一个区间，使得区间内集齐所有画家，找最小区间
    int l = 1, r = 1, num = 0;
    int ansl = 1, ansr = n, anslen = n;
    // 区间为[l, r)，因为r会被多加一次，也是因此r <= n + 1
    while(r <= n + 1 && l <= r){  
        // 画家不齐，区间又段向右扩展（尝试集齐画家）
        if(num < m){
            if(artist[paint[r]]++ == 0) num++;
            r++;
        }
        // 画家齐了
        else{
            // 顺手记录最短完整区间
            if(anslen > r - l){
                ansl = l;
                ansr = r - 1;
                anslen = r - l;
            }
            // 区间左端向右收缩（尝试缩短区间）
            if(--artist[paint[l]] == 0) num--;
            l++;
        }
    }
    cout << ansl << ' ' << ansr << endl;
    
    return 0;
}