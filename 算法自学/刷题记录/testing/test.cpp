#include <iostream>
#include <vector>
#include <stack>
using namespace std;

int main(){
    stack<int> st;
    while(1){
        char c;
        cin >> c;
        if(c == '@') break;
        if(c == '.') continue;
        if(c <= '9' && c >= '0'){
            int num = c - '0';
            while(1){
                cin >> c;
                if(c == '.') break;
                num = num * 10 + c - '0';
            }
            st.push(num);
        } 
        else if(c == '+'){
            int num = st.top();
            st.pop();
            num += st.top();
            st.pop();
            st.push(num);
        }
        else if(c == '-'){
            int num = st.top();
            st.pop();
            num = st.top() - num;
            st.pop();
            st.push(num);
        }
        else if(c == '*'){
            int num = st.top();
            st.pop();
            num *= st.top();
            st.pop();
            st.push(num);
        }
        else if(c == '/'){
            int num = st.top();
            st.pop();
            num = st.top() / num;
            st.pop();
            st.push(num);
        }
    }
    cout << st.top();
    return 0;
}