#include <bits/stdc++.h>
using namespace std;
struct Trie{
    struct Node{
        int cnt = 0;
        unordered_map<char, int> next;
    };
    vector<Node> t;
    Trie(): t(1){}
    void insert(const string &s){
        int p = 0;
        for(char c: s){
            if(!t[p].next.count(c)){
                t[p].next[c] = t.size();
                t.emplace_back();
            }
            p = t[p].next[c];
            t[p].cnt++;
        }
    }
    int find(const string &s){
        int p = 0, cnt = 0;
        for(char c: s){
            if(!t[p].next.count(c)) return cnt;
            p = t[p].next[c];
            cnt = t[p].cnt;
        }
        return cnt;
    }
};
void solve(){
    int n, q;
    cin >> n >> q;
    Trie t;
    while(n--){
        string s;
        cin >> s;
        t.insert(s);
    }
    while(q--){
        string s;
        cin >> s;
        cout << t.find(s) << endl;
    }
}
int main(){
    int t;
    cin >> t;
    while(t--) solve();
    return 0;
}