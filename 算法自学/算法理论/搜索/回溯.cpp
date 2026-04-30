#include <iostream>
#include <vector>
using namespace std;

//# 回溯
// 系统地搜索解空间，用于寻找满足特定条件的解（或所有解）
// 对于每个可能选择，选择->继续搜索->回退（撤销选择）->继续搜索下一个选择
//_ 一般结合dfs实现

int maxlen = 3; // 目标解的长度
int selection = 5; // 选项的范围
void backtrack(vector<int> &path){
    // 满足条件，处理当前解
    if(path.size() == maxlen){
        for(int num : path) cout << num << ' ';
        cout << endl;
        return;
    }
    for(int i = 1; i <= selection; i++){
        // 做选择
        path.push_back(i);
        // 继续搜索
        backtrack(path);
        //* 回退（撤销选择）
        path.pop_back();
    }
}

int main(){
    return 0;
}