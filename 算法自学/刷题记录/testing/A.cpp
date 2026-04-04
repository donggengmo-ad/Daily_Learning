#include <iostream>
using namespace std;
int main(){
    int t;
    cin >> t;
    while(t--){
        int x;
        cin >> x;
        int y = x + 1;
        if (y > 67) y = 67;
        if (y < -67) y = -67;
        cout << y << endl;
    }
    return 0;
}