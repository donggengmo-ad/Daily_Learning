#include <iostream>
#include <vector>
using namespace std;
int main(){
    int t;
    cin >> t;
    while(t--){
        int n;
        cin >> n;
        vector<int> a;
        for(int i = 0;i < 3 * n;i++){
            a.push_back(i + 1);
        }
        vector<int> fst;
        vector<int> snd;
        vector<int> trd;
        for(int i = 0;i < n;i++){
            fst.push_back(a[i + 2 * n]);
            snd.push_back(a[i]);
            trd.push_back(a[i + n]);
        }
        for(int i = 0;i < n;i++){
            cout << fst[i] << " " << snd[i] << " " << trd[i] << " ";
        }
        cout << endl;
    }
    return 0;
}