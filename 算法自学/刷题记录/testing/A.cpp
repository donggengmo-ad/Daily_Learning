#include <iostream>
#include <vector>
using namespace std;
void solve(){
    int n, yes = 0;
    cin >> n;
    for(int i = 0;i < n;i++){
        int x;
        cin >> x;
        if(x == 100 && !yes){
            cout << "Yes" << endl;
            yes = 1;
        }
    }
    if(!yes) cout << "No" << endl;
}
int main(){
    int t;
    cin >> t;
    while(t--) solve();
    return 0;
}