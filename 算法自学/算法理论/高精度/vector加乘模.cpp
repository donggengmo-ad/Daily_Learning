#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;
using ll = long long;

// 采用vector<int>来存储大数，每个元素存储一位数字，比string更方便运算
// 按位反着存，最低位在头，最高位在尾
// 相当于 sum(a[i] * 10^i)
// eg. 1234 -> {4, 3, 2, 1}

// 输入输出
void input(vector<int> &a){
    string s;
    cin >> s;
    // 从字符串末尾开始读
    for(int i = s.size() - 1;i >= 0;i--) 
        a.push_back(s[i] - '0');
}
void output(vector<int> a){
    // 从vector末尾开始输出
    for(int i = a.size() - 1;i >= 0;i--) cout << a[i];
    cout << endl;
}

// 高精度加法
vector<int> add(vector<int> a, vector<int> b){
    // 取较大的长度
    int len = max(a.size(), b.size());
    vector<int> res;
    for(int i = 0;i < len;i++){
        // a或b长度不足时视为0
        if(i >= a.size()) res.push_back(b[i]);
        else if(i >= b.size()) res.push_back(a[i]);
        // 否则正常相加
        else res.push_back(a[i] + b[i]);
    }
    for(int i = 0;i < res.size();i++){
        // >=10时需要进位
        if(res[i] >= 10){
            // 当前数取个位
            res[i] %= 10;
            // 如果不是最后一位，直接在下一位加1
            if(i < res.size() - 1) res[i+1]++;
            // 否则在末尾添加一位1
            else res.push_back(1);
        }
    }
    return res;
}

// 高精度乘法
vector<int> mul(vector<int> a, vector<int> b){
    // 结果长度最多为两个数长度之和（10^n * 10^m = 10^(n+m)）
    int len = a.size() + b.size();
    vector<int> res(len, 0);
    // 遍历两数的每一位
    for(int i = 0;i < a.size();i++){
        for(int j = 0;j < b.size();j++){
            // 当前位乘积加到结果对应位（10^i * 10^j = 10^(i+j)）
            res[i+j] += a[i] * b[j];
            // 进位，最多进1位（9 * 9 = 81）
            // i + j <= len - 2，不会越界
            res[i+j+1] += res[i+j] / 10;
            res[i+j] %= 10;
        }
    }
    // 去除高位多余的0
    while(!res.empty() && res.back() == 0) res.pop_back();
    return res;
}

// 高精度取模
// (a × 10 + b) % m = [(a % m) × 10 + b] % m
// 也就是说整体取模之前，先对因子取模没有影响
// res = (res * 10 + a[i]) 循环得到res是a存储的值
// 因为最高位被*10n次，越低位被*10的次数越少
// eg. {1,2,3,4} -> 0 * 10 + 4 -> 4 * 10 + 3 -> 43 * 10 + 2 -> 432 * 10 + 1 -> 4321
// 每次都对modn取模，对结果无影响，还能防止溢出
// res = (res * 10 + a[i]) % modn 循环得到res是 a存储的值 % modn
ll mod(vector<int> a, ll modn){
    ll res = 0;
    for(int i = a.size() - 1;i >= 0;i--){
        // 从高位开始，每次乘10加当前位取模
        res = (res * 10 + a[i]) % modn;
    }
    return res;
}

int main(){
    vector<int> a, b, c;
    input(a);
    ll modn;
    cin >> modn;
    cout << mod(a, modn);
    return 0;
}