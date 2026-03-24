# .表示当前目录
# ..表示上一级目录

# 文件的打开和关闭

# open("文件路径","打开方式",encoding="编码格式")，返回一个文件对象

# 打开方式：
# "r"：只读模式（默认）
# "w"：写入模式（会覆盖原文件）
# "a"：追加模式（在文件末尾添加内容）
# "b"：二进制模式
# "t"：文本模式（默认）

f = open("./test.txt", "r", encoding="utf-8")  # 打开文件
print(f.read())  # 读取文件内容
print(f.read())  # 再次读取，返回空字符串，因为文件指针已经到达末尾
f.close()  # 关闭文件

# with语句,自动管理文件的打开和关闭
with open("./test.txt", "r", encoding="utf-8") as f:
    print(f.read(11))  # 读取前11个字符
    print(f.readline())  # 读取一行内容
    
    f.seek(0)  # 将文件指针移动到文件开头
    lines = f.readlines()  # 读取剩余的所有行，返回一个列表
    for line in lines:
        for char in line:
            print(char, end="")  # 打印每个字符，end=""表示不换行