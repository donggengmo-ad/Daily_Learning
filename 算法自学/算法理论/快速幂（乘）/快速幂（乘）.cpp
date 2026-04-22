#include <iostream>
using namespace std;
using ll = long long;

//# 二进制思想
//% n = sum(2^i)
//% a^n = a^(sum(2^i)) = product(a^(2^i))
// eg. 100 = 2^6 + 2^5 + 2^2
//  => a^100 = a^(2^6 + 2^5 + 2^2) = a^(2^6) * a^(2^5) * a^(2^2)

// 快速幂（求 a^p）
ll qpow(ll a, ll p){
    ll res = 1;
    // 
    while(p > 0){
        // p 的当前二进制位为1说明有 2^i 这一项，要乘上 a^(2^i) 这一项
        if(p & 1) res *= a;
        // 保持 a 为 a^(2^i)，每次循环 a 都要平方一次
        a *= a;
        // 右移，让 p 的下一位成为当前位，继续判断
        p >>= 1;
    }
    return res;
}

// 带取模的快速幂（求 a^p % mod）
// a^p % mod = (a % mod)^p % mod
ll qpow_mod(ll a, ll p, ll mod){
    // 其他操作和上面一样
    ll res = 1;
    while(p > 0){
        // 每次乘法都要取模，防止数值过大溢出
        if(p & 1) res = res * a % mod;
        a = a * a % mod;
        p >>= 1;
    }
    return res;
}

// 快速乘（求 a*b），借助快速幂的思想
//% n = sum(2^i)
//% a * n = a * (sum(2^i)) = sum(a * 2^i)
ll qmul(ll a, ll b){
    ll res = 0;
    // b 的当前二进制位为1说明有 2^i 这一项，要加上 a * 2^i 这一项
    while(b > 0){
        if(b & 1) res += a;
        // 保持 a 为 a * 2^i，每次循环 a 都要翻倍一次
        a *= 2;
        // 右移，让 b 的下一位成为当前位，继续判断
        b >>= 1;
    }
    return res;
}

// 带取模的快速乘（求 a*b % mod）
// a*b % mod = (a % mod) * (b % mod) % mod
ll qmul_mod(ll a, ll b, ll mod){
    // 其他操作和上面一样
    ll res = 0;
    while(b > 0){
        // 每次加法都要取模，防止数值过大溢出
        if(b & 1) res = (res + a) % mod;
        a = (a * 2) % mod;
        b >>= 1;
    }
    return res;
}

int main(){
    ll a, p;
    cin >> a >> p;
    cout << qmul(a, p) << endl;
    return 0;
}