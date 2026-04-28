#include <iostream>
#include <vector>
using namespace std;
using ll = long long;

//# 高精度数的快速幂
// 就是把快速幂的乘换成高精度乘而已

// 输入输出
void input(vector<int> &a){
    string s;
    cin >> s;
    for(int i = s.size() - 1;i >= 0;i--) a.push_back(s[i] - '0');
}
void output(vector<int> a){
    for(int i = a.size() - 1;i >= 0;i--) cout << a[i];
    cout << endl;
}

// 高精乘模版
vector<int> mul(vector<int> a, vector<int> b){
    int len = a.size() + b.size();
    vector<int> res(len, 0);
    for(int i = 0;i < a.size();i++){
        for(int j = 0;j < b.size();j++){
            res[i+j] += a[i] * b[j];
            res[i+j+1] += res[i+j] / 10;
            res[i+j] %= 10;
        }
    }
    while(!res.empty() && res.back() == 0) res.pop_back();
    return res;
}

// 快速幂模版
vector<int> qpow(vector<int> a, int p){
    vector<int> res(1, 1);
    while(p > 0){
        if(p & 1) res = mul(res, a);
        a = mul(a, a);
        p >>= 1;
    }
    return res;
}

int main(){
    vector<int> a;
    input(a);
    int p;
    cin >> p;
    vector<int> res = qpow(a, p);
    output(res);
    return 0;
}