# 常用的正则表达式符号
#_ . 表示匹配任意单个字符（除了换行符）
#_ * 表示匹配前面的字符零次或多次
#/ + 表示匹配前面的字符一次或多次
#_ ? 表示匹配前面的字符零次或一次
#/ ^ 表示匹配字符串的开头
#/ $ 表示匹配字符串的结尾
#/ \ 表示转义字符，用于匹配特殊字符，例如 \. 匹配点号
#/ 转义字符
    #/ \n 表示换行符
    #/ \t 表示制表符
    #/ \d 表示匹配任意数字
    #/ \w 表示匹配任意字母、数字或下划线
    #/ \s 表示匹配任意空白字符（包括空格、制表符、换行符等）
#/ () 表示分组，可以将多个字符组合在一起进行匹配

## 最常用：
##   (.*?): 匹配任意字符（非贪婪模式），直到遇到冒号为止
##   (.*): 匹配任意字符（贪婪模式），直到字符串的结尾为止

import re

#? findall方法
text = "Hello, my name is John, I am 30 years old."
pattern = "am (.*?) years old"
# 提取单个内容
age = re.findall(pattern, text)
print(age)  # 输出: ['30']
# 提取多个内容
pattern = "name is (.*?), I am (.*?) years old"
matches = re.findall(pattern, text)
print(matches)  # 输出: [('John', '30')]
# flag参数可用来忽略换行、大小写等

#? search方法
pattern = "name is (.*?),"
name = re.search(pattern, text)
print(name.group(1))  #* group(1)表示提取第一个分组的内容，即John

#? 先抓大后抓小
text = "The price is $100, $200, and $300, but the discount is $50, $30, and $20."
#/ 直接抓小会得到 ['100', '200', '300', '50', '30', '20']，无法区分价格和折扣
# 抓大
pattern = "price is (.*), but"
match = re.findall(pattern, text)
print(match)  # 输出: ['100, $200, and $300']
# 抓小
pattern = "\$(.*?), "
matches = re.findall(pattern, match[0])
print(matches)  # 输出: ['100', '200', '300']