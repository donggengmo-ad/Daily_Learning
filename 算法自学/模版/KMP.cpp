#include <string>
#include <vector>
using namespace std;

struct KMP{
    string p; //模式串
    vector<int> next;
    KMP(const string &p): p(p), next(p.size(), -1){}
    void buildNext(){
        int i = 0, j = -1;
        while(i < p.size()){
            if(j == -1 || p[i] == p[j]) next[++i] = ++j;
            else j = next[j];
        }
    }
    // 匹配第一个
    int matchFirst(const string &s){
        int i = 0, j = 0;
        while(i < s.size() && j < p.size()){
            if(j == -1 || s[i] == p[j]) i++, j++;
            else j = next[j];
        }
        if(j == p.size()) return i - j;
        else return -1;
    }
    // 匹配所有
    vector<int> matchAll(const string &s){
        vector<int> res;
        int i = 0, j = 0;
        while(i < s.size() && j < p.size()){
            if(j == -1 || s[i] == p[j]) {
                i++, j++;
                if(j == p.size()){
                    res.emplace_back(i - j);
                    j = next[j];
                }
            }
            else j = next[j];
        }
        return res;
    }
    // 存在性检验
    bool exist(const string &s){
        return matchFirst(s) != -1;
    }
    // 出现次数
    int count(const string &s){
        vector<int> res = matchAll(s);
        return res.size();
    }
};

int main(){
    return 0;
}