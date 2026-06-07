#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
using namespace std;
struct AC{
    struct Node{
        int next[26] = {0}; // 小写字母
        int fail = 0, last = 0, cnt = 0;
    };
    vector<Node> t;
    deque<int> order;
    unordered_map<int, string> dict;
    AC(): t(1){}
    // 插入字符串
    void insert(string &s){
        int p = 0;
        for(char c: s){
            if(!t[p].next[c-'a']){
                t[p].next[c-'a'] = t.size();
                t.emplace_back();
            }
            p = t[p].next[c-'a'];
        }
        t[p].cnt++;
        dict[p] = s;
    }
    // 构建 fail 指针（全部插入完成后）
    void build(){
        queue<int> q;
        for(int u: t[0].next) if(u) q.emplace(u);
        while(!q.empty()){
            int u = q.front();
            order.emplace_front(u);
            q.pop();
            for(int c = 0;c < 26;c++){
                int v = t[u].next[c];
                int fp = t[u].fail;
                if(v){
                    t[v].fail = t[fp].next[c];
                    int fv = t[v].fail;
                    if(t[fv].cnt) t[v].last = fv;
                    else t[v].last = t[fv].last;
                    q.emplace(v);
                }
                else t[u].next[c] = t[fp].next[c];
            }
        }
    }
    // 查询所有模式串匹配次数（last 链）
    int countAll(string &s){
        int p = 0, res = 0;
        for(char c: s){
            p = t[p].next[c-'a'];
            for(int u = p;u;u = t[u].last) 
            res += t[u].cnt; 
        }
        return res;
    }
    // 查询每个模式串匹配次数（fail 树）
    // res 下标是节点号
    void countEach(string &s, vector<int> &res){
        res.resize(t.size(), 0);
        int p = 0;
        for(char c: s){
            p = t[p].next[c-'a'];
            res[p]++;
        }
        for(int i: order) res[t[i].fail] += res[i];
    }
};
