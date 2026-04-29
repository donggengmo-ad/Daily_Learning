#include <iostream>
#include <vector>
#include <queue>
using namespace std;

//# 洛谷P1443

int n, m, x, y;
vector<vector<int>> map; 
// 结构体P用来存储当前坐标和步数
struct P{
  int r, c, t;  
};
// 使用队列来实现广度优先搜索
void bfs(queue<P> &q){
    // 取出队首元素
    auto [r, c, t] = q.front();
    q.pop();
    // 判断是否越界或者已经访问过
    if(r < 0 || r >= n || c < 0 || c >= m) return;
    if(map[r][c] != -1) return;
    // 标记当前坐标的步数
    map[r][c] = t;
    // 将所有可能的下一步坐标加入队列，步数加1
    q.push({r + 1, c + 2, t + 1});
    q.push({r + 1, c - 2, t + 1});
    q.push({r - 1, c + 2, t + 1});
    q.push({r - 1, c - 2, t + 1});
    q.push({r + 2, c + 1, t + 1});
    q.push({r + 2, c - 1, t + 1});
    q.push({r - 2, c + 1, t + 1});
    q.push({r - 2, c - 1, t + 1});
}
int main(){
    // 输入
    cin >> n >> m >> x >> y;
    map.assign(n, vector<int>(m, -1));
    // 初始化队列，加入起点坐标和步数0
    queue<P> q;
    q.push({x-1, y-1, 0});
    // 开始广度优先搜索
    while(!q.empty()) bfs(q);
    // 输出
    for(int i = 0;i < n;i++){
        for(int j = 0;j < m;j++) cout << map[i][j] << ' ';
        cout << endl;
    }
    return 0;
}