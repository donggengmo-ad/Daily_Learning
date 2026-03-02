# 简单感知机

## 感知机
#? 定义：接收多个信号，输出单个信号
#? 组成：输入，权重，偏置，输出

## 简单实践

import numpy as np

def AND(x1,x2):
    x = np.array([x1,x2])
    w = np.array([0.5,0.5])
    b = -0.7

    y = np.sum(w * x) + b

    if y <= 0 :
        return 0
    else:
        return 1

    
if __name__ == "__main__":
    a,b = input("请输入两个二进制数（0或1） , 不分隔：")
    a,b = int(a),int(b)
    print("AND({}, {}) = {}".format(a,b,AND(a,b)))