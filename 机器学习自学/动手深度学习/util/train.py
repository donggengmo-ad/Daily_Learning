import torch
from jupyter_lsp import non_blocking

from .misc import *

def sgd(params, lr, batch_size):
    """小批量随机梯度下降
    :param params: 模型参数
    :param lr: 学习率
    :param batch_size: 批量大小
    """
    with torch.no_grad(): # 不需要计算梯度，直接下降
        for param in params:
            param -= lr * param.grad / batch_size
            param.grad.zero_() # 清空梯度

def accuracy(y_hat: torch.Tensor, y: torch.Tensor) -> float:
    """计算预测正确的数量
    :param y_hat: 预测值
    :param y: 真实值
    :return: 预测正确的数量
    """
    if len(y_hat.shape) > 1 and y_hat.shape[1] > 1:
        y_hat = y_hat.argmax(axis=1)  # 取最大值的索引
    cmp = y_hat.type(y.dtype) == y
    return float(cmp.type(y.dtype).sum())

def evaluate_accuracy(net, data_iter) -> float:
    """计算在指定数据集上模型的精度
    :param net: 模型
    :param data_iter: 数据迭代器
    :return: 模型分类正确数 / 分类总次数
    """
    if isinstance(net, torch.nn.Module):
        net.eval()  # 设置为评估模式
    metric = Accumulator(2)  # 正确预测数、预测总数
    for X, y in data_iter:
        metric.add(accuracy(net(X), y), y.numel())
    return metric[0] / metric[1]

def evaluate_accuracy_gpu(net, data_iter, device: torch.device | None = None)-> float | int:
    """使用指定设备，计算在数据集上模型的精度
    :param net: 模型
    :param data_iter: 数据迭代器
    :param device: 设备
    :return: 模型分类正确数 / 分类总次数
    """
    if isinstance(net, torch.nn.Module):
        net.eval()  # 设置为评估模式
        if not device:
            device = next(iter(net.parameters())).device # 获取模型参数所在的设备
    metric = Accumulator(2)  # 正确预测数、预测总数
    with torch.no_grad():
        for X, y in data_iter:
            if isinstance(X, list): # list 逐个挪
                X = [x.to(device) for x in X]
            else: # tensor 直接挪
                X = X.to(device)
            y = y.to(device)
            metric.add(accuracy(net(X), y), y.numel())
    # 分类正确数 / 分类总次数
    return metric[0] / metric[1]

def train_epoch_ch3(net, train_iter, loss: torch.nn.Module, updater)-> tuple[float, float]:
    """训练模型一个迭代周期
    :param net: 模型
    :param train_iter: 训练数据迭代器
    :param loss: 损失函数
    :param updater: 更新器
    :return: 训练损失、训练准确率
    """
    if isinstance(net, torch.nn.Module):
        net.train()  # 设置为训练模式
    metric = Accumulator(3)
    for X, y in train_iter:
        y_hat = net(X)
        l = loss(y_hat, y)
        if isinstance(updater, torch.optim.Optimizer):
            updater.zero_grad()
            l.mean().backward()  # 反向传播计算梯度
            updater.step()
            metric.add(float(l) * len(y), accuracy(y_hat, y), y.size().numel())
        else:
            l.sum().backward()  # 反向传播计算梯度
            updater(X.shape[0])
            metric.add(float(l.sum()), accuracy(y_hat, y), y.numel())
    # 返回训练损失、正确率
    return metric[0] / metric[2], metric[1] / metric[2]

def train_ch3(net,
              train_iter,
              test_iter,
              loss: torch.nn.Module ,
              num_epochs: int,
              updater):
    """训练模型
    :param net: 模型
    :param train_iter: 训练数据迭代器
    :param test_iter: 测试数据迭代器
    :param loss: 损失函数
    :param num_epochs: 训练轮数
    :param updater: 更新器
    """
    animator = Animator(xlabel='epoch', xlim=[1, num_epochs], ylim=[0.3, 1.0],
                        legend=['train loss', 'train acc', 'test acc'])
    for epoch in range(num_epochs):
        train_metrics = train_epoch_ch3(net, train_iter, loss, updater)
        test_acc = evaluate_accuracy(net, test_iter)
        animator.add(epoch + 1, train_metrics + (test_acc,))
        train_loss, train_acc = train_metrics


