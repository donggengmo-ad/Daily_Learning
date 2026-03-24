from openai import OpenAI
import os

# 初始化OpenAI客户端，使用环境变量中的API密钥
client = OpenAI(
    api_key=os.environ.get('DEEPSEEK_API_KEY'),
    base_url="https://api.deepseek.com"
)

# 输入提示词
prompt = input("请输入提示词：")

# 创建聊天
response = client.chat.completions.create(
    # 模型型号
    model="deepseek-chat",
    # 聊天消息列表
    messages=[
        # 系统消息，告诉模型它的角色和任务
        {"role": "system", "content": "扮演一位中国古代的文人，回答用户的问题时要使用古文的风格和表达方式。"},
        # 用户消息，告诉模型用户的需求和问题
        {"role": "user", "content": "你是谁？"},
        # 模型消息，存储上下文信息
        {"role": "assistant", "content": "吾乃古代文人也，专精诗词歌赋，乐于与人交流文学之道。"},
        # 用户消息，继续提问
        {"role": "user", "content": prompt}
    ],
    # 是否开启流式输出，开启后模型会逐步返回生成的内容
    stream = False,
    # 设置生成内容的最大长度，单位为token
    max_tokens = 1000,
    # 设置温度，决定生成内容的多样性
    #? 温度高时，让token的概率分布更平坦，更容易连接不常见的语句
    temperature = 1.4,
    # 设置top_p，决定生成内容的多样性，值越小越保守
    #? top_p高时，截取概率分布的前top_p部分，保留更多的候选token，生成内容更丰富
    top_p = 0.9,
    # 重复惩罚，值越大越惩罚重复的内容
    #? 统计词出现数，用于控制同次复现次数
    frequency_penalty = 1.4,
    # 存在惩罚，值越大越惩罚已经出现过的内容
    #? 统计词是否出现过，用于控制词多样性
    presence_penalty = 1.4,
)

# 输出模型的回复
print(response.choices[0].message.content)