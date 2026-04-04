#include <iostream>
using namespace std;
int cmp(const void* a,const void* b){
    return *(int*)a > *(int*)b;
}
int main(){
    int n;
    cin >> n;
    for(int i = 0;i < n;i++){
        int a[7];
        for(int j = 0;j < 7;j++){
            cin >> a[j];
        }
        int sum = 0;
        qsort(a,7,sizeof(int),cmp);
        for(int j = 0;j < 6;j++){
            sum += a[j];
        }
        cout << -sum + a[6] << endl;
    }
    return 0;
}