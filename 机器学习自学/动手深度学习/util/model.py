from torch import nn
from torch.nn import functional as F
import torch
import typing

# CNN
class Inception(nn.Module):
    def __init__(self, in_channels: int, c1: int, c2: tuple[int, int], c3: tuple[int, int], c4: int, **kwargs):
        """Inception 块
        :param in_channels: 输入通道数
        :param c1: 路径 1 的 1x1 卷积输出通道数
        :param c2: 路径 2 的两层通道数，c2[0] 是 1x1 卷积的输出通道数，c2[1] 是 3x3 卷积的输出通道数
        :param c3: 路径 3 的两层通道数，c3[0] 是 1x1 卷积的输出通道数，c3[1] 是 5x5 卷积的输出通道数
        :param c4: 路径 4 的 1x1 卷积输出通道数
        """
        super(Inception, self).__init__(**kwargs)
        # path 1
        self.p1_1 = nn.Conv2d(in_channels, c1, kernel_size=1) # 1x1 卷积
        # path 2
        self.p2_1 = nn.Conv2d(in_channels, c2[0], kernel_size=1) # 1x1 卷积
        self.p2_2 = nn.Conv2d(c2[0], c2[1], kernel_size=3, padding=1) # 3x3 卷积
        # path 3
        self.p3_1 = nn.Conv2d(in_channels, c3[0], kernel_size=1) # 1x1 卷积
        self.p3_2 = nn.Conv2d(c3[0], c3[1], kernel_size=5, padding=2) # 5x5 卷积
        # path 4
        self.p4_1 = nn.MaxPool2d(kernel_size=3, stride=1, padding=1) # 3x3 最大池化
        self.p4_2 = nn.Conv2d(in_channels, c4, kernel_size=1) # 1x1 卷积

    def forward(self, X: torch.Tensor) -> torch.Tensor:
        p1 = F.relu(self.p1_1(X))
        p2 = F.relu(self.p2_2(F.relu(self.p2_1(X))))
        p3 = F.relu(self.p3_2(F.relu(self.p3_1(X))))
        p4 = F.relu(self.p4_2(self.p4_1(X)))
        return torch.cat((p1, p2, p3, p4), dim=1) # 在通道维度上 concat

class Residual(nn.Module):
    def __init__(self, input_channels, num_channels, use_1x1conv=False, strides=1):
        """
        生成一个残差单元
        @param input_channels: 输入通道数
        @param num_channels: 输出通道数
        @param use_1x1conv: 是否使用 1x1 卷积（调整通道）
        @param strides: 第一个卷积层步幅
        """
        assert (input_channels == num_channels) or use_1x1conv, "输入输出通道数不一致，必须使用 1x1 卷积调整通道数"
        super().__init__()
        # 第一个卷积层（可调步幅），在这里把通道变成输出通道数
        self.conv1 = nn.Conv2d(input_channels, num_channels, kernel_size=3, padding=1, stride=strides)
        # 第二个卷积层不改变高宽和通道
        self.conv2 = nn.Conv2d(num_channels, num_channels, kernel_size=3, padding=1)
        if use_1x1conv:
            # 如果需要调整通道数（输入输出通道不一样），则使用 1x1 卷积层
            self.conv3 = nn.Conv2d(input_channels, num_channels, kernel_size=1, stride=strides)
        else:
            self.conv3 = None
        self.bn1 = nn.BatchNorm2d(num_channels)
        self.bn2 = nn.BatchNorm2d(num_channels)
        self.relu = nn.ReLU(inplace=True) # inplace=True 表示直接在原来的内存上进行操作，节省内存

    def forward(self, X):
        # 得到 g(x)
        Y = self.relu(self.bn1(self.conv1(X)))
        Y = self.bn2(self.conv2(Y))
        # 是否调整通道数
        if self.conv3:
            X = self.conv3(X)
        # 残差连接
        Y += X
        return self.relu(Y)

def resnet_block(input_channels, num_channels, num_residuals, first_block=False)->nn.Sequential:
    """
    生成一个残差块（由多个残差单元组成）
    @param input_channels: 输入通道数
    @param num_channels: 输出通道数
    @param num_residuals: 残差块的数量
    @param first_block: 是否是第一个残差块（第一个残差块不需要调整通道数和高宽）
    """
    blk = []
    for i in range(num_residuals):
        if i == 0 and not first_block:
            # 第一个残差块需要调整通道数和高宽
            blk.append(Residual(input_channels, num_channels, use_1x1conv=True, strides=2))
        else:
            blk.append(Residual(num_channels, num_channels))
    return nn.Sequential(*blk)

