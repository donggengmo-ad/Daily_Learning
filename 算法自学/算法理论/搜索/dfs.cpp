#include <iostream>
#include <vector>
#include <stack>
using namespace std;

//# 深度优先搜索
// 对于每一个节点，访问它的单个子节点，直到访问到叶子节点为止
// 然后回退访问下一个子节点，直到所有子节点都被访问过
//_ 用栈（或递归栈）管理“候选点”

struct Node{
    int val;
    vector<Node*> children;
};

//? 递归栈实现
void recursive_dfs(Node* node){
    // 递归出口（即上一个节点为叶子节点）
    if(!node) return;
    // 访问当前节点
    cout << node->val << " ";
    // 递归访问子节点
    for(Node* child : node->children){
        recursive_dfs(child);
    }
}
//? 栈实现
void stack_dfs(Node* root){
    // 栈中存储待访问的节点，初始时只有根节点
    stack<Node*> st;
    st.push(root);
    // 还有待访问节点，继续访问
    while(!st.empty()){
        // 取栈顶为当前节点，并弹出
        Node* node = st.top();
        st.pop();
        // 访问当前节点
        cout << node->val << " ";
        // 将子节点压入栈（后进先出，倒序保证访问顺序与递归一致）
        for(int i = node->children.size() - 1; i >= 0; i--){
            st.push(node->children[i]);
        }
    }
}


int main(){
    return 0;
}