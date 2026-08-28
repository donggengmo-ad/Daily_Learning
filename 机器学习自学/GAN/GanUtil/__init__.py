import typing
from matplotlib import pyplot as plt
from matplotlib_inline import backend_inline
from torch import nn
import torch
import pandas as pd
import numpy as np

backend_inline.set_matplotlib_formats('svg')


def show_images(images: list[np.ndarray | torch.Tensor],
                rows: int=1,
                color: str='Blues',
                figsize: tuple=(10, 10),
                alpha: float=0.9,
                save_path: str | None=None):
    """
    画出多张图片
    :param images: 图片列表
    :param rows: 行数
    :param color: 颜色映射
    :param figsize: 图像大小
    :param alpha: 透明度
    :param save_path: 保存路径
    """
    cols = len(images) // rows + int(len(images) % rows != 0)
    plt.figure(figsize=figsize)
    # RGB图 重新归一化到 [0, 1]
    if images[0].shape[0] == 3:
        images = [(image + 1) / 2 for image in images]
    for i, image in enumerate(images):
        plt.subplot(rows, cols, i + 1)
        # [C, H, W] -> [H, W, C]，适应格式
        image = image.transpose(1, 2, 0) if isinstance(image, np.ndarray) else image
        image = image.permute(1, 2, 0) if isinstance(image, torch.Tensor) else image
        plt.imshow(image, cmap=color, alpha=alpha)
        plt.axis('off')

    plt.tight_layout()
    if save_path:
        plt.savefig(save_path, dpi=300)
    plt.show()

def show_tensor_images(images: torch.Tensor,
                       rows: int=1,
                       color: str='Blues',
                       figsize: tuple=(10, 10),
                       alpha: float=0.9,
                       save_path: str | None=None):
    """
    画出多张张量图片
    :param images: 图片张量，形状为 (N, C, H, W)
    :param rows: 行数
    :param color: 颜色映射
    :param figsize: 图像大小
    :param alpha: 透明度
    :param save_path: 保存路径
    """
    # 将张量分层，搬到cpu，去除梯度，转换为 numpy 数组
    show_images([img.cpu().detach().numpy() for img in images],
                rows=rows,
                color=color,
                figsize=figsize,
                alpha=alpha,
                save_path=save_path)

class Discriminator(nn.Module):
    """鉴别器"""
    def __init__(self,
                 net: nn.Module,
                 loss: nn.Module=nn.BCEWithLogitsLoss(),
                 lr: float | int=2e-4,
                 wd: float | int=1e-5,
                 betas: tuple[float, float]=(0.5, 0.9)):
        """
        :param net: 鉴别器网络
        :param lr: 学习率
        :param wd: 权重衰减
        """
        super().__init__()
        self.net = net
        self.loss = loss
        self.optimizer = torch.optim.Adam(self.net.parameters(),
                                          lr=lr,
                                          weight_decay=wd,
                                          betas=betas)
        self.counter = 0
        self.progress = []

    def forward(self, x):
        return self.net(x)

    def fit(self, x, y, verbose: bool=False):
        """进行一次训练
        :param x: 输入数据
        :param y: 真实标签
        :param verbose: 是否打印训练信息
        """
        # 输入可能来自 DataLoader（总是 CPU 张量）等外部来源，
        # 统一搬到网络所在设备，避免与 CUDA/MPS 上的模型参数不匹配
        device = next(self.net.parameters()).device
        x, y = x.to(device), y.to(device)
        y_hat = self(x)
        l = self.loss(y_hat, y)
        self.optimizer.zero_grad()
        l.backward()
        self.optimizer.step()
        self.counter += 1
        if self.counter % 10 == 0:
            self.progress.append(l.item())
        if self.counter % 1000 == 0 and verbose:
            print(f'Discriminator step {self.counter}, loss {l.item()}')

    def plot_progress(self, save_path: str | None=None):
        df = pd.DataFrame({'loss': self.progress})
        ax = df.plot(title='Discriminator Loss Progress', alpha=0.1, marker='.', grid=True)
        if save_path:
            ax.get_figure().savefig(save_path, dpi=600)

