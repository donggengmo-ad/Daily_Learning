#include <iostream>
#include <algorithm>
using namespace std;
#define int long long //十年oi一场空，不开long long见祖宗！

//# 洛谷P3799

//确定边长，找两根符合的，再找两根能拼起来符合的

//组合数，题中之需要C(2, n)，即n选2
int C2(int n){
    return n * (n - 1) / 2;
}
//虽然多此一举，但我感觉直观一些
int C1(int n){
    return n;
}

//答案要求对10^9+7取模
const int mod = (int)1e9 + 7;

// 桶记录每个长度有多少根，不少于2的去更小桶凑
int buk[5001] = {0};
int n, cnt = 0;

// 找与x相加为sum的数是否存在，返回选法数
int match(int x, int sum){
    if(x > sum) return 0;//x太大不行，y不可以为负
    if(buk[x] < 1) return 0;//桶里没有x
    int y = sum - x;
    if(y == x) {//要取同样长的两根
        if(buk[y] < 2) return 0;//桶里不到两个
        return C2(buk[y]);//选两根的选法数
    }
    //各取一根，选法数是两桶的数量的乘积，直接取模防溢出
    if(buk[y] > 0) return C1(buk[y]) * C1(buk[x]) % mod;
    return 0;
}

//《long long main》
signed main(){
    cin >> n;
    //入桶
    for(int i = 1;i <= n;i++){
        int x;
        cin >> x;
        buk[x]++;
    }
    //枚举边长
    for(int i = 2;i <= 5000;i++){
        if(buk[i] < 2) continue;// 边长不够两根，凑不成边
        for(int j = 1;j <= i / 2;j++){// 会重复，只看一半
            //match返回两短棍的选法数，还要乘上两根长棍的选法数
            cnt += match(j, i) * C2(buk[i]);
            cnt %= mod;//再取模
        }
    }
    
    cout << cnt << endl;

    return 0;
}
