import matplotlib.pyplot as plt
import numpy as np

#_绘制z = x^2 + y^2的三维图像

def plot_3d_surface(mode):
    # 画布
    board = plt.figure()

    # 定义数据点
    #/ 生成等间距x，y列表
    X = np.arange(-5, 5, 0.5)
    Y = np.arange(-5, 5, 0.5)
    #/ 将x，y列表转换为二维数组
    X, Y = np.meshgrid(X, Y)

    # 计算z的值
    Z = X**2 + Y**2
    
    if mode == "contour":
        axis = board.add_subplot(1,1,1)
        # 绘制二维等高线图
        plt.contour(X, Y, Z)
    
    else:
        # 三维坐标轴
        axis = board.add_subplot(1,1,1,projection='3d')
        
        if mode == "surface":
            # 绘制三维曲面图像
            axis.plot_surface(X, Y, Z)

        if mode == "contour3d":
            # 绘制三维等高线图
            plt.contour(X, Y, Z)

    plt.show()
    
if __name__ == "__main__":
    mode = input("请输入绘图模式（surface：三维曲面图，contour：二维等高线图，contour3d：三维等高线图）：")
    plot_3d_surface(mode)
    