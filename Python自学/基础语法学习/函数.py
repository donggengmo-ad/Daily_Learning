# 函数是用来实现特定功能的代码块
# 可以分为计算类函数和功能类函数

# 计算类函数：输入参数，经过计算，返回结果
def add(a, b):
    return a + b

# 功能类函数：执行特定的操作，可能不返回结果
def greet(name):
    print("Hello, " + name + "!")
    
# 调用函数
result = add(5, 3)
print("5 + 3 =", result)
greet("Alice")

# 函数的参数分类
def positional_args(a, b):  #位置参数（必传，按位置）
    print("a:", a)
    print("b:", b)

def default_args(a=5, b=10):  #默认参数（可选，不传使用默认值）
    print("a:", a)
    print("b:", b)

def keyword_args(name1, name2):  #关键字参数（必传，按名称）
    print("a:", name1)
    print("b:", name2)

def variable_args(*args):  #可变参数，任意多个位置参数->元组（传一堆没名的参数）
    for i, arg in enumerate(args):
        print("arg {}: {}".format(i, arg))

def variable_keyword_args(**kwargs):  #可变关键字参数，任意多个关键字参数->字典（传一堆有名的参数）
    for key, value in kwargs.items():
        print("key: {}, value: {}".format(key, value))
    print("find: " + kwargs["find"])  #通过键访问值
        
# 调用函数
positional_args(1, 2)
default_args()
keyword_args(name2=2, name1=1)
variable_args(5, 6, 7)
variable_keyword_args(name1=8, name2=9, find="找到我了")

# 高阶函数：接受函数作为参数，或者返回一个函数

#1.接受函数作为参数
def calculate(*args, calculator):
    result = calculator(*args)
    print("结果是：", result)
    
def multiply(x, y):
    return x * y
def square(x):
    return x * x
def cube(x):
    return x * x * x
def power(x, y):
    return x ** y

calculate(5, 3, calculator=multiply)
calculate(4, calculator=square)
calculate(2, calculator=cube)
calculate(2, 3, calculator=power)

#2.返回一个函数
def make_multiplier(n):
    def multiplier(x):
        return x * n
    return multiplier

double = make_multiplier(2) #接收make_multiplier函数的返回值，即multiplier函数，并将n固定为2
triple = make_multiplier(3)

print(double(5))  # 10
print(triple(5))  # 15

#匿名函数：没有名字的函数，通常用于需要一个函数对象但不想定义一个正式函数的场合
#语法：lambda 参数: 表达式
#lambda函数只能包含一个表达式，不能包含多行代码，也不能使用return语句，表达式的结果就是函数的返回值
a = (lambda x, y: x + y)(5, 3)  # 8
print("5 + 3 =", a)
