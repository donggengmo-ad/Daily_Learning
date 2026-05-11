#include <iostream>
using namespace std;
using ll = long long;
int main() {
    int n;
    cin >> n;
    while(n--){
        ll l, r;
        cin >> l >> r;
        ll ub = 0, lb = 0;
        while(ub <= r) ub = (ub << 1) + 1, lb = (ub >> 1);
        if(lb >= l) {cout << lb << endl; continue;}
        ub = lb = l;
        while(ub <= r) lb = ub, ub |= ub + 1;
        cout << lb << endl;
    }
    return 0;
}