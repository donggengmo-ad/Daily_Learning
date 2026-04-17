#include <iostream>
using namespace std;
using ll = long long;

//#  洛谷P1036

int n, k, cnt = 0;
ll x[21];

//简单素数判断
bool su(ll x){
    //排除小于等于1的数，2是素数，偶数不是素数
    if(x <= 1) return false;
    if(x == 2) return true;
    if((x & 1) == 0) return false;
    //判断奇数的倍数即可
    for(ll i = 3;i <= x / i;i += 2){
        if(x % i == 0) return false;
    }
    return true;
}
//高效素数判断
bool prime(ll x){
    //把数分成6k、6k+1、6k+2、6k+3、6k+4、6k+5
    //素数只能是6k+1或6k+5，当然这个数要大于6
	if(x < 2)return 0;//负数、0、1不是素数
	if(x < 4)return 1;//2、3是素数
    if(x == 5)return 1;//5是素数
    //排除2、3、5的倍数，2倍数用位运算更快，注意&优先级比==低
    if((x & 1) == 0 || x % 3 == 0 || x % 5 == 0) return 0;
    ll i = 7;//代表第一个6k+1
	while(i <= x / i){// i*i <= x容易溢出
        if(x % i == 0)return 0;
        i += 4;//转换成6k+5
        if(x % i == 0)return 0;
        i += 2;//转换回6k+1
    }
    return 1;
}
//枚举所有组合
//pos当前枚举到哪个数，pick已经选了几个数，sum已经选的数的和
void dfs(int pos, int pick, ll sum){
    //剪枝：剩下数不够选了，直接返回
    if(k - pick > n - pos) return;
    if(pick == k){//选够了，判断sum是否是素数
        if(prime(sum)) cnt++;
        return;
    }
    //选到头了
    if(pos == n) return;
    //选这个数
    dfs(pos + 1, pick + 1, sum + x[pos]);
    //不选这个数
    dfs(pos + 1, pick, sum);
} 

int main(){
    //加速
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    
    cin >> n >> k;   
    for(int i = 0;i < n;i++) cin >> x[i];
    
    dfs(0, 0, 0);
    cout << cnt << endl;
    
    return 0;
}
