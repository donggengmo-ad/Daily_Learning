#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

// 前缀字典树
struct Trie{
    struct Node{
        bool in = false;
        int pass = 0;
        unordered_map<char, int> next;
    };
    vector<Node> t;
    Trie(): t(1){}
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
        t[p].in = true;
    }
    // 存在性查找
    bool find(const string &s){
        int p = 0;
        for(char c: s){
            if(!t[p].next.count(c) || !t[p].pass) return false;
            p = t[p].next[c];
        }
        return t[p].in;
    }
    void del(const string &s){
        int p = 0;
        for(char c: s){
            if(!t[p].next.count(c) || !t[p].pass) return;
            p = t[p].next[c];
            t[p].pass--;
        }
        t[p].in = false;
    }
};

// 01字典树
struct Trie01{
    struct Node{
        int cnt = 0, pass = 0;
        int next[2] = {0};
    };
    int wid = 32;// 位宽
    vector<Node> t;
    Trie01(int w = 32): wid(w), t(1){}
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
    // 统计出现次数
    int find(int x){
        int p = 0;
        for(int i = wid - 1; i >= 0; i--){
            int b = (x >> i) & 1;
            if(!t[p].next[b] || !t[p].pass) return 0;
            p = t[p].next[b];
        }
        return t[p].cnt;
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
    // 最大异或数
    int maxXor(int x){
        //TODO
    }
    // 更大/小数统计
    int greater(int x){
        //TODO
    }
    int less(int x){
        //TODO
    }
    // 第k大/小数查找
    int kthLarge(int k){
        //TODO
    }
    int kthSmall(int k){
        //TODO
    }
};


