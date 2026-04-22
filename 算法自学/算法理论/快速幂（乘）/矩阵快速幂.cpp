#include <iostream>
#include <vector>
using namespace std;
using ll = long long;

//# 快速幂的矩阵应用
//% A^n = A^(sum(2^i)) = product(A^(2^i))
// 就是快速幂把数乘换成矩阵乘法而已

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

int main(){
    Matrix A, B;
    ll n;
    cin >> n;
    A.in();
    B = qpow(A, n);
    B.out();
    return 0;
}