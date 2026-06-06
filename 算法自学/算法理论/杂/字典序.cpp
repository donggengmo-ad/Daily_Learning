#include <iostream>
#include <vector>
using namespace std;
#define int long long

/*
    想象一个线段，被分成n1个小段，每小段又被分成n2个更小段，依此类推
    这样就形成了一个树形结构，树的每一层代表一个位置，每个节点代表一个数
    字典序列的每位会对应一个节点，数值决定走向哪个分支，最终会到达一个叶子节点，叶子节点的编号就是字典序对应的数字
*/


// 预处理阶乘
vector<int> fac;
int n;

// 将字典序转换为数字
int vec2num(vector<int> vec){
    int num = 0;
    // 处理每一位
    for(int i = 0;i < n;i++){
        int cnt = 0;
        // 统计后面比当前位小的数的个数
        for(int j = i + 1;j < n;j++){
            if(vec[j] < vec[i]) cnt++;
        }
        // 剩余n-i-1位数，有fac[n-i-1]种排列方式
        // 比当前位小的有cnt个数，即比当前小的排列cnt * fac[n-i-1]种
        num += cnt * fac[n-i-1];
    }
    return num;
}
// 将数字转换为字典序
vector<int> num2vec(int num){
    vector<int> all, vec;
    for(int i = 1;i <= n;i++) all.push_back(i);
    for(int i = n;i >= 1;i--){
        int pos = num / fac[i-1];
        vec.push_back(all[pos]);
        all.erase(all.begin() + pos);
        num %= fac[i-1];
    }
    return vec;
}

signed main(){
    int m;
    cin >> n >> m;
    fac.push_back(1);
    for(int i = 1;i <= n;i++){
        fac.push_back(fac.back() * i);
    }    

    vector<int> vec(n);
    for(int i = 0;i < n;i++) cin >> vec[i];

    int num;
    vector<int> ans;
    num = vec2num(vec);
    num += m;
    ans = num2vec(num);
    
    for(int i = 0;i < n;i++) cout << ans[i] << ' ';
    
    return 0;
}