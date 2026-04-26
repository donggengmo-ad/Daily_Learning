#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main(){
    int n, sum = 0;
    cin >> n;
    vector<int> a(n);
    for(int i = 0;i < n;i++) cin >> a[i];
    sort(a.begin(), a.end());
    for(int i = 0;i < n - 1;i++) sum += a[i+1] += a[i];
    cout << sum << endl;
    return 0;
}