# 多元线性回归预测房价

# 特征如下
#// *date*,（成交日期，与房价无关）
#? price,（房价，目标变量）
#/ bedrooms,（卧室数量）
#/ bathrooms,（浴室数量）
#/ sqft_living,（居住面积）
#/ sqft_lot,（土地面积）
#/ floors,（楼层数量）
#// *waterfront*,（是否临水，过于稀疏）
#// *view*,（房屋视野质量，过于稀疏）
#/ condition,（房屋状况）
#/ sqft_above,（地上面积）
#/ sqft_basement,（地下室面积）
#/ yr_built,（建造年份）
#/ yr_renovated,（翻新年份）
#// *street*,（街道地址，全都不一样）
#/ city,（城市）
#/ statezip,（州和邮政编码）
#// *country*，（国家，全都一样）

from pyexpat import model
import pandas as pd
import torch
from torch import nn
from sklearn.metrics import mean_squared_error, mean_absolute_error, r2_score
from sklearn.model_selection import train_test_split
import os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
data_path_upper = os.path.join(BASE_DIR, 'data', 'kagglehub', 'houseprice')+'/'
model_path = os.path.join(BASE_DIR, 'linear_regression_model.pth')

def preprocess_data():
    
    # 读取数据
    data = pd.read_csv(data_path_upper + 'data.csv')

    # 删除无关特征
    data = data.drop(columns=['date', 'waterfront', 'view', 'street', 'country'])

    # 部分特征整数化
    data['city'] = data['city'].astype('category').cat.codes
    data['statezip'] = data['statezip'].astype('category').cat.codes

    # 获取特征
    features = data.columns.difference(['price'])

    # 特征标准化
    data[features] = (data[features] - data[features].mean()) / data[features].std()

    # 房价以万为单位
    data['price'] = data['price'] / 10000

    # 划分训练集和测试集
    train_data, test_data = train_test_split(data, test_size=0.15, random_state=42)
    train_data.to_csv(data_path_upper + 'train_data.csv', index=False)
    test_data.to_csv(data_path_upper + 'test_data.csv', index=False)

def load_data():
    train_data = pd.read_csv(data_path_upper + 'train_data.csv')
    test_data = pd.read_csv(data_path_upper + 'test_data.csv')
    return train_data, test_data

class LinearRegression(nn.Module):
    def __init__(self):
        super().__init__()
        # 输入特征数量为12，输出为1
        self.layer = nn.Linear(12, 1)
        
    def forward(self, x):
        # 将x传入线性层，进行线性变换
        #_ 相当于计算了线性回归方程h(x) = w1*x1 + w2*x2 + ... + w12*x12 + b
        return self.layer(x)
    
def print_model_parameters(model):
    # 打印模型参数
    print(model)
    print("")
    for name, param in model.named_parameters():
        print(f"{name}: {param.data}")
    print("")
    
def train_model():
    # 读取训练数据
    train_data, _ = load_data()
    
    # 获取特征列
    #? 语法：列表推导式，生成一个新的列表，包含train_data中所有列名，除了'price'
    features = [col for col in train_data.columns if col != 'price']
    
    # 输入至特征，转为张量x
    x = torch.tensor(train_data[features].values, dtype=torch.float32)
    # 输入至目标变量，转为张量y
    y = torch.tensor(train_data['price'].values, dtype=torch.float32).unsqueeze(1)  # 转为列向量
    
    # 创建模型实例
    model = LinearRegression()
    try:
        # 尝试加载之前训练好的模型参数
        model.load_state_dict(torch.load(model_path))  
    except FileNotFoundError:
        print("No pre-trained model found. Starting training from scratch.")
    # 设置模型为训练模式
    model.train()  
    
    # 定义损失函数和优化器
    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=0.01)
    
    # 开始训练
    for epoch in range(20000):   
        ## 经典五步训练流程
        # 输入至模型，得到预测结果
        h = model(x)
        # 计算损失
        loss = criterion(h, y)
        # 反向传播计算梯度
        loss.backward()
        # 更新模型参数
        optimizer.step()
        # 梯度清零
        optimizer.zero_grad()
        
        if (epoch + 1) % 1000 == 0:
            # 每1000轮打印一次损失
            print(f"Epoch [{epoch+1}/20000], Loss: {loss.item():.4f}")
            
            # 打印MSE,MAE,R2
            h_np = h.detach().numpy()
            y_np = y.detach().numpy()
            mse = mean_squared_error(y_np, h_np)
            mae = mean_absolute_error(y_np, h_np)
            r2 = r2_score(y_np, h_np)
            print(f"\tMean Squared Error: {mse:.4f}")
            print(f"\tMean Absolute Error: {mae:.4f}")
            print(f"\tR2 Score: {r2:.4f}")

    # 保存最终模型参数
    torch.save(model.state_dict(), model_path)
    print_model_parameters(model)

def evaluate_model():
    # 读取测试数据
    _, test_data = load_data()
    
    # 获取特征列
    features = [col for col in test_data.columns if col != 'price']
    
    # 输入至特征，转为张量x
    x = torch.tensor(test_data[features].values, dtype=torch.float32)
    # 输入至目标变量，转为张量y
    y = torch.tensor(test_data['price'].values, dtype=torch.float32).unsqueeze(1)  # 转为列向量
    
    # 创建模型实例并加载训练好的参数
    model = LinearRegression()
    model.load_state_dict(torch.load(model_path))
    model.eval()  # 设置模型为评估模式
    
    # 输入至模型，得到预测结果
    with torch.no_grad():
        h = model(x)
    
    # 打印MSE,MAE,R2
    h_np = h.detach().numpy()
    y_np = y.detach().numpy()
    mse = mean_squared_error(y_np, h_np)
    mae = mean_absolute_error(y_np, h_np)
    r2 = r2_score(y_np, h_np)
    print(f"Test Set Evaluation:")
    print(f"\tMean Squared Error: {mse:.4f}")
    print(f"\tMean Absolute Error: {mae:.4f}")
    print(f"\tR2 Score: {r2:.4f}")

def visualize_predictions():
    import matplotlib.pyplot as plt
    # 读取测试数据
    _, test_data = load_data()
    # 获取特征列
    features = [col for col in test_data.columns if col != 'price']
    # 输入至特征，转为张量x
    x = torch.tensor(test_data[features].values, dtype=torch.float32)
    # 输入至目标变量，转为张量y
    y = torch.tensor(test_data['price'].values, dtype=torch.float32).unsqueeze(1)  # 转为列向量
    # 创建模型实例并加载训练好的参数
    model = LinearRegression()
    model.load_state_dict(torch.load(model_path))
    model.eval()  # 设置模型为评估模式
    # 输入至模型，得到预测结果
    with torch.no_grad():
        h = model(x)
    # 可视化真实值与预测值
    plt.figure(figsize=(8, 6))
    plt.scatter(y.numpy(), h.numpy(), alpha=0.5)
    # 添加y=x参考线（理想情况下所有点应位于此线上）
    plt.plot([y.min(), y.max()], [y.min(), y.max()], 'r--') 
    # 设置坐标轴范围
    plt.xlim(0, 200)
    plt.ylim(0, 200)
    # 设置坐标轴标签和标题
    plt.xlabel('Actual Price')
    plt.ylabel('Predicted Price')
    plt.title('Actual vs Predicted Prices')
    plt.show()

if __name__ == '__main__':
    preprocess_data()
    # train_model()
    evaluate_model()
    visualize_predictions()
    