def resnet18(num_classes, in_channels=1):
    """使用了更小的卷积核、步长和填充，而且删除了最大汇聚层
    :param num_classes: 分类数
    :param in_channels: 输入通道数
    """
    net = nn.Sequential(
        nn.Conv2d(in_channels, 64, kernel_size=3, stride=1, padding=1),
        nn.BatchNorm2d(64),
        nn.ReLU()
    )
    net.add_module("resnet_block1", resnet_block(64, 64, 2, first_block=True))
    net.add_module("resnet_block2", resnet_block(64, 128, 2))
    net.add_module("resnet_block3", resnet_block(128, 256, 2))
    net.add_module("resnet_block4", resnet_block(256, 512, 2))
    net.add_module("global_avg_pool", nn.AdaptiveAvgPool2d((1,1)))
    net.add_module("fc", nn.Sequential(nn.Flatten(),
                                       nn.Linear(512, num_classes)))
    return net

class RNNModelScratch:
    """从零实现的循环神经网络模型"""

    def __init__(self,
                 vocab_size: int,
                 num_hiddens: int,
                 device: torch.device,
                 get_params: typing.Callable,
                 init_state: typing.Callable,
                 forward_fn: typing.Callable):
        """初始化循环神经网络模型
        :param vocab_size: 词表大小
        :param num_hiddens: 隐藏层大小
        :param device: 设备
        :param get_params: 获取参数的函数 (vocab_size, num_hiddens, device) -> params
        :param init_state: 初始化隐藏状态的函数 (batch_size, num_hiddens, device) -> state
        :param forward_fn: 前向计算的函数 (inputs, state, params) -> (outputs, state)
        """
        self.vocab_size, self.num_hiddens = vocab_size, num_hiddens
        self.params = get_params(vocab_size, num_hiddens, device)
        self.init_state, self.forward_fn = init_state, forward_fn

    def __call__(self, X: torch.Tensor, state: tuple):
        """前向计算
        :param X: 输入 token id 序列 (B, T)
        :param state: 隐藏状态 (H)
        :return: 输出竖拼接 (T*B, V), 隐藏状态 (H)
        """
        X = F.one_hot(X.T.long(), self.vocab_size).type(torch.float32)  # one-hot 编码成 (T, B, V)
        return self.forward_fn(X, state, self.params)  # 调用 rnn 函数计算输出和隐藏状态

    def begin_state(self, batch_size: int, device: torch.device):
        """初始化隐藏状态
        :param batch_size: 批量大小
        :param device: 设备
        :return: 隐藏状态 (H)
        """
        return self.init_state(batch_size, self.num_hiddens, device)

class RNNModel(nn.Module):
    """循环神经网络模型"""
    def __init__(self, rnn_layer, vocab_size: int):
        """
        :param rnn_layer: RNN 层
        :param vocab_size: 词表大小
        """
        super().__init__()
        self.rnn = rnn_layer
        self.vocab_size = vocab_size
        self.num_hiddens = rnn_layer.hidden_size
        # 如果是双向 RNN，则 num_directions = 2，否则为 1
        self.num_directions = 1 + rnn_layer.bidirectional
        # 输出层，将 RNN 的输出映射到词表大小的维度
        self.linear = nn.Linear(self.num_directions * self.num_hiddens, self.vocab_size)

    def forward(self, X: torch.Tensor, state: torch.Tensor):
        """前向计算
        :param X: 输入 token id 序列 (B, T)
        :param state: 隐藏状态 (num_layers * num_directions, B, H)
        :return: 输出竖拼接 (T*B, V), 隐藏状态 (num_layers * num_directions, B, H)
        """
        X = F.one_hot(X.T.long(), self.vocab_size).type(torch.float32) # one-hot 编码成 (T, B, V)
        Y, state = self.rnn(X, state) # 前向计算
        output = self.linear(Y.reshape(-1, Y.shape[-1])) # 输出层，(T*B, V)
        return output, state

    def begin_state(self, batch_size: int, device: torch.device):
        """初始化隐藏状态
        :param batch_size: 批量大小
        :param device: 设备
        :return: 隐藏状态 (num_layers * num_directions, B, H)
        """
        if not isinstance(self.rnn, nn.LSTM):
            # 如果是 RNN 或 GRU，返回全 0 的隐藏状态
            return torch.zeros((self.num_directions * self.rnn.num_layers, batch_size, self.num_hiddens), device=device)
        else:
            # 如果是 LSTM，返回全 0 的隐藏状态和细胞状态
            return (torch.zeros((self.num_directions * self.rnn.num_layers, batch_size, self.num_hiddens), device=device),
                    torch.zeros((self.num_directions * self.rnn.num_layers, batch_size, self.num_hiddens), device=device))