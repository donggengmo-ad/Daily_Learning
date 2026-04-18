#include <iostream>
#include <vector>
using namespace std;

int main(){
    int n, m;
    cin >> n >> m;
    vector<int> buk(n+1, 0);
    buk[1] = 1;
    vector<int> a(m+1), b(m+1);
    for(int i = 1;i <= m;i++) cin >> a[i] >> b[i];

    for(int i = 1; i <= m; i++)
        for(int j = i; j <= m; j++)
            if(buk[a[j]] > 0) buk[b[j]]++;

    int cnt = 0;
    for(int i = 1; i <= n; i++) if(buk[i] > 0) cnt++;
    cout << cnt << endl;

    return 0;
}