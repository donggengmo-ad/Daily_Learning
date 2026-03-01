import matplotlib.pyplot as plt
import mpl_toolkits.axisartist as axisartist
import numpy as np

#传入左右下上，绘制坐标系
def draw_board(left, right, down,up):
    #创建画布
    board = plt.figure()
    #创建坐标轴
    axis = axisartist.Subplot(board, 111)
    #坐标轴添加到画布
    board.add_axes(axis)
    #设置坐标轴范围
    axis.set_aspect('equal')
    #隐藏原始坐标轴
    axis.axis[:].set_visible(False)
    
    #创建新的悬浮坐标
    axis.axis['x'] = axis.new_floating_axis(0, 0)
    #末端画箭头
    axis.axis['x'].set_axisline_style('->')
    #设置x轴方向
    axis.axis['x'].set_axis_direction('top')
    #设置x轴范围
    axis.set_xlim(left, right)
    
    #同理创建y轴
    axis.axis['y'] = axis.new_floating_axis(1, 0)
    axis.axis['y'].set_axisline_style('->')
    axis.axis['y'].set_axis_direction('right')
    axis.set_ylim(down, up)
    

def draw_line(k,b,left, right):
    #生成100个相邻浮点数
    x = np.linspace(left, right, 100)
    y = k * x + b
    plt.plot(x, y)
    
def draw_quadratic(a, b, c, left, right):
    x = np.linspace(left, right, 100)
    y = a * x**2 + b * x + c
    plt.plot(x, y)

    
if __name__ == "__main__":
    left = -10
    right = 10
    draw_board(left, right, left, right)
    
    draw_line(1, 0, left, right)
    draw_quadratic(1, 0, 0, left, right)
    
    plt.show()