from torch import nn
import torch
from util.data import load_array
from util.plt import plot
import pandas as pd
import numpy as np

def get_net(in_features: int,
            kind: str ='linear',
            layers: list|None = None)->nn.Module:
    """
    获取模型
    :param in_features: 输入特征数
    :param kind: 模型类型，'linear' 或 'mlp'
    :param layers: 隐藏层神经元数列表，默认 [256]
    :return: 模型
    """
    if layers is None and kind == 'mlp':
        layers = [256]
    net = None
    # 线性回归
    if kind == 'linear':
        net = nn.Linear(in_features, 1)
    # 多层感知机
    elif kind == 'mlp':
        dims = [in_features] + layers + [1]
        net = nn.Sequential()
        for i in range(len(dims) - 1):
            net.add_module(f'linear_{i}', nn.Linear(dims[i], dims[i+1]))
            if i < len(dims) - 2:
                net.add_module(f'relu_{i}', nn.ReLU())
        def xavier(m):
            """xavier 初始化，均值为 0，方差为 2/(n_in+n_out)"""
            if type(m) == nn.Linear:
                nn.init.xavier_uniform_(m.weight)
        net.apply(xavier)
    net = net.to(device=torch.device('cuda' if torch.cuda.is_available() else 'cpu'))
    return net

def train_ones(net,
          train_features, train_labels,
          test_features, test_labels,
          num_epochs, batch_size,
          loss, display_loss,
          learning_rate: float = 0.01,
          weight_decay: float | int = 0):
    # 记录训练集和测试集的误差
    train_ls, test_ls = [], []
    # 训练集迭代器和优化器
    train_iter = load_array((train_features, train_labels), batch_size)
    optimizer = torch.optim.Adam(net.parameters(), lr=learning_rate, weight_decay=weight_decay)
    for epoch in range(num_epochs):
        for X, y in train_iter:
            optimizer.zero_grad()
            # 这里用 MSELoss 计算损失，log_rmse 只在记录误差时使用
            l = loss(net(X), y)
            l.backward()
            optimizer.step()
        train_ls.append(display_loss(net, train_features, train_labels))
        if test_labels is not None:
            test_ls.append(display_loss(net, test_features, test_labels))
    return train_ls, test_ls

def get_k_fold_data(k, i, X, y):
    assert k > 1
    fold_size = X.shape[0] // k # 每折的大小
    X_train, y_train = None, None
    for j in range(k):
        idx = slice(j * fold_size, (j + 1) * fold_size) # slice(start, stop)
        X_part, y_part = X[idx, :], y[idx]
        if j == i: # 第 i 折作为验证集
            X_valid, y_valid = X_part, y_part
        elif X_train is None: # 其他折作为训练集，第一次创建
            X_train, y_train = X_part, y_part
        else: # 其他折作为训练集，后续拼接
            X_train = torch.cat((X_train, X_part), dim=0)
            y_train = torch.cat((y_train, y_part), dim=0)
    return X_train, y_train, X_valid, y_valid

def k_fold(k,
           X_train, y_train,
           num_epochs, batch_size,
           loss, display_loss,
           learning_rate, weight_decay,
           in_features,
           kind, layers):
    train_l_sum, valid_l_sum = 0, 0 # 记录训练集和验证集的误差
    for i in range(k):
        # 创建数据和模型
        data = get_k_fold_data(k, i, X_train, y_train)
        net = get_net(in_features=in_features, kind=kind, layers=layers)
        # 训练并记录误差
        train_ls, valid_ls = train_ones(net, *data, num_epochs, batch_size, loss, display_loss, learning_rate, weight_decay)
        train_l_sum += train_ls[-1] # 记录最后一个 epoch 的误差
        valid_l_sum += valid_ls[-1]
        # 绘制第一个折的训练和验证误差曲线
        if i == 0:
            plot(list(range(1, num_epochs + 1)), [train_ls, valid_ls],
                      xlabel='epoch', ylabel='log rmse', xlim=[1, num_epochs],
                      legend=['train', 'valid'], yscale='log')
        # 打印每折的误差
        print(f'fold {i + 1}, train log rmse {float(train_ls[-1]):f}, '
              f'valid log rmse {float(valid_ls[-1]):f}')
    # 返回平均误差
    return train_l_sum / k, valid_l_sum / k

def loop_tune(train_attempt, supparams, loss, display_loss):
    for lr in supparams['lr']:
        for weight_decay in supparams['weight_decay']:
            for kind in supparams['kind']:
                if kind == 'linear':
                    train_attempt(loss, display_loss, lr, weight_decay, kind)
                else:
                    for layers in supparams['layers']:
                        train_attempt(loss, display_loss, lr, weight_decay, kind, layers)

def tunning(train_features, train_labels,
            k: int, supparams: dict,
            num_epochs, batch_size,
            loss, display_loss,
            in_features):
    records = pd.DataFrame(columns=['lr', 'weight_decay', 'kind', 'layers', 'train_log_rmse', 'valid_log_rmse'])
    # 定义训练尝试函数
    def train_attempt(loss, display_loss, lr, weight_decay, kind, layers=None):
        print(f'lr={lr}, weight_decay={weight_decay}, kind={kind}, layers={layers}')
        train_l, valid_l = k_fold(k=k,
                                  X_train=train_features,
                                  y_train=train_labels,
                                  num_epochs=num_epochs,
                                  batch_size=batch_size,
                                  loss=loss,
                                  display_loss=display_loss,
                                  learning_rate=lr,
                                  weight_decay=weight_decay,
                                  kind=kind,
                                  layers=layers,
                                  in_features=in_features)
        print(f'{k}-折验证: 平均训练 log rmse {float(train_l):f}, 平均验证 log rmse {float(valid_l):f}\n')
        records.loc[len(records)] = [lr, weight_decay, kind, layers, train_l, valid_l]
    # 循环训练
    loop_tune(train_attempt, supparams, loss, display_loss)
    return records

def train_and_pred(train_features, test_features,
                   train_labels, test_data,
                   num_epochs, batch_size,
                   loss, display_loss,
                   lr, weight_decay,
                   kind, layers,
                   keep_col: list,
                   target_col: str,
                   path='./results/prediction.csv'):
    net = get_net(kind=kind, layers=layers, in_features=train_features.shape[1])
    train_ls, _ = train_ones(net=net,
                             train_features=train_features,
                             train_labels=train_labels,
                             test_features=None,
                             test_labels=None,
                             num_epochs=num_epochs,
                             batch_size=batch_size,
                             loss=loss,
                             display_loss=display_loss,
                             learning_rate=lr,
                             weight_decay=weight_decay)
    print(f'训练 log rmse {float(train_ls[-1]):f}')
    # 预测
    preds = net(test_features).cpu().detach().numpy()
    # 将预测结果写入文件
    test_data[target_col] = pd.Series(preds.reshape(1, -1)[0])
    submission = pd.concat([test_data[keep_col], test_data[target_col]], axis=1)
    submission.to_csv(path, index=False)
