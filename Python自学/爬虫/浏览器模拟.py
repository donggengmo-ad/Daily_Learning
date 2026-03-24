from selenium import webdriver
import time

# 创建一个新的 Chrome 浏览器实例
driver = webdriver.Chrome('./chromedriver')

# 打开一个网页
driver.get('https://www.example.com')

time.sleep(5)  # 等待页面加载完成

html = driver.page_source
print(html)