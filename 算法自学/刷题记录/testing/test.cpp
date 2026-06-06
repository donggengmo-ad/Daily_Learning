#include <bits/stdc++.h>
using namespace std;
struct DSULH{
    struct LH{
        struct Node{
            int l = 0, r = 0, d = 0, v = 0, fa = 0;
            bool del = false;
            Node(){}
            Node(int v): v(v){}
        };
        vector<Node> h;
        LH(){}
        LH(const vector<int> &a): h(a.size()){
            h[0].d = -1;
            for(int i = 1;i < a.size();i++) 
                h[i] = Node(a[i]);
        }
        int merge(int x, int y){
            if(!x || !y) return x | y; 
            if(h[x].v > h[y].v) swap(x, y);  
            h[x].r = merge(h[x].r, y);
            if(h[x].r) h[h[x].r].fa = x;
            if(h[h[x].l].d < h[h[x].r].d) swap(h[x].l, h[x].r);
            h[x].d = h[h[x].r].d + 1;
            return x;
        }
        void fix(int x){
            int &l = h[x].l, &r = h[x].r;
            if(h[l].d < h[r].d) swap(l, r);
            if(x && h[x].d != h[r].d + 1) 
                h[x].d = h[r].d + 1, fix(h[x].fa);
        }
        int del(int x){
            int fa = h[x].fa;
            int root = merge(h[x].l, h[x].r);
            if(root) h[root].fa = fa;
            if(fa && x == h[fa].l) h[fa].l = root;
            else if(fa && x == h[fa].r) h[fa].r = root;
            fix(h[root].fa);
            return root;
        }
        int pop(int x){
            x = merge(h[x].l, h[x].r);
            h[x].fa = 0;
            return x;
        }
        int get(int x){
            return h[x].v;
        }
        void set(int x, int k){
            h[x] = Node(k);
        }
        bool top(int x){
            return !h[x].fa;
        }
    };
    struct DSU{
        vector<int> p, h;
        DSU(){}
        DSU(int n): p(n + 1, 0), h(n + 1, 0){
            for(int i = 1;i <= n;i++) p[i] = i;
        }
        int find(int x){
            if(p[x] != x) p[x] = find(p[x]);
            return p[x];
        }
        void unite(int x, int y){
            int rx = find(x), ry = find(y);
            if(rx != ry){
                if(h[rx] > h[ry]) p[ry] = rx;
                else p[rx] = ry, h[ry] += h[ry] == h[rx];
            }
        }
    };
    DSU s;
    LH h;
    vector<int> s2h;
    DSULH(vector<int> &a): h(a), s(a.size()), s2h(a.size(), 0){
        for(int i = 0;i < a.size();i++) s2h[i] = i;
    }
    void merge(int x, int y){
        x = s.find(x), y = s.find(y);
        s.unite(x, y);
        s2h[s.find(x)] = h.merge(s2h[x], s2h[y]);
    }
    void pop(int x){
        s2h[s.find(x)] = h.pop(x);
    }
    int top(int x){
        return h.get(s2h[s.find(x)]);
    }
    void modify(int x, int k){
        int &root = s2h[s.find(x)];
        if(h.top(x)) root = h.del(x);
        else h.del(x);
        h.set(x, k);
        root = h.merge(x, root);
    }
    void del(int x){
        if(h.top(x)) s2h[s.find(x)] = h.del(x);
        else h.del(x);
    }
};
int main(){
    int n, m;
    cin >> n >> m;
    vector<int> a(n + 1, 0);
    for(int i = 1;i <= n;i++) cin >> a[i];
    DSULH s(a);
    int op, x, y, z;
    while(m--){
        cin >> op;
        if(op == 0){
            cin >> x >> y;
            s.del(y);
        }
        else if(op == 1){
            cin >> x;
            cout << s.top(x) << '\n';
        }
        else if(op == 2){
            cin >> x >> y;
            s.merge(x, y);
        }
        else if(op == 3){
            cin >> x >> y >> z;
            s.modify(y, z);
        }
    }
    return 0;
}
