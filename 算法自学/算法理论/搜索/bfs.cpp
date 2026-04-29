#include <iostream>
#include <vector>
#include <queue>
using namespace std;

//# 广度优先搜索
// 对于每一个节点，访问它的所有子节点
// 然后访问下层的所有子节点，直到所有节点都被访问过
//_ 用队列管理“候选点”

struct Node{
    int val;
    vector<Node*> children;
};

//? 队列实现
void bfs(Node* root){
    // 队列中存储待访问的节点，初始时只有根节点
    queue<Node*> q;
    q.push(root);
    // 还有待访问节点，继续访问
    while(!q.empty()){
        // 取队首为当前节点，并弹出
        Node* node = q.front();
        q.pop();
        // 访问当前节点
        cout << node->val << " ";
        // 将子节点压入队列（先进先出）
        for(Node* child : node->children){
            q.push(child);
        }
    }
}

int main(){
    return 0;
}