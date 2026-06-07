#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

// 前缀字典树
struct Trie{
    struct Node{
        int pass = 0, cnt = 0;
        unordered_map<char, int> next;
    };
    vector<Node> t;
    Trie(): t(1){t[0].pass = 1;}
    void insert(const string &s){
        int p = 0;
        for(char c: s){
            if(!t[p].next.count(c)) {
                t[p].next[c] = t.size();
                t.emplace_back();
            }
            p = t[p].next[c];
            t[p].pass++;
        }
        t[p].cnt++;
    }
    // 统计出现次数
    bool count(const string &s){
        int p = 0;
        for(char c: s){
            if(!t[p].next.count(c) || !t[p].pass) return false;
            p = t[p].next[c];
        }
        return t[p].cnt;
    }
    void del(const string &s){
        int p = 0;
        for(char c: s){
            if(!t[p].next.count(c) || !t[p].pass) return;
            p = t[p].next[c];
            t[p].pass--;
        }
        t[p].cnt--;
    }
};

// 01字典树
struct Trie01{
    struct Node{
        int cnt = 0, pass = 0;
        int next[2] = {0};
    };
    const static int wid = 32;// 位宽
    vector<Node> t;
    Trie01(): t(1){t[0].pass = 1;}
    void insert(int x){
        int p = 0;
        for(int i = wid - 1; i >= 0; i--){
            int b = (x >> i) & 1;
            if(!t[p].next[b]) {
                t[p].next[b] = t.size();
                t.emplace_back();
            }
            p = t[p].next[b];
            t[p].pass++;
        }
        t[p].cnt++;
    }
    void del(int x){
        int p = 0;
        for(int i = wid - 1; i >= 0; i--){
            int b = (x >> i) & 1;
            if(!t[p].next[b] || !t[p].pass) return;
            p = t[p].next[b];
            t[p].pass--;
        }
        t[p].cnt--;
    }
    // 统计出现次数（不能是空树）
    int count(int x){
        int p = 0;
        for(int i = wid - 1; i >= 0; i--){
            int b = (x >> i) & 1;
            if(!t[p].next[b] || !t[p].pass) return 0;
            p = t[p].next[b];
        }
        return t[p].cnt;
    }
    // 最大异或数（不能是空树）
    int maxXor(int x){
        int p = 0, res = 0;
        for(int i = wid - 1; i >= 0; i--){
            int b = (x >> i) & 1;
            int go = t[p].next[!b];
            if(go && t[go].pass) p = go, res |= (1 << i);
            else p = t[p].next[b];
        }
        return res;
    }
    // 更大数统计（不能是空树）
    int greater(int x){
        int p = 0, res = 0;
        for(int i = wid - 1; i >= 0; i--){
            int go0 = t[p].next[0], go1 = t[p].next[1];
            int bx = (x >> i) & 1;
            if(!go1 || !t[go1].pass){
                if(bx) break;
                p = go0;
                continue;
            }
            if(bx) p = go1;
            else{
                res += t[go1].pass;
                if(go0 && t[go0].pass) p = go0;
                else break;
            }
        }
        return res;  
    }
    // 第k大数查找（不能是空树）
    int kthLarge(int k){
        int p = 0, res = 0;
        for(int i = wid - 1; i >= 0; i--){
            int go0 = t[p].next[0], go1 = t[p].next[1];
            if(go1 && t[go1].pass >= k) p = go1, res |= (1 << i);
            else{
                if(go1) k -= t[go1].pass;
                p = go0;
            }
        }
        return res;        
    }
};


