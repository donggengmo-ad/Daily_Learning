""" 
分支结构常用的有三种：
    if...elif...else结构、
    try...except...else...finally结构、
    match...case结构 （类似于C语言中的switch语句）
"""

#try是用来捕获异常的

#异常类型：
#ValueError：参数类型正确但值不合法
#TypeError：类型不匹配
#IndexError：索引超出范围
#KeyError：键不存在
#ZeroDivisionError：除数为零
#FileNotFoundError：文件不存在
#NameError：变量未定义
#SyntaxError：语法错误
#……

    

num = input("请输入一个数字：")

# 1. 判断输入的是否是数字
# try...except...else...finally结构
try:
    int(num)
except ValueError:
    # 出现错误时执行的代码
    print("输入的不是数字")
except:
    print("发生了其他错误")
else:
    # 2. 判断输入的数字是几位数
    # if...elif...else结构
    num = int(num)
    if 0 <= num < 10:
        print("输入的数字是个位数")
    elif num >= 10 and num < 100:
        print("输入的数字是两位数")
    else:
        print("三位数或更多位")
    # 3. 判断输入的数字是奇数还是偶数
    if not num % 2:
        print("输入的数字是偶数")
    else:
        print("输入的数字是奇数")
finally:
    # 无论如何都会执行的代码
    print("判断完成")

    
