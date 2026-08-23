from torch import nn
from torch.nn import functional as F
import torch

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
