import tkinter as tk
import requests
import webbrowser
from bs4 import BeautifulSoup


def get_links():
    url = input()
    #防止抓取失败
    if not url:
        print("抓取失败！")
        return
    try:
        #发送请求，5秒连不上放弃
        resp = requests.get(url,timeout=5)
        if resp.ok:
            print(resp.text)
            soup = BeautifulSoup(resp.text,'html.parser')
            video_list = soup.find_all('source') #找到所有source标签，视频链接通常在source标签的src属性中
            print("共找到{}个视频链接！".format(len(video_list)))
            for video in video_list:
                if video.get('type') == 'video/mp4': #只提取mp4格式的视频链接
                    video_url = video.get('src')
                    if video_url:
                        print("视频链接：", video_url)
        else:
            print("抓取失败！状态码：", resp.status_code)
    except Exception as e:
        print("抓取失败！",e)
        
get_links()
    