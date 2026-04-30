#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

//# 剪枝
// 去除不必要的搜索，降低复杂度

/*
? 常见方法
    1.可行性剪枝：
        状态已不可能达到目标
    2.最优性剪枝：
        已经找到一个解，当前状态已不可能比这个解更优
    3.去重剪枝：
        已经搜索过的状态不再搜索
    4.顺序剪枝：
        以某种顺序搜索，使其他剪枝效果最大化
    5.其他：
        估价函数、贪心初始解等
*/

//# 洛谷P1433 

struct Point{
    double x, y;
    Point(){}
};
double ans = -1;
int n;
vector<Point> cheeze;
vector<bool> eat;// 标记已经吃过的奶酪
vector<vector<double>> cut;// 预处理的距离矩阵
vector<double> mind;
double dist(Point a, Point b){
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}
// 贪心初始解，用于最优性剪枝
void greedy(){
    double d = 0;
    int pos = 0, num = 0;
    while(num < n){
        double minD = 1e9;
        int next = -1;
        // 每次吃最近的奶酪
        for(int i = 1;i <= n;i++){
            if(eat[i]) continue;
            if(i == pos) continue;
            if(cut[pos][i] < minD){
                minD = cut[pos][i];
                next = i;
            }
        }
        eat[next] = 1;
        d += minD;
        pos = next;
        num++;
    }
    ans = d;
}
// DFS + 剪枝
void dfs(int pos, int num, double d){
    // 可行性剪枝：递归正常结束
    if(num == n) {
        ans = (ans > 0) ? min(ans, d) : d;
        return;
    }
    // 最优性剪枝：当前距离已经不可能比ans更优
    if(d >= ans && ans > 0) return;
    double minD = 0;
    // 最优性剪枝：即使接下来选最短路，距离也不可能比ans更优
    for(int i = 1;i <= n;i++){
        if(eat[i]) continue;
        if(i == pos) continue;
        minD += mind[i];
    }
    if(minD + d >= ans && ans > 0) return;
    // 顺序剪枝：优先搜索距离较近的奶酪，使最优性剪枝剪掉更多分支
    vector<pair<double, int>> order;
    for(int i = 1;i <= n;i++) 
        if(!eat[i]) order.push_back({cut[pos][i], i});
    sort(order.begin(), order.end());
    for(auto [_, i]: order){
        if(eat[i]) continue;
        if(i == pos) continue;
        eat[i] = 1;
        dfs(i, num + 1, d + cut[pos][i]);
        eat[i] = 0;
    }
}
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n;
    cheeze.resize(n + 1);
    eat.resize(n + 1, 0);
    cut.resize(n + 1, vector<double>(n + 1, -1));
    mind.resize(n + 1, 1e9);
    cheeze[0].x = cheeze[0].y = 0;
    for(int i = 1;i <= n;i++) cin >> cheeze[i].x >> cheeze[i].y;
    for(int i = 0;i <= n;i++){
        for(int j = 0;j <= n;j++){
            if(i > j) cut[i][j] = cut[j][i];
            else cut[i][j] = dist(cheeze[i], cheeze[j]);
            if(i != j) mind[i] = min(mind[i], cut[i][j]);
        }
    }
    // 先用贪心算法得到初始解
    greedy();
    // DFS + 剪枝搜索最优解
    fill(eat.begin(), eat.end(), 0);
    dfs(0, 0, 0);
    cout << format("{:.2f}", ans);
    return 0;
}