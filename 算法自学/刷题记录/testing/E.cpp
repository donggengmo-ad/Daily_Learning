#include <iostream>
#include <vector>
using namespace std;
int XOR(int a,int b){
    return (a | b) - (a & b);
}
int main(){
    int t;
    cin >> t;
    while(t--){
        int n;
        cin >> n;
        vector<int> a;
        for(int i = 0;i < n;i++){
            int x;
            cin >> x;
            a.push_back(x);
        }
        
    }
    return 0;
}
