import requests
from bs4 import BeautifulSoup

#浏览每个页面
for start_num in range(0,250,25):

    # 豆瓣电影Top250爬虫示例
    url = "https://movie.douban.com/top250?start={}&filter=".format(start_num)
    # 直接请求回返回418，说它是一个茶壶

    # 解决方法：添加User-Agent头，模拟浏览器访问
    # 去浏览器随便找个网页，用检查功能，找浏览器发送的请求头，抄过来
    headers = {
        "User-Agent":"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/145.0.0.0 Safari/537.36 Edg/145.0.0.0"
    }

    response = requests.get(url, headers=headers)

    if response.ok:
        soup = BeautifulSoup(response.text, 'html.parser')
        # 解析电影信息
        movies = soup.find_all('div', class_='item')
        for movie in movies:
            all_title = movie.find_all('span', class_='title')
            all_rating = movie.find_all('span', class_='rating_num')
            for (title, rating) in zip(all_title, all_rating):
                print("{title} - 评分: {rating}".format(title=title.text, rating=rating.text))
    else:
        print("请求失败，状态码：", response.status_code)