class Generator(nn.Module):
    """生成器"""
    def __init__(self,
                 net: nn.Module,
                 # 默认 lr 比鉴别器大s，因为一轮训练中鉴别器训练了两次，生成器只训练了一次
                 lr: float | int=2e-4,
                 wd: float | int=1e-5,
                 betas: tuple[float, float]=(0.5, 0.9)):
        """
        :param net: 生成器网络
        :param lr: 学习率
        :param wd: 权重衰减
        """
        super().__init__()
        self.net = net
        self.optimizer = torch.optim.Adam(self.net.parameters(),
                                          lr=lr,
                                          weight_decay=wd,
                                          betas=betas)
        self.counter = 0
        self.progress = []

    def forward(self, x):
        return self.net(x)

    def fit(self, x, y, D: Discriminator, verbose: bool=False):
        """进行一次训练
        :param x: 输入数据
        :param y: 真实标签
        :param D: 鉴别器
        :param verbose: 是否打印训练信息
        """
        self.train()
        # 与 Discriminator.fit 同理，先把输入搬到生成器所在设备
        device = next(self.net.parameters()).device
        x, y = x.to(device), y.to(device)
        gen = self(x)
        y_hat = D(gen)
        l = D.loss(y_hat, y)
        self.optimizer.zero_grad()
        l.backward()
        self.optimizer.step()
        self.counter += 1
        if self.counter % 10 == 0: # 注意因为一轮训练“一真一假”，偶数每次记录的都是假数据loss
            self.progress.append(l.item())
        if self.counter % 1000 == 0 and verbose:
            print(f'Generator step {self.counter}, loss {l.item()}')

    def plot_progress(self, save_path: str | None=None):
        df = pd.DataFrame({'loss': self.progress})
        ax = df.plot(title='Generator Loss Progress', alpha=0.1, marker='.', grid=True)
        if save_path:
            ax.get_figure().savefig(save_path, dpi=600)

    def generate(self, x):
        """生成样本"""
        self.eval()
        device = next(self.net.parameters()).device
        x = x.to(device).detach()
        return self(x)

def right(batch_size: int = 1, device=torch.device('cpu'), smooth: float = 0.9):
    """返回全1张量，表示真实样本标签，并进行标签平滑处理"""
    return torch.full((batch_size, 1), smooth, dtype=torch.float32, device=device)

def wrong(batch_size: int = 1, device=torch.device('cpu'), smooth: float = 0.1):
    """返回全0张量，表示生成样本标签，并进行标签平滑处理"""
    return torch.full((batch_size, 1), smooth, dtype=torch.float32, device=device)

def get_device(i: int=0):
    """获取设备
    :param i: 设备索引，默认使用第一个 GPU，如果没有 GPU，则使用 CPU
    :return: 设备
    """
    if torch.cuda.is_available():
        return torch.device(f'cuda:{i}')
    elif torch.backends.mps.is_available():
        return torch.device('mps')
    else:
        return torch.device('cpu')

def train_gan_generative(D: Discriminator,
                         G: Generator,
                         real_source: typing.Callable,
                         seed: typing.Callable,
                         num_steps: int=10000,
                         device: torch.device=get_device(),
                         show_evolve: bool=False):
    """用生成函数训练 GAN
    :param D: 鉴别器
    :param G: 生成器
    :param real_source: 真实样本生成函数
    :param seed: 生成器种子函数
    :param num_steps: 训练步数
    :param device: 设备
    :param show_evolve: 是否显示生成器演化过程
    """
    img_list = []
    for i in range(num_steps):
        # 真实样本训练鉴别器
        D.fit(real_source().to(device), right(device=device))
        # 生成样本训练鉴别器，detach 防止梯度回传到生成器
        D.fit(G(seed(device=device)).detach(), wrong(device=device))
        # 训练生成器
        G.fit(seed(device=device), right(device=device), D)
        if show_evolve and i % 1000 == 0:
            img_list.append(G.generate(seed()).detach().cpu())
    if show_evolve:
        plt.imshow(np.array(img_list), cmap='Blues')
    return img_list

def train_gan_dataloader(D: Discriminator,
                         G: Generator,
                         dataloader: typing.Iterable,
                         seed: typing.Callable,
                         num_epochs: int=10,
                         device: torch.device=get_device(),
                         show_evolve: bool=False,
                         evolve_interval: int=50,
                         get_epoch_tensor: bool=True,
                         save_path: str | None=None,
                         checkpoint_path: str | None=None,
                         checkpoint_interval: int=1):
    """用数据加载器训练 GAN
    :param D: 鉴别器
    :param G: 生成器
    :param dataloader: 数据加载器
    :param seed: 生成器种子函数
    :param num_epochs: 训练轮数
    :param device: 设备
    :param show_evolve: 是否显示生成器演化过程
    :param evolve_interval: 显示生成器演化过程的间隔步数
    :param get_epoch_tensor: 是否返回每轮训练后生成器生成的样本张量
    :param save_path: 保存生成器进度图的路径
    :param checkpoint_path: 保存 checkpoint 的路径
    :param checkpoint_interval: 保存 checkpoint 的间隔轮数
    """
    img_list = []
    epoch_tensor = []
    batch_size = dataloader.batch_size
    scheduler_D = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer=D.optimizer, T_max=num_epochs)
    scheduler_G = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer=G.optimizer, T_max=num_epochs)
    def train_batch(data, j):
        # 真样本批 -> D
        D.fit(data,
              right(batch_size=batch_size, device=device))
        # 假样本批 -> D -x G
        D.fit(G(seed(batch_size=batch_size, device=device)).detach(),
              wrong(batch_size=batch_size, device=device))
        # G -> 生成批 -> D
        G.fit(seed(batch_size=batch_size, device=device),
              right(batch_size=batch_size, device=device), D)
        if (j + 1) % 100 == 0:
            print(f'Epoch {i + 1}, Step {j + 1}, D Loss: {D.progress[-1]:.3f}, G Loss: {G.progress[-1]:.3f}')
        if show_evolve and (j + 1) % evolve_interval == 0:
            img_list.append(G.generate(seed(batch_size=1, device=device))[0][0].flatten().detach().cpu())

    for i in range(num_epochs):
        for j, (data, _label) in enumerate(dataloader):
            train_batch(data, j)
        scheduler_D.step()
        scheduler_G.step()
        if get_epoch_tensor:
            epoch_tensor.append(G.generate(seed(batch_size=1, device=device)).squeeze(0).detach().cpu())
        if checkpoint_path:
            save_checkpoint(D, G,
                            scheduler_D=scheduler_D,
                            scheduler_G=scheduler_G,
                            path=checkpoint_path,
                            epoch=i + 1,
                            extra={'epoch_tensor': epoch_tensor,
                                   'img_list': img_list})
    if show_evolve:
        plt.imshow(np.array(img_list), cmap='Blues')
    if save_path:
        plt.savefig(save_path, dpi=300)
    return torch.stack(epoch_tensor, dim=0) if get_epoch_tensor else None


