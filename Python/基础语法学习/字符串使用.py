#字符串的使用

'''
字符串是不可变的，
一旦创建了一个字符串，就不能修改它的内容，
但可以通过字符串方法来创建新的字符串。

可以用字符串不赋值，相当于注释掉了代码
'''

#普通打印
print("Hello World")

#引号区分
print('"Hello World"')

#多行字符串
print("""
      Hello
      World
      """)

#转义字符
print("Hello\nWorld")  # 换行
print("Hello\tWorld")  # 制表符
print("Hello\\World")  # 反斜杠
print("She said, \"Hello World\"")  # 双引号转义

#字符串转换
num = 123
print(str(num))  # 将数字转换为字符串
print(int("123"))  # 将字符串转换为数字

#字符串拼接
print("Hello" + " " + "World")
print("Hello", "World")  # 使用逗号会自动添加空格
print("Hello" + str(123))  # 将数字转换为字符串进行拼接

#字符串重复
print("Hello " * 3)

#字符串索引
s = "Hello World"
print(s[0])  # H 索引，0表示第一个字符
print(s[-1]) # d
print(s[0:5])  # Hello 切片，0:5表示从索引0到索引5（不包括5）之间的字符
print(s[6:])   # World 后面不写一直到末尾
print(s[:5])   # Hello 前面不写从头开始
print(s[::])   # Hello World 切片的步长参数，::x表示每x个字符取一个，默认步长为1
print(s[::2])  # HloWrd  每2个字符取一个
print(s[::-1])  # dlroW olleH  反转字符串
print(s[1:10:2])  # el ol  从索引1到索引10（不包括10）之间的字符，每2个字符取一个
#总结：s[a:b:c:] a表示起始索引，b表示结束索引（不包括b），c表示步长 


#字符串格式化
name = "Alice"
age = 30
print("My name is {} and I am {} years old.".format(name, age)) #依次替换
print("I am a {0}, he is also a {0}, Mr.Alice is our {1}.".format("student", "teacher"))  #位置参数
print("My name is {n} and I am {a} years old.".format(n="Alice", a=30))  #关键字参数
print(f"My name is {name} and I am {age} years old.")  #f-string格式化

#字符串方法（不改变原字符串，返回新字符串）
s = "hello world"
print(s.upper())  # HELLO WORLD 转换为大写
print(s.lower())  # hello world 转换为小写
print(s.title())  # Hello World 每个单词首字母大写
print(s.strip())  # hello world 去除字符串两端的空白
print(s.replace("world", "Python"))  # hello Python 替换字符串中的内容
print(s.split())  # ['hello', 'world'] 将字符串分割成列表
print(s.find("world"))  # 6 查找子字符串的位置，返回索引，如果找不到返回-1
print(s.count("o"))  # 2 统计子字符串出现的次数


