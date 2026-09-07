import torch
from torch import nn
import torch.nn.functional as F
from .plt import set_axes
from IPython import display
from matplotlib import pyplot as plt
import numpy as np
import time
import collections
import typing

class Accumulator:
    """在 n 个变量上累加"""
    def __init__(self, n):
        self.data = [0.0] * n
    def add(self, *args):
        self.data = [a + float(b) for a, b in zip(self.data, args)]
    def reset(self):
        self.data = [0.0] * len(self.data)
    def __getitem__(self, idx):
        return self.data[idx]

class Animator:
    """在动画中绘制数据"""
    def __init__(self, xlabel=None, ylabel=None, legend=None, xlim=None,
                 ylim=None, xscale='linear', yscale='linear',
                 fmts=('-', 'm--', 'g-.', 'r:'), nrows=1, ncols=1,
                 figsize=(3.5, 2.5)):
        """初始化图表"""
        # 增量绘图
        if legend is None:
            legend = []
        self.fig, self.axes = plt.subplots(nrows, ncols, figsize=figsize)
        if nrows * ncols == 1:
            self.axes = [self.axes, ]
        # 设置坐标轴
        self.config_axes = lambda: set_axes(self.axes[0], xlabel, ylabel, xlim, ylim, xscale, yscale, legend)
        self.X, self.Y, self.fmts = None, None, fmts
        self.legend = legend
    def add(self, x, y):
        # 向图表中添加多个数据点
        if not hasattr(y, "__len__"):
            y = [y]
        n = len(y)
        if not hasattr(x, "__len__"):
            x = [x] * n
        if not self.X:
            self.X = [[] for _ in range(n)]
        if not self.Y:
            self.Y = [[] for _ in range(n)]
        for i, (a, b) in enumerate(zip(x, y)):
            if a is not None and b is not None:
                self.X[i].append(a)
                self.Y[i].append(b)
        self.axes[0].cla()
        for i, (x_arr, y_arr) in enumerate(zip(self.X, self.Y)):
            fmt = self.fmts[i % len(self.fmts)]
            self.axes[0].plot(x_arr, y_arr, fmt)
        self.config_axes()
        if self.legend:
            self.axes[0].legend(self.legend)
        display.display(self.fig)
        display.clear_output(wait=True)

class Timer:  #@save
    """记录多次运行时间"""
    def __init__(self):
        self.times = []
        self.start()

    def start(self):
        """启动计时器"""
        self.tik = time.time()

    def stop(self):
        """停止计时器并将时间记录在列表中"""
        self.times.append(time.time() - self.tik)
        return self.times[-1]

    def avg(self):
        """返回平均时间"""
        return sum(self.times) / len(self.times)

    def sum(self):
        """返回时间总和"""
        return sum(self.times)

    def cumsum(self):
        """返回累计时间"""
        return np.array(self.times).cumsum().tolist()

def try_gpu(i: int=0, mps: bool=True) -> torch.device:
    """尝试获取设备
    :param i: GPU 索引
    :param mps: 是否尝试使用 MPS（Metal Performance Shaders）
    :return: 可用的设备（GPU、MPS 或 CPU）
    """
    if torch.cuda.is_available() and torch.cuda.device_count() >= i + 1:
        return torch.device(f'cuda:{i}')
    if mps and torch.backends.mps.is_available():
        return torch.device('mps')
    return torch.device('cpu')

def try_all_gpus():
    """返回所有可用的GPU，如果没有GPU，则返回[cpu(),]"""
    devices = [torch.device(f'cuda:{i}')
             for i in range(torch.cuda.device_count())]
    return devices if devices else [torch.device('cpu')]


