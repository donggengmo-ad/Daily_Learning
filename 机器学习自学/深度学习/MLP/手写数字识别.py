# 用3层神经网络识别手写数字
# 输入28*28的图像，输出一个数字（0-9）

#? 1. 输入层：784个神经元（28*28）
#? 2. 隐藏层：256个神经元，激活函数ReLU（比较简单所以隐藏层神经元不用太多）
#? 3. 输出层：10个神经元，激活函数softmax

#! 28*28图像 -> 784维向量 -> 256维向量 -> 10维向量 -> 数字


#获取数据集
from torchvision import datasets, transforms
from torch.utils.data import DataLoader

def normalize():
    #数据预处理：将图像转换为张量，并进行归一化
    transform = transforms.Compose([#Compose可以将多个变换组合成一个变换
        #将图像转换为灰度图像，num_output_channels=1表示输出一个通道，如果是RGB图像则num_output_channels=3
        transforms.Grayscale(num_output_channels=1),
        transforms.Resize((28, 28)),#将图像调整为28*28大小
        transforms.ToTensor(),#将图像转换为PyTorch张量，像素值从[0,255]缩放到[0,1]
        transforms.Normalize((0.5,), (0.5,))#均值和标准差都是0.5，表示将像素值从[0,1]缩放到[-1,1]
    ])
    return transform

def load_data():    
    #数据预处理：将图像转换为张量，并进行归一化
    transform = normalize()

    #下载MNIST数据集
    # train=True表示训练集，train=False表示测试集
    # download=True表示如果数据集不存在则下载
    # transform=transform表示对数据进行预处理
    # MNIST数据集包含60000张训练图像和10000张测试图像，每张图像是28*28的灰度图像，标签是0-9的数字
    # 数据集保存在当前目录下的data文件夹中，如果没有则会自动创建
    # 用自己的数据集则用ImageFolder类
    train_dataset = datasets.MNIST(root='./机器学习/data', train=True, download=True, transform=transform)
    test_dataset = datasets.MNIST(root='./机器学习/data', train=False, download=True, transform=transform)

    #数据加载器
    #把数据分成小批次，每次训练一个批次，更新一次模型参数
    
    #batch_size表示每个批次的样本数量，shuffle=True表示在每个epoch结束后打乱数据
    train_loader = DataLoader(train_dataset, batch_size=64, shuffle=True) 
    print("读取训练样本：", len(train_loader),"组") 
    #测试集不需要打乱数据，所以shuffle=False
    test_loader = DataLoader(test_dataset, batch_size=64, shuffle=False)
    
    return train_loader, test_loader


#定义神经网络
import torch
from torch import nn

class NetWork(nn.Module): #nn库Moduled的子类
    #构造函数
    def __init__(self):
        super().__init__() #调用父类的构造函数
        #输入层到隐藏层的线性变换
        self.layer1 = nn.Linear(784, 256) 
        #隐藏层到输出层的线性变换
        self.layer2 = nn.Linear(256, 10) 
        
    #前向传播函数
    def forward(self,x):
        x = x.view(-1, 28*28) #将输入图像展平为784维向量,-1表示自动计算批量大小
        x = self.layer1(x) #x输入到layer1进行线性变换
        x = torch.relu(x) #使用ReLU激活函数
        x = self.layer2(x) #x输入到layer2进行线性变换
        return x
    
#训练模型
def train():
    #创建必要对象
    train_loader, test_loader = load_data() #加载数据集
    model = NetWork() #创建神经网络对象
    optimizer = torch.optim.Adam(model.parameters()) #使用Adam优化器，传入模型的参数
    criterion = nn.CrossEntropyLoss() #损失函数,交叉熵损失函数适用于多分类问题
    
    try:
        model.load_state_dict(torch.load("./机器学习/MLP/mnist.pth")) #加载模型参数
        print("模型参数加载成功，继续训练...")
    except FileNotFoundError:
        print("没有找到模型参数文件，开始训练新模型...")

    #训练模型
    #训练模型的过程:前向传播、计算损失、反向传播、更新模型参数、清零梯度
    
    for epoch in range(5): #外层循环：训练多少次
        #内层循环：遍历加载器中每个批次，data输入图像，target标签
        for batch_idx, (data, target) in enumerate(train_loader): 
            #前向传播：图像传入模型，得到输出
            output = model(data) #* 不用写model.forward(data),调用模型对象时会自动调用forward函数
            loss = criterion(output, target) #计算损失，输出和标签作为参数传入损失函数
            #反向传播：计算梯度，更新模型参数
            loss.backward() #* 不用写反向传播逻辑，optimizer会自动更新模型参数    
            optimizer.step() # 更新模型参数
            optimizer.zero_grad() # 清零梯度，准备下一次迭代
            
            #每100个批次打印一次损失
            if batch_idx % 100 == 0:
                print("Epoch: {}, Batch: {}/938, Loss: {:.4f}".format(epoch, batch_idx, loss.item()))

    torch.save(model.state_dict(), "./机器学习/MLP/mnist.pth") #保存模型参数到文件中

#测试模型
def test():
    #创建必要对象
    _, test_loader = load_data() #* 只需要测试集,"_"表示不需要的变量
    model = NetWork() #创建神经网络对象
    model.load_state_dict(torch.load("./机器学习/MLP/mnist.pth")) #加载模型参数
    model.eval() #* 设置模型为评估模式，关闭dropout和batchnorm等训练时特有的功能
    correct = 0 #正确预测的数量
    total = 0 #总的样本数量
    
    # 计算测试准确率（按批量处理）
    with torch.no_grad():#在测试阶段不需要计算梯度
        for i, (data, label) in enumerate(test_loader):  # 遍历测试集中的每个批次
            output = model(data)  # 前向传播，输出10维向量，表示每个类别的得分
            predicts = output.argmax(dim=1)  #* argmax(dim=1)表示在第1维上取最大值的索引，即预测的类别标签
            # 累加本批次预测正确的数量
            for predict, real in zip(predicts, label): #* zip函数将两个列表打包成一个元组列表
                if predict == real: #如果预测标签和真实标签相同
                    correct += 1 #正确预测数量加1
                total += 1 #总的样本数量加1
                       
    acc = 100 * correct / total #计算准确率
    print("测试准确率：{:.2f}%".format(acc)) #计算并打印测试准确率

#实际使用
import os
from PIL import Image

def predict():
    #创建必要对象
    model = NetWork() #创建神经网络对象
    model.load_state_dict(torch.load("./机器学习/MLP/mnist.pth")) #加载模型参数
    model.eval() #设置模型为评估模式
    
    #输入图片
    img = input("请输入图片路径：") #输入图片路径
    #数据预处理：将图像转换为张量，并进行归一化
    transform = normalize() #获取预处理函数
    img = transform(Image.open(img)) #打开图片
    
    output = model(img) #前向传播，得到输出
    predict = output.argmax(dim=1).item() #argmax(dim=1)表示在第1维上取最大值的索引，即预测的类别标签，item()将单元素张量转换为Python数值
    print("预测结果：", predict) #打印预测结果
    
    
if __name__ == "__main__":
    train() 
    test()
    predict()
    
    
