#include <iostream>
#include <algorithm>
using namespace std;

/*
# P3799 小 Y 拼木棒

## 题目背景

上道题中，小 Y 斩了一地的木棒，现在她想要将木棒拼起来。

## 题目描述

有 $n$ 根木棒，现在从中选 $4$ 根，想要组成一个正三角形，问有几种选法？

答案对 $10^9+7$ 取模。

## 输入格式

第一行一个整数 $n$。

第二行往下 $n$ 行，每行 $1$ 个整数，第 $i$ 个整数 $a_i$ 代表第 $i$ 根木棒的长度。

## 输出格式

一行一个整数代表答案。

## 输入输出样例 #1

### 输入 #1

```
4 
1
1
2
2
```

### 输出 #1

```
1
```

## 说明/提示

#### 数据规模与约定

- 对于 $30\%$ 的数据，保证 $n \le 5 \times 10^3$。
- 对于 $100\%$ 的数据，保证 $1 \leq n \le 10^5$，$1 \le a_i \le 5 \times 10^3$。

*/
#define int long long

int C2(int n){
    return n * (n - 1) / 2;
}
const int mod = (int)1e9 + 7;

// 桶记录每个长度有多少根，不少于2的去更小桶凑
int buk[5001] = {0};
int n, cnt = 0;

// 找与x相加为sum的数是否存在，sum=2x则需要桶里有两个，返回选法数
int match(int x, int sum){
    if(x > sum) return 0;
    if(buk[x] < 1) return 0;
    int y = sum - x;
    if(y == x) {
        if(buk[y] < 2) return 0;
        return C2(buk[y]);
    }
    if(buk[y] > 0) return buk[y] * buk[x];
    return 0;
}

signed main(){
    cin >> n;

    for(int i = 1;i <= n;i++){
        int x;
        cin >> x;
        buk[x]++;
    }

    for(int i = 2;i <= 5000;i++){
        if(buk[i] < 2) continue;
        for(int j = 1;j <= i / 2;j++){// 会重复，只看一半
            cnt += match(j, i) * C2(buk[i]);
            cnt %= mod;
        }
    }
    
    cout << cnt << endl;

    return 0;
}
