#include <vector>
#include <algorithm>
using namespace std;
using ll = long long;

// 维护最大值为例
struct ST{
    int LOG, len;
    vector<int> log2;
    vector<vector<ll>> t;
    ST(int n): len(n), log2(n + 1, 0){
        for(int i = 2;i <= n;i++) log2[i] = log2[i>>1] + 1;
        LOG = log2[n];
        t = vector<vector<ll>>(n + 1, vector<ll>(LOG + 1, 0));
    }
    // 要求 1-based 数组 a
    void build(vector<int> &a){
        for(int i = 1;i <= len;i++) t[i][0] = a[i];
        for(int k = 1;k <= LOG;k++)
            for(int i = 1;i + (1 << k) - 1 <= len;i++)
                t[i][k] = max(t[i][k-1], t[i+(1<<(k-1))][k-1]);
    }
    ll query(int l, int r){
        int k = log2[r-l+1];
        return max(t[l][k], t[r-(1<<k)+1][k]);
    }
};

