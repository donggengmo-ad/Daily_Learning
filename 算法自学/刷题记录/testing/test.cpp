#include <iostream>
#include <vector>
using namespace std;
using ll = long long;

void F(string &s){
    ll len = s.length();
    s += s[len-1];
    for(ll i = 0;i < len - 1;i++) s += s[i];
}

ll halfpos(ll n, ll slen){
    ll pow2x = 1, x = 0;
    while(n >= slen * pow2x){
        pow2x *= 2;
        x++;
    }
    ll len = slen * (pow2x / 2);
    n -= len;
    n--;
    if(n < 0) n = len - 1;
    return n;
}

int main(){
    string s;
    ll n;
    cin >> s >> n;
    ll m = n, len = s.length();
    n--;
    if(n < 0){
        cout << s[0] << endl;
        return 0;
    }

    while(n >= len) n = halfpos(n, len);
    cout << s[n] << endl;

    while(s.length() < m) F(s);
    cout << s[m-1] << endl;
    return 0;
}