def train_ch6(net,
              train_iter,
              test_iter,
              num_epochs: int,
              lr: float=0.01,
              device: torch.device=try_gpu(),
              loss: torch.nn.Module=torch.nn.CrossEntropyLoss(),
              ylim: tuple[float, float]|None=None):
    """训练模型，支持 GPU
    :param net: 模型
    :param train_iter: 训练数据迭代器
    :param test_iter: 验证数据迭代器
    :param num_epochs: 训练轮数
    :param lr: 学习率
    :param device: 设备
    :param loss: 损失函数
    :param ylim: 训练图中 y 轴（损失与精度）范围
    """
    def init_weights(m):
        """对线性层、卷积层进行 xavier 初始化"""
        if type(m) == torch.nn.Linear or type(m) == torch.nn.Conv2d:
            torch.nn.init.xavier_uniform_(m.weight)
            if m.bias is not None:
                torch.nn.init.zeros_(m.bias)
    net.apply(init_weights)
    print('training on', device)
    net.to(device)
    optimizer = torch.optim.SGD(net.parameters(), lr=lr)
    # 实时画图
    animator = Animator(xlabel='epoch', xlim=[1, num_epochs], ylim=ylim,
                        legend=['train loss', 'train acc', 'test acc'])
    timer, num_batches = Timer(), len(train_iter)
    for epoch in range(num_epochs):
        # 训练损失、训练准确率、样本数
        metric = Accumulator(3)
        net.train()
        for i, (X, y) in enumerate(train_iter):
            timer.start()
            optimizer.zero_grad()
            # 输入输出张量移到设备
            X, y = X.to(device), y.to(device)
            # 计算梯度并更新参数
            y_hat = net(X)
            l = loss(y_hat, y)
            l.backward()
            optimizer.step()
            # 累加训练损失、训练准确率、样本数
            metric.add(l * X.shape[0], accuracy(y_hat, y), X.shape[0])
            timer.stop()
            # 计算训练损失、训练准确率、测试准确率
            train_l = metric[0] / metric[2]
            train_acc = metric[1] / metric[2]
            if (i + 1) % (num_batches // 5) == 0 or i == num_batches - 1:
                animator.add(epoch + (i + 1) / num_batches,
                             (train_l, train_acc, None))
        test_acc = evaluate_accuracy_gpu(net, test_iter, device)
        animator.add(epoch + 1, (train_l, train_acc, test_acc))
    print(f'loss {train_l:.3f}, train acc {train_acc:.3f}, test acc {test_acc:.3f}')
    print(f'{metric[2] * num_epochs / timer.sum():.1f} examples/sec on {str(device)}')

def train_batch_ch13(net
                     , X, y,
                     loss: torch.nn.Module,
                     trainer: torch.optim.Optimizer,
                     devices: list[torch.device]):
    """训练模型一个批量，支持并行训练
    :param net: 模型
    :param X: 输入
    :param y: 标签
    :param loss: 损失函数
    :param trainer: 优化器
    :param devices: 设备列表，devices[0] 为主设备
    :return: 训练损失、训练准确率
    """
    if isinstance(X, list):
        # 多输入时，将每个输入复制到 devices[0]
        X = [x.to(devices[0], non_blocking=True) for x in X]
    else:
        X = X.to(devices[0], non_blocking=True)
    # 使用异步拷贝
    y = y.to(devices[0], non_blocking=True)
    net.train()
    trainer.zero_grad()
    pred = net(X)
    l = loss(pred, y)
    l.sum().backward()
    trainer.step()
    train_loss = l.sum()
    train_acc = accuracy(pred, y)
    return train_loss, train_acc

def train_ch13(net,
               train_iter,
               test_iter,
               loss: torch.nn.Module,
               trainer: torch.optim.Optimizer,
               num_epochs: int,
               devices: list[torch.device]=try_all_gpus()):
    """训练模型，支持并行训练
    :param net: 模型
    :param train_iter: 训练数据迭代器
    :param test_iter: 测试数据迭代器
    :param loss: 损失函数
    :param num_epochs: 训练轮数
    :param trainer: 优化器
    :param devices: 设备列表，devices[0] 为主设备
    """
    timer, num_batches = Timer(), len(train_iter)
    animator = Animator(xlabel='epoch', xlim=[1, num_epochs], ylim=[0, 1],
                            legend=['train loss', 'train acc', 'test acc'])
    # 使用 DataParallel 将模型复制到多个 GPU 上
    net = torch.nn.DataParallel(net, device_ids=devices).to(devices[0])
    for epoch in range(num_epochs):
        # 4个维度：储存训练损失，训练准确度，实例数，特点数
        metric = Accumulator(4)
        for i, (features, labels) in enumerate(train_iter):
            timer.start()
            l, acc = train_batch_ch13(
                net, features, labels, loss, trainer, devices)
            metric.add(l, acc, labels.shape[0], labels.numel())
            timer.stop()
            if (i + 1) % (num_batches // 5) == 0 or i == num_batches - 1:
                animator.add(epoch + (i + 1) / num_batches,
                             (metric[0] / metric[2], metric[1] / metric[3],
                              None))
        test_acc = evaluate_accuracy_gpu(net, test_iter)
        animator.add(epoch + 1, (None, None, test_acc))
    print(f'loss {metric[0] / metric[2]:.3f}, train acc '
          f'{metric[1] / metric[3]:.3f}, test acc {test_acc:.3f}')
    print(f'{metric[2] * num_epochs / timer.sum():.1f} examples/sec on '
          f'{str(devices)}')