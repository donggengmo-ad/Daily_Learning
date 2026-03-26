//# 文件头

//万能头（常用）
#include <bits/stdc++.h>

//命名空间，避免std::前缀
using namespace std;
//也可以用来定义常用类型的别名
using ll = long long; //定义ll为long long类型的别名
using pii = pair<int,int>; //定义pii为pair<int,int>类型的别名

int main(){ 

//# 流式输入输出

    //*标准用法
    //从标准输入读取两个整数
    int a,b;
    cin >> a >> b; 
    //输出它们的和到标准输出
    cout << a + b << endl; ///endl表示换行+刷新

    //*cin高级用法
    //读字符串,默认以空格为分隔符
    string s;
    cin >> s; 
    //读取一行字符串（包括空格）
    getline(cin, s);
    //读单个字符
    char c;
    cin.get(c);
    //读整数直到遇到非数字字符
    int num;
    cin >> num;

    //*cout高级用法
    //设置输出格式
    cout << fixed << setprecision(2); //设置浮点数输出为固定小数点格式，保留两位小数
    //输出多个变量
    int x = 10, y = 20;
    cout << "x=" << x << ", y=" << y << endl;
    //十六进制、八进制输出
    cout << hex << 255 << endl; //输出ff
    cout << oct << 255 << endl; //输出377

//# STL容器
    
    //*vector（动态数组）
    //定义动态数组
    vector<int> v1; 
    vector<int> v2(5); //定义一个包含5个元素的动态数组，初始值为0
    vector<int> v3(5, 10); //定义一个包含5个元素的动态数组，初始值为10
    //增删元素
    v1.push_back(10); //在末尾添加元素10
    v1.pop_back(); //删除末尾元素
    v1.clear(); //清空动态数组
    //访问元素
    v2[1] = 1;//下标访问
    v2.size(); //获取动态数组大小
    v2.empty(); //判断动态数组是否为空

    //*stack（栈）
    stack<int> s1; //定义一个整数栈
    //增删元素
    s1.push(10); //压入元素10
    s1.pop(); //弹出栈顶元素
    //访问
    s1.top(); //获取栈顶元素
    s1.empty(); //判断栈是否为空
    s1.size(); //获取栈大小

    //*queue（队列）
    queue<int> q1; //定义一个整数队列
    //增删元素
    q1.push(10); //入队元素10
    q1.pop(); //出队元素
    //访问
    q1.front(); //获取队首元素
    q1.back(); //获取队尾元素
    q1.empty(); //判断队列是否为空
    q1.size(); //获取队列大小

    //set（集合）
    set<int> s2; //定义一个整数集合
    //增删元素(自动排序、去重)
    s2.insert(10); //插入元素10
    s2.erase(10); //删除元素10
    //访问
    s2.count(10); //判断元素10是否存在于集合中(返回1或0)
    s2.size(); //获取集合大小
    s2.empty(); //判断集合是否为空
    s2.find(10); //查找元素10，返回迭代器

    //map（映射）
    map<int, string> m; //定义一个整数对字符串映射
    //增删元素
    m[1] = "10"; //插入或更新键值对(1,10)
    m.erase(1); //删除键为1的元素
    //访问
    m.count(1); //判断键1是否存在于映射中(返回1或0)
    m.size(); //获取映射大小
    m.empty(); //判断映射是否为空
    m.find(1); //查找键1，返回迭代器
    //也可以是其他映射
    map<string, vector<int>> m2; //定义一个字符串对整数动态数组的映射
    m2["key"].push_back(10); //在键为"key"的动态数组中添加元素10

    //*迭代器
    //迭代器是STL容器的通用访问方式，可以用于遍历、修改容器元素
    vector<int> v4 = {1, 2, 3, 4, 5};
    //使用迭代器遍历动态数组
    vector<int>::iterator it; //定义一个动态数组迭代器
    for(it = v4.begin(); it != v4.end(); ++it);
    for(auto it2 = v4.begin(); it2 != v4.end(); ++it2);
    for(int x : v4);

//#常用函数
    //排序
    sort(v1.begin(), v1.end()); //对动态数组v1进行升序排序
    sort(v1.rbegin(), v1.rend()); //对动态数组v1进行降序排序
    sort(v1.begin(), v1.end(), greater<int>()); //使用greater函数对象进行降序排序

    //反转
    reverse(v1.begin(), v1.end()); //反转动态数组v1的元素顺序

    //二分查找
    auto it1 = lower_bound(v1.begin(), v1.end(), 10); //在动态数组v1中查找第一个>=10的元素，返回迭代器
    auto it2 = upper_bound(v1.begin(), v1.end(), 10); //在动态数组v1中查找第一个>10的元素，返回迭代器

    //初始化
    memset(v2.data(), 0, v2.size() * sizeof(int)); //将动态数组v2的所有元素初始化为0

    //最值
    a = 1, b = 2;
    max(a,b);
    min(a,b);

    return 0;


}

