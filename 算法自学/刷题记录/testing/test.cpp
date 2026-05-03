#include <iostream>
#include <vector>
using namespace std;

struct Node{
    int data;
    int parent;
};
int pow2(int p){
    int res = 1, a = 2;
    while(p > 0){
        if(p & 1) res *= a;
        a *= a;
        p >>= 1;
    }
    return res;
}

int main(){
    int n;
    cin >> n;
    cout << pow2(n);
    //vector<Node> a(pow2(n));
    return 0;
}
