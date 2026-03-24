# 用梯度下降，求二次函数f(x)的极小值

## 设置初始参数
alpha = 0.1 # 迭代速率
iteration_num = 100 #迭代次数

## 设置函数和导函数
def f(x):
    y = x**2 - x*2 - 3
    return y
def df(x):
    y = x*2 - 2
    return y

## 随机取x并开始迭代
x = 2
for i in range(iteration_num):
    #* 求出梯度
    gradient = df(x)
    #* 算出步长
    step = gradient * alpha
    #* 梯度下降
    x -= step
    #/ 打印函数值和梯度
    print(f"iteration {i}, x = {x:.3f}, y = {f(x):.3f}, gradient = {gradient:.3f}")

#? 最终得出 x = 1 时取最小值，y = -4，此时梯度为0
