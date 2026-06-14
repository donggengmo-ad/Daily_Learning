#include <bits/stdc++.h>
using namespace std;
using ll = long long;
struct BA{
    struct B{
        int l = 0, r = 0;
        ll add = 0;
        vector<ll> inc;
        B(){}
        B(int i, int len): 
            l((i - 1) * len + 1), r(i * len), inc(len, 0){
                cout << inc.size() << endl;
            }
        void adjust(int n){
            r = n;
            inc = vector<ll>(r - l + 1, 0);
        }
        void sort(vector<ll> &org){
            for(int i = l; i <= r; i++) {
                cout << inc.size() << endl;
                cout << format("org[{}] to inc[{}]", i, i - l) << endl;
                inc[i-l] = org[i];
            }
            std::sort(inc.begin(), inc.end());
        }
        int count(ll k){
            return inc.end() 
                - lower_bound(inc.begin(), inc.end(), k);
        }
    };
    vector<B> b;
    vector<ll> org;
    vector<int> in;
    int len = 0, num = 0;
    BA(int n, vector<ll> &a): org(a), in(n + 1, 0){
        len = sqrt(n * 1.0);
        num = n / len + (n % len != 0);
        b.resize(num + 1);
        for(int i = 1; i <= num; i++) b[i] = B(i, len);
        b[num].adjust(n);
        for(int i = 1; i <= num; i++){
            for(int j = b[i].l; j <= b[i].r; j++) in[j] = i;
            b[i].sort(org);
        }
    }
    void add(int l, int r, ll k){
        int p = in[l], q = in[r];
        if(p == q){
            for(int i = l; i <= r; i++) org[i] += k;
            b[p].sort(org);
            return;
        }
        for(int i = p + 1; i < q; i++) b[i].add += k;
        for(int i = l; i <= b[p].r; i++) org[i] += k;
        for(int i = b[q].l; i <= r; i++) org[i] += k;
        b[p].sort(org), b[q].sort(org);
    }
    int query(int l, int r, ll k){
        int p = in[l], q = in[r];
        ll res = 0;
        if(p == q){
            for(int i = l; i <= r; i++) 
                res += org[i] + b[p].add >= k;
            return res;
        }
        for(int i = p + 1; i < q; i++) res += b[i].count(k);
        for(int i = l; i <= b[p].r; i++)
            res += org[i] + b[p].add >= k;
        for(int i = b[q].l; i <= r; i++) 
            res += org[i] + b[q].add >= k;
        return res;
    }
};
int main(){
    int n, q;
    cin >> n >> q;
    vector<ll> a(n + 1, 0);
    for(int i = 1;i <= n;i++) cin >> a[i];
    BA b(n, a);
    while(q--){
        char op; int l, r; ll k;
        cin >> op >> l >> r >> k;
        if(op == 'M') b.add(l, r, k);
        if(op == 'A') cout << b.query(l, r, k) << '\n';
    }
    return 0;
}