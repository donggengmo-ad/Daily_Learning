# 用多层感知机实现异或门

import numpy as np

class MLP:
    def __init__(self):
        self.lr = 0.1 #学习率

        #三层
        self.input = np.zeros((1,2)) #1行2列的全0数组，即2维行向量
        self.hidden = np.zeros((1,4)) #4维行向量

        #层间计算权重
        self.w1_2 = np.zeros((2,4)) #2行4列矩阵，2维行向量右乘该矩阵转为4维行向量
        self.b1_2 = np.zeros((1,4))
        
        self.w2_3 = np.zeros((4,1)) #4维列向量，4维行向量右乘该向量转为标量
        self.b2_3 = 0

    def forward(self,input_list):
        
        self.input = np.array(input_list)

        #_矩阵乘法
        #? a @ b 等价于np.dot(a,b)
        self.hidden = self.input @ self.w1_2 + self.b1_2
        self.output = self.hidden @ self.w2_3 + self.b2_3

        return self.output
    
    def backward(self,y_true):
        # loss2_3 = (y - wx - b) ** 2
        # dloss/dw = （y - wx - b）* 2 * (-x)
        # dloss/db = （y - wx - b）* 2 * (-1)
        # 反向传播
        grad_w2_3 = (y_true - self.output[0][0]) * 2 * (-1 * self.hidden) # 4维行向量
        grad_b2_3 = (y_true - self.output[0][0]) * 2 * (-1) # 标量
        grad_w1_2 = self.input.T @ grad_w2_3 # 2维行向量转置为2维列向量，右乘4维行向量转为2行4列矩阵
        grad_b1_2 = grad_w2_3 # 4维行向量

        # 更新权重
        self.w2_3 -= grad_w2_3 * self.lr
        self.b2_3 -= grad_b2_3 * self.lr
        self.w1_2 -= grad_w1_2 * self.lr
        self.b1_2 -= grad_b1_2 * self.lr
       
if __name__ == "__main__":
    mlp = MLP()
    for i in range(1000):
        mlp.forward([0,1])
        mlp.backward(1)
        mlp.forward([1,0])
        mlp.backward(1)
        mlp.forward([0,0])
        mlp.backward(0)
        mlp.forward([1,1])
        mlp.backward(0)

    # 输入测试
    s = input("输入2个二进制数，空格分隔")
    #_字符串转数组
    #? 也可以list(map(int,s.split()))
    # split():"1 2 3 4" -> ["1","2","3","4"]
    # map(int,):对列表["1","2","3","4"]中元素一一执行int()函数
    # list():接收所有元素组成列表
    input_list = list(map(int,s.split()))
    output = mlp.forward(input_list)
    print("输出：",int(output[0][0]))
    
