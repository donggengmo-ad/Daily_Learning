# 面向对象编程（OOP）是一种编程范式，强调将数据和操作数据的函数封装在一起形成对象。
# 通过对象的交互，来模拟现实世界中的实体和行为。

# 核心概念包括类、对象、封装、继承和多态。
# 1. 类（Class）：类是对象的蓝图或模板，定义了对象的属性和方法。
# 2. 对象（Object）：对象是类的实例
# 3. 封装（Encapsulation）：将数据和操作数据的函数绑定在一起，隐藏对象的内部实现细节，只暴露必要的接口。
# 4. 继承（Inheritance）：一个类可以继承另一个类的属性和方法，形成一个新的类。（内涵与外延）
# 5. 多态（Polymorphism）：指不同类的对象可以通过相同的接口调用方法，表现出不同的行为。

# 这种编程范式体现了柏拉图理念论式的思想，通过抽象和封装来实现对现实世界的建模和理解。


# 类名称通常使用大驼峰命名法（PascalCase）
class Student:
    # 构造函数：固定命名为__init__，区别于C++，Py在构造函数里定义对象的属性，而不是在类体里定义属性
    def __init__(self, name, age):
        self.name = name
        self.age = age
    # 对象方法必须首先传入self表示对象本身，调用时不用填入
    def study(self):
        print(self.name + " is studying.")
        
# 创建对象
Alex = Student("Alex", 20)
# 访问对象属性
print(Alex.name)  # 输出: Alex
print(Alex.age)   # 输出: 20
# 调用对象方法
Alex.study()      # 输出: Alex is studying.

# 继承示例
class CollegeStudent(Student):  # 写法：子类(父类)
    def __init__(self, name, age, major):
        # 调用父类的构造函数，super()表示调用父类的方法，括号里是子类的名字和self，可省略
        super().__init__(name, age)  
        self.major = major  # 新增属性：专业
    def study(self):
        print(self.name + " is studying " + self.major + ".")  # 重写覆盖父类的study方法
        
# 创建子类对象
Bob = CollegeStudent("Bob", 22, "Computer Science")
print(Bob.name)   # 输出: Bob
print(Bob.age)    # 输出: 22
print(Bob.major)  # 输出: Computer Science
Bob.study()       # 输出: Bob is studying Computer Science.

# 多态示例
def student_study(student):
    student.study()  # 调用study方法，表现出多态性
    
student_study(Alex)  # 输出: Alex is studying.
student_study(Bob)   # 输出: Bob is studying Computer Science.