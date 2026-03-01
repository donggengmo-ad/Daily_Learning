# CSV文件操作

import csv

file_path = '进阶库操作\\csv文件操作\\data.csv'

# 写入CSV文件
#/ 还可以使用newline=''参数来避免在Windows系统中出现空行
#/ 还可以使用encoding参数指定编码格式
with open(file_path, mode='w') as file:
    # 创建一个DictWriter对象，指定字段名（即表头）
    writer = csv.DictWriter(file, fieldnames=['name', 'age', 'city'])
    # 写入表头
    writer.writeheader()
    # 写入数据行
    #* writerow方法写入单行数据
    dict_data = {'name': 'Alice', 'age': 30, 'city': 'New York'}
    writer.writerow(dict_data)
    #* writerows方法写入多行数据
    dict_list = [
        {'name': 'Bob', 'age': 25, 'city': 'Los Angeles'},
        {'name': 'Charlie', 'age': 35, 'city': 'Chicago'}
    ]
    writer.writerows(dict_list)
    
# 读取CSV文件
with open(file_path, mode='r') as file:
    # 创建一个DictReader对象，自动将第一行作为字段名
    reader = csv.DictReader(file)
    # 逐行读取数据
    for row in reader:
        #/ 每行数据以字典形式返回，键为字段名，值为对应的单元格内容
        print(row)  # 输出每行数据的字典形式
        #/ 还可以通过字段名访问具体的值，例如 row['name'] 获取姓名
        print(row['name'], row['age'], row['city'])  # 输出具体的字段值