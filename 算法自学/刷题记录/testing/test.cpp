#include <iostream>
#include <vector>
#include <string>
using namespace std;
struct HT{
    int len = 0, base = 31, cnt = 0;
    vector<int> val;
    int H(const string &s){
        int res = 0;
        for(char c:s) res = res * base + c;
        return res % len;
    }
    HT(){}
    HT(int l): len(l), val(l, 0){}
    int& operator[](const string &s){
        int i = H(s);
        if(!val[i]) cnt++;
        return val[i];
    }
};
int main(){
    int n;
    cin >> n;
    HT tb(n);
    while(n--){
        string s;
        cin >> s;
        tb[s]++;
    }
    cout << tb.cnt;
    return 0;
}