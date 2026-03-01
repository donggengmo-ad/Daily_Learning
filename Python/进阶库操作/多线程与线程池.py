import threading
from time import time
from concurrent import futures

def test(thread_name):
    print("线程{}正在运行".format(thread_name))

#普通多线程
start = time()

threads = [] #创建一个列表来保存线程对象
for i in range(5):
    #创建线程对象，target参数指定线程要执行的函数，args参数以元组形式传递函数参数
    t = threading.Thread(target=test, args=(i,)) #写逗号是为了让i成为一个单元素元组，否则会被当成普通变量
    t.start()
    #保存线程对象
    threads.append(t)

for t in threads:
    t.join() # 等待所有线程完成

end = time()

print("普通多线程耗时：{:.4f}秒".format(end - start)) # 保留4位小数


#线程池
# 线程池通过几个线程循环利用，加上一些临时线程来处理任务
# 避免频繁创建和销毁线程的开销，提高性能和资源利用率。

start = time()

with futures.ThreadPoolExecutor(max_workers=5) as executor: #创建线程池对象，最大线程数也可以不填
    #使用map方法将函数和参数映射到线程池中，自动分配线程执行
    executor.map(test, range(5)) 
    
end = time()

print("线程池耗时：{:.4f}秒".format(end - start)) # 保留4位小数
