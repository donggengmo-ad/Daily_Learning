#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

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

int main(){
    return 0;
}