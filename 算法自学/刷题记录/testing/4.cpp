#include <iostream>
#include <string>
#include <vector>
using namespace std;

string strip(string s){
    vector<char> st;
    string res = "";
    int l = 0, r = s.size() - 1;
    for(int i = 0; i < s.size(); i++){
        res += s[i];
        if(s[i] == '(') {
            st.push_back('(');
            l = i;
        }
        else if(s[i] == ')'){
            if(st.empty()) continue;
            st.pop_back();
            r = i;
            if(r - l + 1 == 4){
                for(int j = 0;j < 4;j++) res.pop_back();
                for(int j = 0;j < 2;j++) res.push_back('x');
            } 
        }
    }
    return res;
}

void solve(){
    string s1, s2;
    cin >> s1 >> s2;
    while(s1 != strip(s1)) s1 = strip(s1);
    while(s2 != strip(s2)) s2 = strip(s2);
    if(s1 == s2) cout << "Yes" << endl;
    else cout << "No" << endl;
    return;
}

int main(){
    int t;
    cin >> t;
    while(t--){
        solve();
    }
    return 0;
}