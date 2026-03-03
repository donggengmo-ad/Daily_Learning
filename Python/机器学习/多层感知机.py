# 用多层感知机实现异或门

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import BoundaryNorm

# 或门+与非门 -> 与门 -> 异或门
class MLP:
    def __init__(self):
        self.lr = 0.1 #学习率

        #三层
        self.input = np.zeros((1,2)) #1行2列的全0数组，即2维行向量
        self.hidden = np.zeros((1,2)) #2维行向量
        self.output = np.zeros((1,1)) #1维行向量

        #层间计算权重
        self.w1_2 = np.array([[-0.5,0.5],
                                     [-0.5,0.5]]) #2行2列矩阵
        self.b1_2 = np.array([0.7,-0.2]) #1行2列矩阵
        
        self.w2_3 = np.array([[0.5],[0.5]]) #2行1列矩阵
        self.b2_3 = np.array([-0.7]) #1行1列矩阵

    def forward(self,input_list):
        self.input = np.array([input_list]) # 确保为 (1, 2) 形状
        # 输入层 -> 隐藏层
        self.hidden = np.dot(self.input,self.w1_2) + self.b1_2
        # 激活函数：<=0的部分置0，>0的部分变1（阶跃函数）
        self.hidden = np.where(self.hidden <= 0, 0, 1) # np.where(condition, x, y):满足条件condition的元素替换为x，不满足条件的元素替换为y
        # 隐藏层 -> 输出层
        self.output = np.dot(self.hidden,self.w2_3) + self.b2_3
        # 再次激活
        self.output = np.where(self.output <= 0, 0, 1)
        return self.output
        
    
       
if __name__ == "__main__":
    mlp = MLP()

    # 绘制决策边界
    x = np.linspace(-0.5, 1.5, 20)
    # 找y使mlp.forward(x,y)刚好为0：
    y = np.linspace(-0.5, 1.5, 20)
    z = np.zeros((len(x), len(y))) # 20行20列的全0数组
    for i,x_i in enumerate(x):
        for j,y_i in enumerate(y):
            z[i][j] = mlp.forward([x_i, y_i])[0][0]
    levels = [0,1]
    cmap = plt.get_cmap("coolwarm")
    norm = BoundaryNorm(levels, ncolors=cmap.N, clip=True)
    plt.pcolormesh(x, y, z.T, cmap=cmap, norm=norm) # z.T:转置
    plt.colorbar()
    plt.show()

    # 输入测试
    while True:
        s = input("输入2个二进制数，空格分隔")
        #_字符串转数组
        #? 也可以list(map(int,s.split()))
        # split():"1 2 3 4" -> ["1","2","3","4"]
        # map(int,):对列表["1","2","3","4"]中元素一一执行int()函数
        # list():接收所有元素组成列表
        input_list = list(map(int,s.split()))
        output = mlp.forward(input_list)
        print("输出：",output[0][0])
    
