import unittest

#一些用于测试程序问题的方法
#注：测试代码一般与正式代码分开，放在不同的文件中

#assert语句
#判断表达式的值是否为真，如果为假则抛出异常，并终止程序
assert 1 + 1 == 2, "1 + 1应该等于2"
#assert 1 + 1 == 3, "1 + 1应不等于3"  # 这行代码会抛出异常，因为表达式的值为假
#返回AssertError（断言错误）异常，并输出后面的字符串

#unittest模块
class TestMath(unittest.TestCase):#继承unittest.TestCase类，创建一个测试类
    def setUp(self):
        #在每个测试方法执行前都会调用这个方法，可以用来准备测试环境，比如创建对象、连接数据库等
        pass
    
    #测试方法必须以test_开头，否则不会被识别为测试方法
    def test_equal(self):
        self.assertEqual(1 + 1, 2, "1 + 1应该等于2")  # 判断两个值是否相等，如果不相等则抛出异常，并输出后面的字符串
    def test_not_equal(self):
        self.assertNotEqual(1 + 1, 3, "1 + 1应不等于3")  # 判断两个值是否不相等，如果相等则抛出异常，并输出后面的字符串
    def test_true(self):
        self.assertTrue(1 + 1 == 2, "1 + 1应该等于2")  # 判断表达式的值是否为真，如果为假则抛出异常，并输出后面的字符串
    def test_false(self):
        self.assertFalse(1 + 1 == 3, "1 + 1应不等于3")  # 判断表达式的值是否为假，如果为真则抛出异常，并输出后面的字符串
    def test_in(self):
        self.assertIn(1, [1, 2, 3], "1应该在列表中")  # 判断第一个值是否在第二个值中，如果不在则抛出异常，并输出后面的字符串
    def test_not_in(self):
        self.assertNotIn(4, [1, 2, 3], "4不应该在列表中")  # 判断第一个值是否不在第二个值中，如果在则抛出异常，并输出后面的字符串
    def test_is(self):
        a = [1, 2, 3]
        b = a
        self.assertIs(a, b, "a和b应该是同一个对象")  # 判断两个对象是否是同一个对象，如果不是则抛出异常，并输出后面的字符串
    def test_is_not(self):
        a = [1, 2, 3]
        b = [1, 2, 3]
        self.assertIsNot(a, b, "a和b不应该是同一个对象")  # 判断两个对象是否不是同一个对象，如果是则抛出异常，并输出后面的字符串
    #还有很多
        
#测试通过显示OK，测试失败显示AssertionError异常，并输出错误信息
#测试通过的方法显示为.，测试失败的方法显示为F，测试错误的方法显示为E

#运行测试的方法：
if __name__ == '__main__':
    unittest.main()
    #表示当这个文件被直接运行时，执行unittest.main()函数，运行所有的测试方法


