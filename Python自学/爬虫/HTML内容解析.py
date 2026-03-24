import requests

# HTML内容解析函数
from bs4 import BeautifulSoup


url = 'http://books.toscrape.com/' 
response = requests.get(url)  # 发送GET请求

if response.ok:
    #把html解析为树状结构
    soup = BeautifulSoup(response.text, 'html.parser')# 指定解析器
    # 解析网页内容
    all_prices = soup.find_all('p',class_='price_color')# 找到所有价格元素
    all_names = soup.find_all('a',title=True)# 找到所有书名元素
    # 打印书名和价格
    for name,price in zip(all_names,all_prices):
        print(name['title'], ":", price.text[2:])# 去掉£符号  
else:
    print("请求失败，状态码：", response.status_code)