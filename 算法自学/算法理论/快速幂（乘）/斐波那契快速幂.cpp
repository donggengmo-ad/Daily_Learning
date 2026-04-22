#include <iostream>
#include <vector>
using namespace std;
using ll = long long;
// 矩阵类
struct Matrix{
    // 行数列数和矩阵数组
    int rows, cols;
    vector<vector<ll>> mat;
    // 构造函数
    // r行c列的矩阵，全员初始化(默认0)
    Matrix(int r, int c, ll init=0):
        rows(r), cols(c), mat(r, vector<ll>(c, init)) {}
    // n阶方阵，可为单位矩阵(默认非单位矩阵)
    Matrix(int n, bool identity=false): Matrix(n, n) {
        if(identity){
            for(int i = 0;i < n;i++) mat[i][i] = 1;
        }
    }
    Matrix(): Matrix(2, 2){}
    // 输入输出函数
    void in(){
        for(int i = 0; i < rows; i++)
            for(int j = 0; j < cols; j++) cin >> mat[i][j];
    }
    void out(){
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++) cout << mat[i][j] << " ";
            cout << endl;
        }
    }
    // 矩阵乘法
    Matrix operator*(const Matrix b) const{
        Matrix res(rows, b.cols);
        // 要求前列数等于后行数
        if(cols != b.rows) return res;
        // 遍历前矩阵行
        for(int i = 0;i < rows;i++){
            // 遍历后矩阵列
            for(int j = 0;j < b.cols;j++){
                // 遍历前矩阵列/后矩阵行，进行点积
                for(int k = 0;k < cols;k++){
                    res.mat[i][j] += mat[i][k] * b.mat[k][j];
                }
            }
        }
        return res;
    }
    Matrix& operator*=(const Matrix &b){
        return *this = *this * b;
    }
};
Matrix qpow(Matrix A, ll p){
    // 结果矩阵，初始化为单位矩阵
    Matrix res = Matrix(A.rows, true);
    // 要求是方阵
    if(A.rows != A.cols) return res;
    // 快速幂模板
    while(p > 0){
        // 如果当前二进制位为1，则乘上当前矩阵
        if(p & 1) res *= A;
        // 当前矩阵平方，二进制位右移
        A *= A;
        p >>= 1;
    }
    return res;
}

//# 矩阵快速幂求斐波那契数列
// 一种简单的线性递推
// 斐波那契数列的递推关系为：F(n) = F(n-1) + F(n-2)

/*
 | F(n)   | = | 1 1 | * | F(n-1) |
 | F(n-1) |   | 1 0 |   | F(n-2) |
*/
/*
 | F(n)   | = | 1 1 |^(n-1) | F(1) | = | 1 1 |^(n-1) | 1 |
 | F(n-1) |   | 1 0 |       | F(0) |   | 1 0 |       | 0 |
*/
// 其中 *(1,0)^T 相当于保留第一列，所以第一列就是 F(n) 和 F(n-1)
// 再取第一列第一行就是 F(n) 
// 通过快速幂，O(log n) 时间复杂度内算出 F(n) 

ll fib(ll n){
    // 构造转移矩阵
    Matrix F(2);
    F.mat[0][0] = 1; F.mat[0][1] = 1;
    F.mat[1][0] = 1; F.mat[1][1] = 0;
    // 快速幂
    Matrix res = qpow(F, n-1);
    // 返回第一列第一行，即 F(n)
    return res.mat[0][0];
}

int main(){
    int n;
    cin >> n;
    cout << fib(n) << endl;
    return 0;
}