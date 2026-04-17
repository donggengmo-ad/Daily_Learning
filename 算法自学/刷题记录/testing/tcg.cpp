// test case generator
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int main(){
    srand((int)time(NULL));
    
    int n = rand() % 100 + 10;
    int k = rand() % n;

    cout << n << " " << k << endl;

    vector<ll> a(n+1);
    for(int i = 1;i <= n;i++) {
        a[i] = rand() % 1000000 + 1;
        cout << a[i] << " ";
    }
    cout << endl;

    sort(a.begin(), a.end());
    cout << "ans = " << a[++k] << endl;

}