import math
from math import pow as power
# from math import * 表示引入math库中的所有函数和变量，但不建议使用这种方式，因为可能会导致命名冲突和代码可读性降低。
# 计算库引入


"""
解方程：
-x² - 2x + 3 = 0
求根公式：
x = -b ± √(b² - 4ac) / 2a
其中：
a = -1, b = -2, c = 3
"""

a = -1
b = -2
c = 3

delta = b**2 - 4*a*c

x1 = (-b + math.sqrt(delta)) / (2*a)
x2 = (-b - math.sqrt(delta)) / (2*a)

print("解为：", x1, x2)

print("使用power函数计算：", power(2, 3))  # 计算2的3次方



