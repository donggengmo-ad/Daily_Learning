# include <iostream>
# include <vector>
using namespace std;
int gcd(int a,int b){
    if(b == 0) return a;
    return gcd(b,a % b);
}
int main(){
    int t;
    cin >> t;
    while(t--){
        int n;
        cin >> n;
        vector<int> a;
        for(int i = 0;i < n;i++){
            a.push_back(1);
            int last_gcd = i >= 2 ? gcd(a[i-2],a[i-1]) : 1;
            while(gcd(a[i-1],a[i]) == last_gcd){
                a[i]++;
            }
        }
        for(int i = 0;i < n;i++){
            cout << a[i] << " ";
        }
        cout << endl;
    }
    return 0;
}