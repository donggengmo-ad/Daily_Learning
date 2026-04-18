#include <iostream>
using namespace std;
using ll = long long;

int N, M, F;
int p[100001], c[100001];
ll seed;

void read(int q[],cint d[]) {
    ll state = seed;

    for (int i = 2; i <= N; i++) {
        if(i <= M) p[i] = q[i];
        else{
            p[i] = (state % (i-1)) + 1;
            state = (state * 1103515245 + 12345) % (1LL << 31);
        }
    }

    for (int i = 1; i <= N; i++) {
        if (i <= M) c[i] = d[i];
        else {
            c[i] = (state % F) + 1;
            state = (state * 1103515245 + 12345) % (1LL << 31);
        }
    }
}

int main(){


    return 0;
}