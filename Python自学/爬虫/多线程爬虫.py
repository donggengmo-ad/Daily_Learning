import requests
import time
from multiprocessing import Pool

# 定义函数和URL
def q(url):
    requests.get(url)

url = "http://books.toscrape.com/"

if __name__ == "__main__":
    # 单线程计时
    start = time.time()
    print("正在进行单线程爬取...")

    for i in range(10):
        q(url)

    end = time.time()
    print(f"单线程总耗时：{end - start}")

    # 多线程计时

    urls = []
    for i in range(10):
        urls.append(url)

    start = time.time()
    print("正在进行多线程爬取...")

    with Pool(5) as p:
        p.map(q, urls)

    end = time.time()
    print(f"多线程总耗时：{end - start}")
    
##最终结果：多线程t 略微大于 单线程t/5
