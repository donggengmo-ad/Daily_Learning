#include <iostream>
#include <vector>
#include <list>
using namespace std;
vector<int> p, sz;
int find(int x){
    if(p[x] != x) p[x] = find(p[x]);
    return p[x];
}
void unite(int x, int y, int &cnt){
    int rx = find(x), ry = find(y);
    if(rx != ry){
        cnt--;
        if(sz[rx] > sz[ry]) 
            sz[rx] += sz[ry], p[ry] = rx;
        else sz[ry] += sz[rx], p[rx] = ry;
    }
}
int main(){
    int n, m;
    cin >> n >> m;
    p.resize(n + 1);
    sz.resize(n + 1, 1);
    for(int i = 1;i <= n;i++) p[i] = i;
    vector<list<int>> link(n + 1);
    for(int i = 0;i < m;i++){
        int x, y;
        cin >> x >> y;
        link[x].emplace_back(y);
        link[y].emplace_back(x);
    }
    int k;
    cin >> k;
    vector<int> atk(k);
    vector<bool> exi(n + 1, 1);
    int cnt = n;
    for(int i = 0;i < k;i++){
        cin >> atk[i];
        exi[atk[i]] = 0;
        cnt--;
    }
    for(int i = 1;i <= n;i++){
        if(!exi[i]) continue;
        for(int j:link[i]) 
            if(exi[j]) unite(i, j, cnt);
    }
    vector<int> ans(k + 1);
    ans[k] = cnt;
    for(int i = k - 1;i >= 0;i--){
        int u = atk[i];
        exi[u] = 1;
        cnt++;
        for(int j:link[u]) if(exi[j]) unite(u, j, cnt);
        ans[i] = cnt;
    }
    for(int i:ans) cout << i << endl;
    return 0;
}