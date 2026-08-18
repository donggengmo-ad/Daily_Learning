import os, requests, hashlib
import zipfile, tarfile
import torch
from torch.utils import data
import torchvision
from torchvision import transforms
from torch.utils.data import DataLoader, TensorDataset
import random

def load_array(data_arrays: tuple, batch_size, is_train=True):
    """构造一个 PyTorch 数据迭代器"""
    dataset = TensorDataset(*data_arrays) # 将特征和标签打包成数据集
    return DataLoader(dataset, batch_size, shuffle=is_train) # 返回数据迭代器

def data_iter(batch_size, features, labels):
    """生成一个小批量随机样本迭代器"""
    # 随机打乱样本的读取顺序
    num_examples = len(features)
    indices = list(range(num_examples)) # 生成索引列表
    random.shuffle(indices) # 随机打乱索引列表
    # 按批量大小切割小批量样本
    for i in range(0, num_examples, batch_size):
        batch_indices = torch.tensor(indices[i: min(i + batch_size, num_examples)]) # 取出当前批量的索引
        yield features[batch_indices], labels[batch_indices] # yield 返回一个迭代器


# fashion-mnist 数据集
def get_fashion_mnist_labels(labels):
    """返回 Fashion-MNIST 数据集的文本标签"""
    text_labels = ['t-shirt', 'trouser', 'pullover', 'dress', 'coat',
                   'sandal', 'shirt', 'sneaker', 'bag', 'ankle boot']
    return [text_labels[int(i)] for i in labels]

def get_dataloader_workers():
    """使用多进程读取数据"""
    return 4

def load_data_fashion_mnist(batch_size, resize=None):
    """下载 Fashion-MNIST 数据集，然后将其加载到内存中
    :param batch_size: 批量大小
    :param resize: 图像大小
    :return: (训练集, 测试集) 的迭代器
    """
    trans = [transforms.ToTensor()]
    if resize:
        trans.insert(0, transforms.Resize(resize))
    trans = transforms.Compose(trans)
    mnist_train = torchvision.datasets.FashionMNIST(root='./data', train=True, transform=trans, download=True)
    mnist_test = torchvision.datasets.FashionMNIST(root='./data', train=False, transform=trans, download=True)
    return (data.DataLoader(mnist_train, batch_size, shuffle=True, num_workers=get_dataloader_workers()),
            data.DataLoader(mnist_test, batch_size, shuffle=False, num_workers=get_dataloader_workers()))

# kaggle 数据集
DATA_HUB = dict()
DATA_URL = 'https://d2l-data.s3-accelerate.amazonaws.com/'

def download(name, cache_dir=os.path.join('.', 'data')):
    """下载一个DATA_HUB中的文件，并返回本地文件名"""
    assert name in DATA_HUB, f"{name} 不存在于 {DATA_HUB}"
    url, sha1_hash = DATA_HUB[name]
    os.makedirs(cache_dir, exist_ok=True)
    fname = os.path.join(cache_dir, url.split('/')[-1])
    if os.path.exists(fname):
        sha1 = hashlib.sha1()
        with open(fname, 'rb') as f:
            while True:
                data = f.read(1048576)
                if not data:
                    break
                sha1.update(data)
        if sha1.hexdigest() == DATA_HUB[name][1]:
            return fname  # 已经下载过了
    print(f'正在从{url}下载{fname}...')
    r = requests.get(url, stream=True)
    with open(fname, 'wb') as f:
        f.write(r.content)
    return fname

