#include <bits/stdc++.h>
using namespace std;
int getRain(vector<int> a,int n){
    vector<int> maxLR;
    vector<int> maxRL;
    int maxI=0;
    for(int i=0;i<n;i++){
        maxI=max(maxI,a[i]);
        maxLR.push_back(maxI);
    }
    maxI=0;
    for(int i=n-1;i>=0;i--){
        maxI=max(maxI,a[i]);
        maxRL.push_back(maxI);
    }
    reverse(maxRL.begin(), maxRL.end());

    int rain = 0;
    for(int i=0;i<n;i++){
        int rainI = min(maxLR[i],maxRL[i])-a[i];
        rainI = rainI>0 ? rainI:0;
        rain+=rainI;
    }
    return rain;
}
void testCase(){
    int n;
    cin>>n;
    vector<int> a;
    for(int i=0;i<n;i++){
        int temp;
        cin>>temp;
        a.push_back(temp);
    }
    int q;
    cin>>q;
    for(int i=0;i<q;i++){
        int x,v;
        cin>>x>>v;
        if(x>0 && x<=n) a[x-1]+=v;
        cout<<getRain(a,n)<<endl;
    }
}
int main(){
    int T;
    cin>>T;
    for(int i=0;i<T;i++){
        testCase();
    }
    return 0;
}