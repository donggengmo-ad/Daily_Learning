#include <vector>
using namespace std;
using ll = long long;

// 并查集（略）
struct DSU{
    DSU(int n){}
    int find(int x){}
    void unite(int x, int y){}
};

// 基础左偏堆（小顶堆）
struct LeftHeap{
    struct Node{
        ll v = 0, d = 0;
        int l = 0, r = 0, fa = 0;
        Node(){}
        Node(ll v): v(v){}
    };
    vector<Node> h;
    LeftHeap(){}
    // 1-based 数组 a
    LeftHeap(vector<ll> &a): h(a.size()){
        h[0].d = -1;
        for(int i = 1; i < a.size(); i++) h[i].v = a[i];
    }
    // 合并两个堆，必须保证 x、y 都是堆根
    int merge(int x, int y){
        if(!x || !y) return x | y; 
        if(h[x].v > h[y].v) swap(x, y);
        h[x].r = merge(h[x].r, y);
        if(h[x].r) h[h[x].r].fa = x; 
        if(h[h[x].l].d < h[h[x].r].d) swap(h[x].l, h[x].r);
        h[x].d = h[h[x].r].d + 1;
        h[x].fa = 0;
        return x;
    }
    // 修复左偏性
    void fix(int x){
        if(!x) return; 
        int &l = h[x].l, &r = h[x].r;
        if(h[l].d < h[r].d) swap(l, r);
        if(x && h[x].d != h[r].d + 1)
           h[x].d = h[h[x].r].d + 1, 
           fix(h[x].fa); 
    }
    // 移除节点 x，返回合并后节点（可能是根）
    int remove(int x){
        int fa = h[x].fa, root = merge(h[x].l, h[x].r);
        if(root) h[root].fa = fa; 
        if(fa && h[fa].l == x) h[fa].l = root;
        else if(fa && h[fa].r == x) h[fa].r = root;
        fix(fa);
        return root;
    }
    bool istop(int x){return !h[x].fa;} // 是否是堆根
    void set(int x, ll k){h[x] = Node(k);} // 重置节点
    ll get(int x){return h[x].v;} // 获取节点值
};

// 完整左偏堆
struct Heap{
    DSU s;
    LeftHeap h;
    vector<int> s2h;
    Heap(vector<ll> &a): s(a.size()), h(a), s2h(a.size()){
        for(int i = 1; i < a.size(); i++) s2h[i] = i;
    }
    // 合并x、y所属的堆
    void merge(int x, int y){
        x = s.find(x), y = s.find(y);
        if(x == y) return;
        s.unite(x, y);
        s2h[s.find(x)] = h.merge(s2h[x], s2h[y]);
    }
    // 移除节点 x
    void remove(int x){
        if(h.istop(x)) s2h[s.find(x)] = h.remove(x);
        else h.remove(x);
    }
    // 修改节点 x 的值
    void modify(int x, ll k){
        remove(x);
        h.set(x, k);
        int &root = s2h[s.find(x)];
        root = h.merge(x, root);
    }
    // 弹出 x 所属堆的堆顶
    void pop(int x){
        remove(s2h[s.find(x)]);
    }
    // 获取 x 所属堆的堆顶
    ll top(int x){
        return h.get(s2h[s.find(x)]);
    }
};