class Vocab:
    """文本词汇表"""

    def __init__(self, tokens=None, min_freq: int = 0, reserved_tokens=None):
        """通过传入的 token 列表构建词汇表
        :param tokens: 列表，包含所有文本的 token
        :param min_freq: token 的最小频率（频率低于该值的 token 会被忽略）
        :param reserved_tokens: 保留的 token 列表（BOS、EOS、PAD 等）
        """
        if tokens is None:
            tokens = []
        if reserved_tokens is None:
            reserved_tokens = []
        # 按出现频率排序
        counter = count_corpus(tokens)
        self.token_freqs = sorted(counter.items(),  # token-频率 对
                                   key=lambda x: x[1],  # 按频率排序
                                   reverse=True)  # 降序
        # 未知词元索引为 0
        self.unk = 0  # 未知词元的索引
        unique_tokens = ['<unk>'] + reserved_tokens
        # 按频率添加 token
        unique_tokens += [token for token, freq in self.token_freqs
                          if freq >= min_freq and
                          token not in unique_tokens]
        # 构建映射表
        self.idx_to_token, self.token_to_idx = [], dict()
        for token in unique_tokens:
            self.idx_to_token.append(token)
            self.token_to_idx[token] = len(self.idx_to_token) - 1

    def __len__(self):
        return len(self.idx_to_token)  # 词汇表大小

    def __getitem__(self, tokens):
        """获取 token 的索引"""
        if not isinstance(tokens, (list, tuple)):  # 单个 token
            return self.token_to_idx.get(tokens, self.unk)  # 返回索引，若 token 不在词汇表中则返回 unk 索引
        return [self.__getitem__(token) for token in tokens]  # token 列表（递归调用）

    def to_tokens(self, indices):
        """获取索引对应的 token"""
        if not isinstance(indices, (list, tuple)):  # 单个索引
            return self.idx_to_token[indices]
        return [self.idx_to_token[index] for index in indices]  # 索引列表


def tokenize(lines, token='word'):
    """将文本行拆分为单词或字符"""
    if token == 'word': # 按空格拆分
        return [line.split() for line in lines]
    elif token == 'char': # 按字符拆分
        return [list(line) for line in lines]
    else:
        print('错误：未知词元类型：' + token)

def count_corpus(tokens):
    """统计 token 的频率"""
    # tokens 是 1D 列表或 2D 列表
    if len(tokens) == 0 or isinstance(tokens[0], list):
        # 将所有子列表展平到一个列表中
        tokens = [token for line in tokens for token in line]
    return collections.Counter(tokens)


# RNN
def predict_ch8(prefix: str, num_preds: int, net, vocab: Vocab, device: torch.device):
    """根据前缀 prefix 生成后续的 num_preds 个字符
    :param prefix: 前缀字符串
    :param num_preds: 预测的字符数
    :param net: 循环神经网络模型
    :param vocab: 词表
    :param device: 设备
    :return: 生成的字符串
    """
    # 获取当前输入的函数
    get_input = lambda: torch.tensor([[outputs[-1]]], device=device).reshape(1, 1)  # 取最后一次输出
    # 将 prefix 转换为 token id 序列
    state = net.begin_state(batch_size=1, device=device)
    outputs = [vocab[prefix[0]]]  # 输出列表，先放入第一个字符的 id
    for y in prefix[1:]:  # 遍历前缀的剩余字符
        y_hat, state = net(get_input(), state)  # 前向计算，更新隐藏状态
        outputs.append(vocab[y])  # 将当前字符的 id 加入输出列表
    for y in range(num_preds):  # 生成后续字符
        y_hat, state = net(get_input(), state)  # 前向计算，更新隐藏状态
        outputs.append(int(y_hat.argmax(dim=1).reshape(1)))  # 取最大概率的字符 id 加入输出列表
    return ''.join([vocab.idx_to_token[i] for i in outputs])  # 将 id 转换为字符并拼接成字符串

def grad_clipping(net, theta: float):
    """裁剪梯度
    :param net: 循环神经网络模型
    :param theta: 阈值
    :param device: 设备
    """
    if isinstance(net, nn.Module):
        params = [p for p in net.parameters() if p.requires_grad] # 获取需要梯度的参数
    else:
        params = net.params # 如果是自定义模型，直接取 params
    # 计算梯度的 L2 范数，所有参数的梯度平方和开根
    norm = torch.sqrt(sum(torch.sum((p.grad ** 2)) for p in params))
    if norm > theta: # 如果范数超过阈值
        for param in params:
            # 按比例缩放梯度
            param.grad[:] *= theta / norm # [:] 表示 inplace