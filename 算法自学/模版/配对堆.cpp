#include <vector>
using namespace std;
using ll = long long;
struct DSU{
    DSU(){}
    DSU(int n){}
    int find(int x){}
    void unite(int x, int y){}
};
struct PairHeap{
    struct Node{
        ll val = 0;
        int fa = 0, ch = 0;
        int bro = 0, prev = 0;
        Node(){}
        Node(ll v): val(v){}
    };
    vector<Node> h;
    PairHeap(){}
    // 1-based 数组 a
    PairHeap(vector<ll> &a): h(a.size()){
        for(int i = 1; i < a.size(); i++) h[i].val = a[i];
    }
    int merge(int x, int y){
        if(!x || !y) return x | y;
        if(h[x].val > h[y].val) swap(x, y);
        h[y].bro = h[x].ch;
        h[h[x].ch].prev = y;
        h[x].ch = y;
        h[y].fa = x;
        return x;
    }
    int merge_pair(int a){
        int b = h[a].bro, c = h[b].bro;
        if(!a || !b) return a;
        h[a].bro = h[b].bro = 0;
        h[a].fa = h[b].fa = 0;
        return merge(merge(a, b), merge_pair(c));
    }
    int pop(int x){
        if(!x) return 0;
        return merge_pair(h[x].ch);
    }
    // 要求 k 比原值小
    int decrease(int root, int x, ll k){
        if(!x) return 0;
        int &fa = h[x].fa, &bro = h[x].bro, &prev = h[x].prev;
        if(prev) h[prev].bro = bro;
        else h[fa].ch = bro;
        if(bro) h[bro].prev = prev; 
        fa = bro = prev = 0;
        h[x].val = k;
        return merge(x, root);
    }
    int remove(int root, int x){
        if(!x) return 0;
        decrease(root, x, LLONG_MIN);
        return pop(x);
    }
    int top(int x){
        return h[x].val;
    }
};
struct Heap{
    PairHeap h;
    DSU s;
    vector<int> s2h;
    // 1-based 数组 a
    Heap(vector<ll> &a): s(a.size()), h(a), s2h(a.size()){
        for(int i = 0; i < a.size(); i++) s2h[i] = i;
    }
    void merge(int x, int y){
        x = s.find(x), y = s.find(y);
        if(x == y) return;
        s.unite(x, y);
        s2h[s.find(x)] = h.merge(s2h[x], s2h[y]);
    }
    void remove(int x){
        int &root = s2h[s.find(x)]; 
        root = h.remove(x, root);
    }
    // 要求 k 比原值小
    void decrease(int x, ll k){
        int &root = s2h[s.find(x)];
        root = h.decrease(root, x, k);
    }
    void pop(int x){
        int &root = s2h[s.find(x)];
        root = h.pop(root);
    }
    ll top(int x){
        return h.top(s2h[s.find(x)]);
    }
};
