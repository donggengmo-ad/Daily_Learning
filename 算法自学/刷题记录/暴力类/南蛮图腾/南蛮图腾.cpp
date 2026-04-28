#include <iostream>
#include <string>
#include <vector>
using namespace std;

//# 洛谷P1498
// 这题没什么难度，就是递归画小三角，主要是比较好玩，留作纪念

int n;
vector<int> pw;
vector<string> pat;
void draw(int scale, int lev, int bias){
    if(scale == 1){
        pat[lev].replace(bias, 4, " /\\ ");
        pat[lev+1].replace(bias, 4, "/__\\");
        return;
    }
    scale--;
    draw(scale, lev, bias + pw[scale]);
    for(int i = 0;i < 2;i++) draw(scale, lev + pw[scale], bias + pw[scale+1] * i); 
}
int main(){
    cin >> n;
    pw.push_back(1);
    for(int i = 1;i <= n + 1;i++) pw.push_back(pw[i-1] << 1);
    pat.assign(pw[n], string(pw[n+1], ' '));
    draw(n, 0, 0);
    for(int i = 0;i < pat.size();i++) cout << pat[i] << endl;
    return 0;
}