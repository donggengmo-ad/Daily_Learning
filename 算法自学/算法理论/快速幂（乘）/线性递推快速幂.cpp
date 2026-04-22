#include <iostream>
#include <vector>
using namespace std;
using ll = long long;
ll mod = (ll)1e9 + 7;

//* 转移矩阵：将线性递推写成线性方程组，用矩阵乘法表示
//* 状态向量：记录某一时刻状态的所有必要信息的向量
//% 构造目标： 新状态向量 = 转移矩阵 * 旧状态向量

//@ 洛谷P1939
// 题目：a[i] = a[i-1] + a[i-3] (i <= 3时a[i] = 1)
/*
 |a[i]  |   |1 0 1|   |a[i-1]|
 |a[i-1]| = |1 0 0| * |a[i-2]|
 |a[i-2]|   |0 1 0|   |a[i-3]|
*/
/*
 |a[i]  |   |1 0 1|^(i-3)   |a[3]|
 |a[i-1]| = |1 0 0|    *    |a[2]|
 |a[i-2]|   |0 1 0|         |a[1]|
*/

// 矩阵类模版
struct Matrix{
    int r, c;
    vector<vector<ll>> m;
    Matrix(int r, int c, int init=0): r(r), c(c), m(r, vector<ll>(c ,init)){}
    Matrix(int n): Matrix(n, n){
        for(int i = 0;i < n;i++) m[i][i] = 1;
    }
    Matrix(Matrix& A): r(A.r), c(A.c), m(r, vector<ll>(c)) {
        for(int i = 0;i < r;i++){
            for(int j = 0;j < c;j++) m[i][j] = A.m[i][j];
        }
    }
    Matrix(): Matrix(0){}
    Matrix operator*(const Matrix &b) const{
        Matrix res(r, b.c);
        if(c != b.r) return res;
        for(int i = 0;i < r;i++){
            for(int j = 0;j < b.c;j++){
                for(int k = 0;k < c;k++) {
                    res.m[i][j] += m[i][k] * b.m[k][j];
                    res.m[i][j] %= mod;
                }
            }
        }
        return res;
    }
    Matrix qpow(ll p){
        Matrix res(r);
        if(r != c) return res;
        Matrix a(*this);
        while(p > 0){
            if(p & 1) res = res * a;
            a = a * a;
            p >>= 1;
        }
        return res;
    }
};

// 转移方程
ll getK(ll k){
    if(k <= 3) return 1;
    // 构造转移矩阵和初始状态向量
    Matrix trans(3, 3), orig(3, 1, 1), res;
    trans.m[0][0] = 1;trans.m[0][1] = 0;trans.m[0][2] = 1;
    trans.m[1][0] = 1;trans.m[1][1] = 0;trans.m[1][2] = 0;
    trans.m[2][0] = 0;trans.m[2][1] = 1;trans.m[2][2] = 0;
    // 快速幂求解
    res = trans.qpow(k-3) * orig;
    // res = |a[k] a[k-1] a[k-2]｜^T
    // 取第一行第一列即为a[k]
    return res.m[0][0] % mod;
}

int main(){
    ll T;
    cin >> T;
    while(T--){
        ll k;
        cin >> k;
        cout << getK(k) << endl;
    }
    return 0;
}