#include <iostream>
using namespace std;
int main(){
    int n, m;
    int buk[101] = {0};
    cin >> n >> m;
    for(int i = 1; i <= n; i++){
        int x;
        cin >> x;
        buk[x]++;
    }
    int ans1 = 1, ans2 = 1;
    for(int i = 1; i <= m; i++){
        if(buk[i] == 0) ans2 = 0;
        if(buk[i] > 1) ans1 = 0;
        if(ans1 == 0 && ans2 == 0) break;
    }
    if(ans1) cout << "Yes" << endl;
    else cout << "No" << endl;
    if(ans2) cout << "Yes" << endl;
    else cout << "No" << endl;
    return 0;
}