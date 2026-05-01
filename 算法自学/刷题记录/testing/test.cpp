#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;
struct Boat{
    int t, k;
    vector<int> x;
};
int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    int n;
    cin >> n;
    queue<Boat> boats;
    unordered_map<int, int> cnts;
    for(int i = 0;i < n;i++){
        Boat b;
        cin >> b.t >> b.k;
        b.x.resize(b.k);
        for(int j = 0;j < b.k;j++) {
            cin >> b.x[j];
            if(cnts.count(b.x[j])) cnts[b.x[j]]++;
            else cnts[b.x[j]] = 1;
        }
        boats.push(b);
        while(!boats.empty() && b.t - boats.front().t >= 86400){
            for(int c:b.x) {
                if(cnts.count(c)) cnts[c]--;
                if(cnts[c] <= 0) cnts.erase(c);
            }
            boats.pop();
        }
        cout << cnts.size() << '\n';
    }
    return 0;
}