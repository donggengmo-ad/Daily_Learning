#include <iostream>
#include <vector>
using namespace std;

//@ 洛谷P4147

struct Block{
    char a;
    int l;
    int r;
    int h;
};
void two_d_dp(int n, int m){
    // 二维dp版本
    vector<vector<Block>> M(n + 1, vector<Block>(m + 1));
    for(int i = 1;i <= n;i++){
        // 初始化
        for(int j = 1;j <= m;j++){
            cin >> M[i][j].a;
            M[i][j].h = 1;
            M[i][j].l = j;
            M[i][j].r = j;
        }
        // 从左向右推左边界
        for(int j = 2;j <= m;j++){
            if(M[i][j].a == 'F' && M[i][j-1].a == 'F')
                M[i][j].l = M[i][j-1].l;
        }
        // 从右向左推右边界
        for(int j = m - 1;j > 0;j--){
            if(M[i][j].a == 'F' && M[i][j+1].a == 'F')
                M[i][j].r = M[i][j+1].r;
        }
    }
    // 对齐与计算
    int ans = 0;
    for(int i = 1;i <= n;i++){
        for(int j = 1;j <= m;j++){
            if(i == 1 && M[i][j].a == 'F'){
                // 第一行单独计算
                int s = (M[i][j].r - M[i][j].l + 1) * M[i][j].h;
                ans = max(s, ans);
            }
            if(M[i][j].a == 'F' && M[i-1][j].a == 'F'){
                // 更新高度
                M[i][j].h = M[i-1][j].h + 1;
                // 与上一行左右边界对齐
                M[i][j].l = max(M[i][j].l, M[i-1][j].l);
                M[i][j].r = min(M[i][j].r, M[i-1][j].r);
                // 计算面积取最大
                int s = (M[i][j].r - M[i][j].l + 1) * M[i][j].h;
                ans = max(s, ans);
            }
        }
    }
    // 答案要求乘以3
    cout << ans * 3;
}

void one_d_dp(int n, int m){
    // 因为只需要上一行的状态，所以一维dp就行，用上一次状态更新当前行
    vector<Block> M(m + 1);
    vector<vector<char>> a(n + 1, vector<char>(m + 1));
    // 输入并初始化
    for(int i = 1;i <= n;i++) 
        for(int j = 1;j <= m;j++) {
            cin >> a[i][j];
            M[j].l = 1;// 初始全伸到最左
            M[j].r = m;// 初始全伸到最右
            M[j].h = 0;// 在第一行会被多加一次，所以设0
        }
    int ans = 0;
    for(int i = 1;i <= n;i++){
        // 更新高度
        for(int j = 1;j <= m;j++){
            if(a[i][j] == 'F') M[j].h++;// 接续上一行的高度+1
            else M[j].h = 0;// 遇到障碍高度置0，下一行又是1了（如果不是障碍）
        }
        // 从左向右推左边界
        int l = 1; // 当前连续F的起始位置
        for(int j = 1;j <= m;j++){
            // 这里 M[j].l = 本列上一行的左边界，l = 本列当前行的左边界
            if(a[i][j] == 'F') M[j].l = max(M[j].l, l);
            else {
                l = j + 1;// 之后的列的左边界至少要从j+1开始
                M[j].l = 1;// 障碍行不希望对下一行起任何影响，所以重置为初始状态
            }
        }
        // 从右向左推右边界（同理）
        int r = m;
        for(int j = m;j > 0;j--){
            if(a[i][j] == 'F') M[j].r = min(M[j].r, r);
            else {
                r = j - 1;
                M[j].r = m;
            }
        }
        // 每过一行统计一次最大面积
        for(int j = 1;j <= m;j++){
            int s = (M[j].r - M[j].l + 1) * M[j].h;
            ans = max(s, ans);
        }
    }
    cout << ans * 3;
}

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);

    int n, m;
    cin >> n >> m;

    // 悬线dp，向左右伸到最长记录位置，向上记录高度
    one_d_dp(n, m);

    return 0;
}