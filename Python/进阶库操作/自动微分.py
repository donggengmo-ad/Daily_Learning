import torch
import matplotlib.pyplot as plt
import numpy as np

def f(x):
    return x**2 + 3*x + 5



def show():
    # 生成自变量序列
    x = torch.linspace(-10, 10, 100, requires_grad=True)
    # 计算函数值
    y = f(x)
    
    # 计算导数值
    #/ backward()只能对标量进行反向传播，因此需要对y求和得到一个标量
    y.sum().backward()
    #/ 梯度值保存在x.grad中
    grad = x.grad
    if grad is None:
        raise RuntimeError("梯度计算失败：x.grad 为 None")
    
    # 张量转为数组
    dy = grad.detach().numpy().copy()
    x_np = x.detach().numpy()
    y_np = y.detach().numpy()
    grad.zero_()  # 梯度清零

    # 绘制函数图像和导数图像
    plt.plot(x_np, y_np, label='f(x) = x^2 + 3x + 5')
    plt.plot(x_np, dy, label="f'(x) = 2x + 3")
    
    
    plt.legend()
    plt.grid(True)
    
    plt.show()
    
if __name__ == "__main__":
    show()