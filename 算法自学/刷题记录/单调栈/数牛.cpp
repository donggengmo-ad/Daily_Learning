#include <iostream>
#include <stack>
#include <vector>
using namespace std;

//@ 洛谷P2866

// stl栈版本
int withStack(long long N, vector<long long>& a){
    // 维护递增单调栈，每次保留左边比当前大的元素（比自己高的牛），表示能被几只牛看见
    stack<long long> st;
    long long ans = 0;

    for(int i = 1;i <= N;i++){
        // 栈顶牛小于等于当前牛时，看不到当前牛的“头顶”，所以弹出
        while(!st.empty() && st.top() <= a[i]) st.pop();
        // 当前牛能被栈中所有牛看见，所以加上栈的大小减一（不包括自己）
        st.push(a[i]);
        ans += st.size() - 1;
    }
    return ans;
}

// 数组栈版本
int withArray(long long N, vector<long long>& a){

    vector<long long> st(N+1, 0);
    long long top = 0;
    long long ans = 0;

    for(int i = 1;i <= N;i++){
        while(top >= 0 && st[top] <= a[i]) top--;
        st[++top] = a[i];
        ans += top;
    }

    return ans;
}

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    long long N;
    cin >> N;
    
    vector<long long> a(N+1);
    for(int i = 1;i <= N;i++) cin >> a[i];

    long long ans = withArray(N, a);

    cout << ans;

    return 0;
}
