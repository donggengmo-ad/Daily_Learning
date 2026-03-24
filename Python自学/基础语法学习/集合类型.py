#集合类型：列表，元组，集合，字典
#区别于基本数据类型，集合类型可以存储多个值，并且可以是不同类型的值
#集合类型通常是可变的，可以修改其中的元素（除了元组），而基本数据类型是不可变的

#列表：有序、可变、允许重复元素
my_list = [1, 2, 3, "hello", True,[4,5]]
print(my_list)
my_list.append("world")  # 添加元素
print(my_list)
my_list[0] = 0  # 修改元素
print(my_list)
my_list.remove(2)  # 删除元素
print(my_list)
print(my_list[0])  # 访问元素
print(my_list[1:4:2])  # 切片访问

#字典：无序、可变（键不可变）、键唯一、值可以重复
my_dict = {"name": "Alice",
           "age": 19, 
           "skills": ["Python", "Java", "C++"], 
           "likes": {"food": "pizza", "color": "blue"}
           }
print(my_dict)
my_dict["age"] = 21  # 修改元素
print(my_dict)
my_dict["country"] = "USA"  # 添加元素
print(my_dict)
del my_dict["likes"]  # 删除元素
print(my_dict)
print(my_dict["name"])  # 访问元素
#keyError是当访问一个不存在的键时抛出的错误，使用get方法可以返回None或者指定的默认值，而不会抛出错误
print(my_dict.get("age"))  # 访问元素，使用get方法可以避免KeyError
print(my_dict.get("Not found"))  # 访问不存在的键，返回默认值
print(len(my_dict))# 字典的长度是键的数量

#元组：有序、不可变、允许重复元素
my_tuple = (1, 2, 3, "hello", True)
print(my_tuple)
print(my_tuple[0])  # 访问元素
print(my_tuple[1:4:2])  # 切片访问
#元组解包是将元组中的元素分别赋值给多个变量，要求变量的数量和元组中的元素数量一致
a,b,c,d,e = my_tuple  
print(a,b,c,d,e)
#反之，多个变量赋值给一个元组时，称为元组打包，变量的数量必须和元组中的元素数量一致
tuple_pack = (a, b, c, d, e)
print(tuple_pack)
#巧妙利用元组
x, y = 1, 2  # 变量赋值时，右边的值会被自动打包成一个元组
print(x, y)
x, y = y, x  # 交换变量的值，右边的值会被自动打包成一个元组
print(x, y)

#集合：无序、可变、不允许重复元素
my_set = {1, 2, 3, "hello", True}
print(my_set)
my_set.add("world")  # 添加元素
print(my_set)
my_set.remove(2)  # 删除元素
print(my_set)
print(1 in my_set)  # 判断元素是否存在
print(-1 in my_set)  # 判断元素是否存在
#集合的数学运算
set_a = {1, 2, 3, 4}
set_b = {3, 4, 5, 6}
print(set_a.union(set_b))  # 并集
print(set_a.intersection(set_b))  # 交集
print(set_a.difference(set_b))  # 差集
print(set_a.symmetric_difference(set_b))  # 对称差集
#总结：
# 列表和字典是最常用的集合类型，
# 列表适合存储有序的数据，
# 字典适合存储键值对的数据，
# 元组适合存储不可变的数据，
# 集合适合存储不重复的数据。