def save_checkpoint(D: Discriminator,
                    G: Generator,
                    path: str,
                    epoch: int | None = None,
                    scheduler_D=None,
                    scheduler_G=None,
                    extra: dict | None = None):
    """保存训练 checkpoint，用于断点续训。

    保存内容：D/G 权重、优化器状态、D/G 的 counter 与 loss 进度（便于 plot_progress 续画），
    以及可选的 epoch 号、LR 调度器状态、任意附加数据（如 epoch_tensor）。

    :param D: 鉴别器
    :param G: 生成器
    :param path: 保存路径（如 './model/celeba/checkpoint.pth'）
    :param epoch: 当前 epoch 号
    :param scheduler_D: 可选的 LR 调度器（train_gan_dataloader 里创建的那两个）
    :param scheduler_G: 可选的 LR 调度器
    :param extra: 可选的附加数据 dict（如 {'epoch_tensor': epoch_tensor}）
    """
    state = {
        'D': D.state_dict(),
        'G': G.state_dict(),
        'D_optimizer': D.optimizer.state_dict(),
        'G_optimizer': G.optimizer.state_dict(),
        'D_counter': D.counter,
        'G_counter': G.counter,
        'D_progress': D.progress,
        'G_progress': G.progress,
        'epoch': epoch,
    }
    if scheduler_D is not None:
        state['scheduler_D'] = scheduler_D.state_dict()
    if scheduler_G is not None:
        state['scheduler_G'] = scheduler_G.state_dict()
    if extra is not None:
        state['extra'] = extra
    torch.save(state, path)
    print(f'[checkpoint] 已保存到 {path} (epoch={epoch})')


def load_checkpoint(D: Discriminator,
                    G: Generator,
                    path: str,
                    load_optimizer: bool = True,
                    load_progress: bool = True,
                    load_scheduler: bool = True,
                    map_location: str | torch.device = 'cpu'):
    """从 checkpoint 恢复训练状态（断点续训）。

    注意：先调用 GanUtil.Discriminator(...).to(device) / Generator(...).to(device) 把 D、G
    放到目标设备，再调用本函数。load_state_dict 会把权重拷进现有张量，因此 map_location
    默认 'cpu' 是安全的（无论训练时用的是 cuda 还是 cpu）。

    :param D: 鉴别器（已 .to(device)）
    :param G: 生成器（已 .to(device)）
    :param path: checkpoint 路径
    :param load_optimizer: 是否恢复优化器状态
    :param load_progress: 是否恢复 D/G 的 counter 与 loss 进度
    :param load_scheduler: 是否从 checkpoint 中取出调度器状态（需新建调度器后 load_state_dict）
    :param map_location: torch.load 的设备映射
    :return: dict，含 epoch、extra、以及 scheduler_D/G 状态（若有），供续训使用
    """
    state = torch.load(path, map_location=map_location, weights_only=False)
    D.load_state_dict(state['D'])
    G.load_state_dict(state['G'])
    if load_optimizer:
        D.optimizer.load_state_dict(state['D_optimizer'])
        G.optimizer.load_state_dict(state['G_optimizer'])
    if load_progress:
        D.counter = state['D_counter']
        G.counter = state['G_counter']
        D.progress = state['D_progress']
        G.progress = state['G_progress']
    ret = {
        'epoch': state.get('epoch'),
        'extra': state.get('extra'),
    }
    if load_scheduler:
        ret['scheduler_D'] = state.get('scheduler_D')
        ret['scheduler_G'] = state.get('scheduler_G')
    print(f'[checkpoint] 已加载 {path}')
    return ret