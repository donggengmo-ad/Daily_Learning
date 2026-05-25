#include <iostream>
using namespace std;

//# 堆排序
//$ 时间复杂度：O(nlogn)，空间复杂度：O(1)，不稳定
//? 维护一个堆结构，每次取出堆顶元素（最大值）放到数组末尾，调整剩余元素使其重新满足堆结构，直到堆中只剩一个元素
/*
完全二叉树：
    节点idx：
        左子节点 2 * idx
        右子节点 2 * idx + 1
        父节点 idx / 2
*/
void maxHeapify(int heap[], int start, int end){
    // start为根结点编号
    int son = start * 2; // son为start左子树的根，因此son + 1为右子树根
    while(son <= end){
        // 取左右子树根中较大的
        if(son + 1 <= end && heap[son + 1] > heap[son]) son++;
        if(heap[son] > heap[start]){
            // 比根节点大则交换
            swap(heap[start], heap[son]);
            // 进入下一层，准备下一轮迭代
            start = son;
            son *= 2;
        }
        else return; // 否则说明根节点比子树都大，不需要调整，直接退出
    }
}
void heapSort(int heap[], int n){
    // 占位，方便计算子树根节点编号
    heap[0] = 0; 
    // 从最后一个非叶子节点开始调整，直到根节点
    for(int i = n / 2;i > 0;i--) maxHeapify(heap, i, n - 1);
    // 交换根节点和最后一个节点，调整剩余的树
    for(int i = n - 1;i > 1;i--){
        swap(heap[1], heap[i]);
        maxHeapify(heap, 1, i - 1); 
    }
}

int main(){
    int arr[] = {9,3,6,1,2,6,9,10};
    
    heapSort(arr, 8);
    for(int a:arr) cout << a << ' ';

    return 0;
}