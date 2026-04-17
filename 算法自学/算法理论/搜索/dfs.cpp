#include <iostream>
#include <vector>
using namespace std;

//# 洛谷P2089

// 给定一个整数n，求有多少个长度为10的序列满足：
// 1. 序列中的每个数都是1、2、3之一
// 2. 序列中所有数的和为n

// 目标，计数
int n, cnt = 0;
// 记录满足条件的序列
vector<vector<int>> rec;

// pos 当前枚举到序列的第几个数了
// sum 当前枚举的序列的和
// path 当前枚举的序列
void dfs(int pos, int sum, vector<int> &path){
    if(pos == 10){// 枚举完了一个长度为10的序列
        if(sum == n){// 满足条件
            cnt++;// 计数
            rec.push_back(path);// 记录序列
        }
        return;
    }
    // 当前数枚举1、2、3
    for(int i = 1;i <= 3;i++){
        // 在这里也可以剪枝，先不管了
        // 加入当前数
        path.push_back(i);
        // 递归伸入下个位置
        dfs(pos + 1, sum + i, path);
        // 回退，枚举下一个数
        path.pop_back(); 
    }
}

int main(){
    cin >> n;
    
    // 剪枝，和不可能达到
    if(n > 30 || n < 10) {
        cout << 0;
        return 0;
    }

    vector<int> path;

    dfs(0, 0, path);

    // 输出总数
    cout << cnt << endl;

    // 输出满足条件的序列
    for(vector<int> v:rec){
        for(int i:v) cout << i << ' ';
        cout << endl;
    }
    
    return 0;
}