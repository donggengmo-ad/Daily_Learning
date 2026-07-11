#include <vector>
#include <algorithm>
using namespace std;
using ll = long long;

// 查询区间和为例
struct Mo{
    struct Query{int id = 0, l = 0, r = 0;};
    struct Block{int l = 0, r = 0;};
    vector<Query> qry;
    vector<Block> b;
    vector<ll> org, ans;
    vector<int> in;
    int len = 0, num = 0;
    ll sum = 0;
    Mo(int n, int m): org(n + 1), qry(m), ans(m, 0), in(n + 1, 0){
        len = sqrt(n * 1.0);
        num = n / len + (n % len != 0);
        b.resize(num + 1);
        for(int i = 1;i <= num;i++){
            b[i].l = (i - 1) * len + 1;
            b[i].r = min(i * len, n);
            for(int j = b[i].l;j <= b[i].r;j++) in[j] = i;
        }
    }
    bool cmp(Query &a, Query &b){
        if(in[a.l] != in[b.l]) return in[a.l] < in[b.l];
        return (in[a.l] & 1) ? a.r < b.r : a.r > b.r;
    }
    void include(int x){sum += org[x];}
    void exclude(int x){sum -= org[x];}
    void solve(){
        sort(qry.begin(), qry.end(), [&](Query &a, Query &b){return cmp(a, b);});
        int l = 1, r = 0;
        for(auto &q : qry){
            while(l > q.l) include(--l);
            while(r < q.r) include(++r);
            while(l < q.l) exclude(l++);
            while(r > q.r) exclude(r--);
            ans[q.id] = sum;
        }
    }
};


