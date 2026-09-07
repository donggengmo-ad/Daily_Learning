import os, requests, hashlib, re
import typing
import zipfile, tarfile
import torch
from torch.utils import data
import torchvision
from torchvision import transforms
from torch.utils.data import DataLoader, TensorDataset
import random
import collections, math
from pathlib import Path
import shutil
import pandas as pd
import numpy as np
from .misc import tokenize, Vocab
import json

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
    return 12

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

# CIFAR-10 数据集
def load_cifar10(is_train: bool, augs, batch_size):
    """下载 CIFAR-10 数据集，然后将其加载到内存中
    :param is_train: 是否为训练集
    :param augs: 数据增强方法
    :param batch_size: 批量大小
    :return: 数据迭代器
    """
    dataset = torchvision.datasets.CIFAR10(root="./data", train=is_train, download=True, transform=augs)
    return torch.utils.data.DataLoader(dataset, batch_size=batch_size, shuffle=is_train, num_workers=get_dataloader_workers())

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

def download_extract(name, folder=None):
    """下载并解压 zip/tar 文件"""
    fname = download(name)
    base_dir = os.path.dirname(fname)
    data_dir, ext = os.path.splitext(fname)
    if ext == '.zip':
        fp = zipfile.ZipFile(fname, 'r')
    elif ext in ('.tar', '.gz'):
        fp = tarfile.open(fname, 'r')
    else:
        assert False, 'Only zip/tar files can be extracted.'
    fp.extractall(base_dir)
    return os.path.join(base_dir, folder) if folder else data_dir

# 图像分类数据集处理
def read_csv_labels(fname: str | Path):
    """读取 csv 文件，并返回 (名称, 标签) 的字典"""
    with open(fname, 'r') as f:
        lines = f.readlines()[1:]  # 去掉表头
    tokens = [l.rstrip().split(',') for l in lines]
    return dict((name, label) for name, label in tokens)

def copyfile(filename, target_dir: Path):
    """将文件复制到目标目录"""
    target_dir.mkdir(parents=True, exist_ok=True)
    shutil.copy(filename, target_dir)

def reorg_train_valid(data_dir: Path, labels: dict, valid_ratio: float)-> int:
    """将训练集划分为训练集和验证集，并按标签存放
    :param data_dir: 数据集所在目录，需要有 'train'、'test' 子目录
    :param labels: (名称, 标签) 的字典
    :param valid_ratio: 验证集占训练集的比例
    :return: 每个标签在验证集中的样本数
    """
    new_root = 'train_valid_test'
    n = collections.Counter(labels.values()).most_common()[-1][1]
    n_valid_per_label = max(1, math.floor(n * valid_ratio))
    label_count = {}
    for train_file in os.listdir(data_dir/'train'):
        label = labels[train_file.split('.')[0]]
        fname = data_dir/'train'/train_file
        if label not in label_count or label_count[label] < n_valid_per_label:
            copyfile(fname, data_dir/new_root/'valid'/label)
            label_count[label] = label_count.get(label, 0) + 1
        else:
            copyfile(fname, data_dir/new_root/'train'/label)
    return n_valid_per_label

def reorg_test(data_dir: Path):
    """将测试集按标签存放
    :param data_dir: 数据集所在目录，需要有 'train'、'test' 子目录
    """
    for test_file in os.listdir(data_dir/'test'):
        copyfile(data_dir/'test'/test_file, data_dir/'train_valid_test'/'test'/'unknown')

def reorg_data(data_dir: Path, label_file: str | Path, valid_ratio: float):
    """将数据集整理为训练集、验证集和测试集，并按标签存放
    :param data_dir: 数据集所在目录，需要有 'train'、'test' 子目录
    :param label_file: 标签文件名
    :param valid_ratio: 验证集占训练集的比例
    """
    labels = read_csv_labels(data_dir/label_file)
    reorg_train_valid(data_dir, labels, valid_ratio)
    reorg_test(data_dir)

def imagenet_normalize():
    """ImageNet 数据集的归一化参数"""
    return torchvision.transforms.Normalize(mean=[0.485, 0.456, 0.406],
                                                 std=[0.229, 0.224, 0.225])

def get_dataset(data_dir: Path, train_aug, test_aug):
    """获取训练集、验证集和测试集
    :param data_dir: 数据集所在目录，需要有 'train_valid_test' 子目录
    :param train_aug: 训练集的数据增强方法
    :param test_aug: 测试集的数据增强方法
    :return: 训练集, 验证集（增强）, 验证集, 测试集
    """
    # 带增强
    train_ds = torchvision.datasets.ImageFolder(data_dir / 'train_valid_test' / 'train', transform=train_aug)
    train_valid_ds = torchvision.datasets.ImageFolder(data_dir / 'train_valid_test' / 'valid', transform=train_aug)
    # 不带增强
    valid_ds = torchvision.datasets.ImageFolder(data_dir / 'train_valid_test' / 'valid', transform=test_aug)
    test_ds = torchvision.datasets.ImageFolder(data_dir / 'train_valid_test' / 'test', transform=test_aug)
    return train_ds, train_valid_ds, valid_ds, test_ds

def get_dataloader(train_ds, train_valid_ds, valid_ds, test_ds, batch_size,
                   workers: int=get_dataloader_workers(),
                   pin_memory: bool=True,
                   persistent_workers: bool=False):
    """获取训练集、验证集和测试集的迭代器
    :param train_ds: 训练集
    :param train_valid_ds: 验证集（增强）
    :param valid_ds: 验证集
    :param test_ds: 测试集
    :param batch_size: 批量大小
    :param workers: 读取数据的进程数
    :param pin_memory: 是否将数据放入固定内存中
    :param persistent_workers: 是否在每个迭代器中保持工作进程
    :return: 训练集, 验证集（增强）, 验证集, 测试集)迭代器
    """
    train_iter = torch.utils.data.DataLoader(train_ds,
                                             batch_size=batch_size,
                                             shuffle=True,
                                             persistent_workers=persistent_workers,
                                             num_workers=workers,
                                             pin_memory=pin_memory)
    train_valid_iter = torch.utils.data.DataLoader(train_valid_ds,
                                                   batch_size=batch_size,
                                                   shuffle=True,
                                                   persistent_workers=persistent_workers,
                                                   num_workers=workers,
                                                   pin_memory=pin_memory)
    valid_iter = torch.utils.data.DataLoader(valid_ds,
                                             batch_size=batch_size,
                                             shuffle=False,
                                             persistent_workers=persistent_workers,
                                             num_workers=workers,
                                             pin_memory=pin_memory)
    test_iter = torch.utils.data.DataLoader(test_ds,
                                            batch_size=batch_size,
                                            shuffle=False,
                                            persistent_workers=persistent_workers,
                                            num_workers=workers,
                                            pin_memory=pin_memory)
    return train_iter, train_valid_iter, valid_iter, test_iter

# 目标检测 banana 数据集
DATA_HUB['banana-detection'] = (
    DATA_URL + 'banana-detection.zip',
    '5de26c8fce5ccdea9f91267273464dc968d20d72')
def read_data_bananas(is_train=True):
    """读取香蕉检测数据集中的图像和标签"""
    data_dir = download_extract('banana-detection')
    csv_fname = os.path.join(data_dir, 'bananas_train' if is_train
                             else 'bananas_val', 'label.csv')
    csv_data = pd.read_csv(csv_fname)
    csv_data = csv_data.set_index('img_name')
    images, targets = [], []
    for img_name, target in csv_data.iterrows():
        images.append(
            torchvision.io.read_image(
                os.path.join(data_dir,
                             'bananas_train' if is_train else
                             'bananas_val', 'images', f'{img_name}')))
        # 这里的target包含（类别，左上角x，左上角y，右下角x，右下角y），
        # 其中所有图像都具有相同的香蕉类（索引为0）
        targets.append(list(target))
    return images, np.expand_dims(np.array(targets), 1) / 256

class BananasDataset(torch.utils.data.Dataset):
    """一个用于加载香蕉检测数据集的自定义数据集"""
    def __init__(self, is_train=True):
        self.features, self.labels = read_data_bananas(is_train)
        print('read ' + str(len(self.features)) + (' training examples' if is_train else ' validation examples'))
    def __getitem__(self, idx):
        return self.features[idx].float(), torch.tensor(self.labels[idx])
    def __len__(self):
        return len(self.features)

def load_data_bananas(batch_size):
    """加载香蕉检测数据集"""
    train_iter = torch.utils.data.DataLoader(BananasDataset(is_train=True),
                                             batch_size, shuffle=True)
    val_iter = torch.utils.data.DataLoader(BananasDataset(is_train=False),
                                           batch_size)
    return train_iter, val_iter

# 语义分割 VOC2012 数据集
DATA_HUB['voc2012'] = (DATA_URL + 'VOCtrainval_11-May-2012.tar',
                           '4e443f8a2eca6b1dac8a6c57641b67dd40621a49')

def read_voc_images(voc_dir, is_train=True):
    """读取VOC数据集中的图像和标签"""
    txt_fname = os.path.join(voc_dir, 'ImageSets', 'Segmentation',
                             'train.txt' if is_train else 'val.txt')
    mode = torchvision.io.image.ImageReadMode.RGB
    with open(txt_fname, 'r') as f:
        images = f.read().split()
    features, labels = [], []
    for i, fname in enumerate(images):
        features.append(torchvision.io.read_image(os.path.join(
            voc_dir, 'JPEGImages', f'{fname}.jpg')))
        labels.append(torchvision.io.read_image(os.path.join( # label 也是图片，每个像素的值是类别索引
            voc_dir, 'SegmentationClass' ,f'{fname}.png'), mode))
    return features, labels

VOC_COLORMAP = [[0, 0, 0], [128, 0, 0], [0, 128, 0], [128, 128, 0],
                [0, 0, 128], [128, 0, 128], [0, 128, 128], [128, 128, 128],
                [64, 0, 0], [192, 0, 0], [64, 128, 0], [192, 128, 0],
                [64, 0, 128], [192, 0, 128], [64, 128, 128], [192, 128, 128],
                [0, 64, 0], [128, 64, 0], [0, 192, 0], [128, 192, 0],
                [0, 64, 128]]

VOC_CLASSES = ['background', 'aeroplane', 'bicycle', 'bird', 'boat',
               'bottle', 'bus', 'car', 'cat', 'chair', 'cow',
               'diningtable', 'dog', 'horse', 'motorbike', 'person',
               'potted plant', 'sheep', 'sofa', 'train', 'tv/monitor']

def voc_colormap2label():
    """建立从RGB到类别索引的映射"""
    def hashing(colormap):
        return (colormap[0] * 256 + colormap[1]) * 256 + colormap[2]
    colormap2label = torch.zeros(256 ** 3, dtype=torch.long) # 存储每个颜色对应的类别索引
    for i, colormap in enumerate(VOC_COLORMAP):
        colormap2label[hashing(colormap)] = i # 将每个颜色映射到对应的类别索引
    return colormap2label

def voc_label_indices(colormap, colormap2label):
    """将彩色标签映射到类别索引"""
    def hashing(colormap):
        return (colormap[:, :, 0] * 256 + colormap[:, :, 1]) * 256 + colormap[:, :, 2]
    colormap = colormap.permute(1, 2, 0).numpy().astype('int32') # 将通道维度移到最后
    idx = hashing(colormap) # 将RGB映射到整数
    return colormap2label[idx] # 返回类别索引

def voc_rand_crop(feature, label, height, width):
    """随机裁剪图像和标签"""
    # get_params 返回裁剪窗口的左上角坐标和高宽
    rect = torchvision.transforms.RandomCrop.get_params(feature, (height, width))
    # 通用的参数裁剪图像和标签
    feature = torchvision.transforms.functional.crop(feature, *rect)
    label = torchvision.transforms.functional.crop(label, *rect)
    return feature, label


class VOCSegDataset(torch.utils.data.Dataset):
    """VOC语义分割数据集"""

    def __init__(self, is_train: bool, crop_size: tuple[int, int], voc_dir: str):
        self.transform = torchvision.transforms.Normalize(mean=[0.485, 0.456, 0.406],
                                                          std=[0.229, 0.224, 0.225])  # ImageNet 标准
        self.crop_size = crop_size
        features, labels = read_voc_images(voc_dir, is_train)
        self.features = [self.normalize_image(feat) for feat in self.filter(features)]
        self.labels = self.filter(labels)
        self.colormap2label = voc_colormap2label()
        print(f'read {len(self.features)} examples')

    def normalize_image(self, img):
        """标准化图像"""
        return self.transform(img.float())

    def filter(self, imgs):
        """过滤掉小于裁剪尺寸的图像"""
        return [img for img in imgs if (img.shape[1] >= self.crop_size[0] and
                                        img.shape[2] >= self.crop_size[1])]

    def __getitem__(self, idx):
        """返回裁剪后的图像和标签"""
        feature, label = voc_rand_crop(self.features[idx], self.labels[idx],
                                       *self.crop_size)
        return feature.float(), voc_label_indices(label, self.colormap2label)

    def __len__(self):
        """返回数据集大小"""
        return len(self.features)

def load_data_voc(batch_size, crop_size):
    """加载VOC数据集"""
    voc_dir = download_extract('voc2012', 'VOCdevkit/VOC2012')
    train_iter = torch.utils.data.DataLoader(
        VOCSegDataset(True, crop_size, voc_dir),
        batch_size=batch_size,
        shuffle=True,
        drop_last=True,
        num_workers=get_dataloader_workers())
    test_iter = torch.utils.data.DataLoader(
        VOCSegDataset(False, crop_size, voc_dir),
        batch_size=batch_size,
        drop_last=True,
        num_workers=get_dataloader_workers())
    return train_iter, test_iter

# 《Time Machine》小说数据
DATA_HUB['time_machine'] = (DATA_URL + 'timemachine.txt', '090b5e7e70c295757f55df93cb0a180b9691891a')

def read_time_machine():
    """将《Time Machine》小说加载到文本行的列表中"""
    with open(download('time_machine'), 'r') as f:
        lines = f.readlines()
    # 使用正则表达式去掉非字母字符，并将文本转换为小写
    return [re.sub('[^A-Za-z]+', ' ', line).strip().lower() for line in lines]

def load_corpus_time_machine(max_tokens=-1, token='char'):
    """返回时序数据和词汇表
    :param max_tokens: 最大 token 数量，若为 -1 则不限制
    :param token: 'word' 或 'char'，按单词或字符拆分
    :return: corpus (列表，包含所有 token 的索引) 和 vocab (词汇表)
    """
    lines = read_time_machine()
    tokens = tokenize(lines, token) # 按字符拆分
    vocab = Vocab(tokens) # 构建词汇表
    # 将所有 token 转成索引
    corpus = [vocab[token] for line in tokens for token in line]
    if max_tokens > 0:
        corpus = corpus[:max_tokens]
    return corpus, vocab

def seg_data_iter_random(corpus, batch_size, num_steps):
    """使用随机采样生成小批量子序列
    :param corpus: 文本序列
    :param batch_size: 小批量的样本数
    :param num_steps: 每个样本的时间步数
    """
    # 随机偏移量，保证每次数据切块不一样
    corpus = corpus[random.randint(0, num_steps - 1):]
    # 子序列数量，-1 因为考虑标签
    num_subseqs = (len(corpus) - 1) // num_steps
    # 长度为 num_steps 的子序列的起始索引
    initial_indices = list(range(0, num_subseqs * num_steps, num_steps))
    # 随机打乱起始索引
    random.shuffle(initial_indices)
    def data(pos):
        """根据索引返回长度为 num_steps 的序列"""
        return corpus[pos: pos + num_steps]
    num_batches = num_subseqs // batch_size # 小批量数量
    for i in range(0, batch_size * num_batches, batch_size): # 每次取 batch_size 个子序列
        initial_indices_per_batch = initial_indices[i: i + batch_size] # 每个小批量的起始索引
        X = [data(j) for j in initial_indices_per_batch] # 每个小批量的输入
        Y = [data(j + 1) for j in initial_indices_per_batch] # 每个小批量的标签（输入右移一位）
        yield torch.tensor(X), torch.tensor(Y)

def seq_data_iter_sequential(corpus, batch_size, num_steps):
    """使用顺序分区生成小批量子序列
    :param corpus: 文本序列
    :param batch_size: 小批量的样本数
    :param num_steps: 每个样本的时间步数
    """
    # 从随机偏移量开始划分序列，保证每次划分不一样
    offset = random.randint(0, num_steps)
    num_tokens = ((len(corpus) - offset - 1) // batch_size) * batch_size # 可整除的 token 数量
    Xs = torch.tensor(corpus[offset: offset + num_tokens]) # 输入
    Ys = torch.tensor(corpus[offset + 1: offset + 1 + num_tokens]) # 标签（输入右移一位）
    Xs, Ys = Xs.reshape(batch_size, -1), Ys.reshape(batch_size, -1) # 每行是一个小批量
    num_batches = Xs.shape[1] // num_steps # 小批量数量
    for i in range(0, num_steps * num_batches, num_steps): # 每次取 num_steps 个时间步
        X = Xs[:, i: i + num_steps] # 每个小批量的输入
        Y = Ys[:, i: i + num_steps] # 每个小批量的标签（输入右移一位）
        yield X, Y


class SeqDataLoader:
    """加载序列数据的迭代器"""
    def __init__(self,
                 batch_size: int,
                 num_steps: int,
                 use_random_iter: bool,
                 max_tokens: int = -1,
                 dataset_load_fn: typing.Callable=load_corpus_time_machine):
        """初始化迭代器
        :param batch_size: 小批量的样本数
        :param num_steps: 每个样本的时间步数
        :param use_random_iter: 是否使用随机采样
        :param max_tokens: 最大 token 数量，-1 表示不限制
        :param dataset_load_fn: 加载数据集的函数
        """
        if use_random_iter:
            self.data_iter_fn = seg_data_iter_random
        else:
            self.data_iter_fn = seq_data_iter_sequential
        self.corpus, self.vocab = dataset_load_fn(max_tokens)  # 读取语料库和词汇表
        self.batch_size, self.num_steps = batch_size, num_steps

    def __iter__(self):
        return self.data_iter_fn(self.corpus, self.batch_size, self.num_steps)

def load_data_time_machine(batch_size: int, num_steps: int, use_random_iter: bool=False, max_tokens: int=-1):
    """返回时光机器数据集的迭代器和词汇表
    :param batch_size: 小批量的样本数
    :param num_steps: 每个样本的时间步数
    :param use_random_iter: 是否使用随机采样
    :param max_tokens: 最大 token 数量，-1 表示不限制
    """
    data_iter = SeqDataLoader(batch_size, num_steps, use_random_iter, max_tokens,
                              dataset_load_fn=load_corpus_time_machine)
    return data_iter, data_iter.vocab

# 自定义宋词数据集
def read_songci():
    """读取宋词数据集"""
    # 数据集里全是 json
    lines = []
    base_dir = Path('./data/songci')
    for file in base_dir.iterdir():
        if file.suffix == '.json':
            with open(file, 'r', encoding='utf-8') as f:
                data_dicts = json.load(f)
                # data_dicts 是一个字典列表
                # poem 是字典，poem['paragraphs'] 是字符串列表
                # poem_lines 是一个字符串列表，extend 方法将每个段落添加到 lines 列表中
                poem_lines = [paragraph for poem in data_dicts for paragraph in poem['paragraphs']]
                lines.extend(poem_lines) # 将每个段落添加到 lines 列表中
    return [line.strip() for line in lines if line.strip()]

def load_corpus_songci(max_tokens=-1):
    """返回宋词数据集的语料库和词汇表
    :param max_tokens: 最大 token 数量，若为 -1 则不限制
    :return: corpus (列表，包含所有 token 的索引) 和 vocab (词汇表)
    """
    lines = read_songci()
    tokens = tokenize(lines, 'char') # 按字符拆分
    vocab = Vocab(tokens) # 构建词汇表
    # 将所有 token 转成索引
    corpus = [vocab[token] for line in tokens for token in line]
    if max_tokens > 0:
        corpus = corpus[:max_tokens]
    return corpus, vocab

def load_data_songci(batch_size: int, num_steps: int, use_random_iter: bool=False, max_tokens: int=-1):
    """返回宋词数据集的迭代器和词汇表
    :param batch_size: 小批量的样本数
    :param num_steps: 每个样本的时间步数
    :param use_random_iter: 是否使用随机采样
    :param max_tokens: 最大 token 数量，-1 表示不限制
    """
    data_iter = SeqDataLoader(batch_size, num_steps, use_random_iter, max_tokens,
                              dataset_load_fn=load_corpus_songci)
    return data_iter, data_iter.vocab

# 机器翻译数据集
DATA_HUB['fra-eng'] = (DATA_URL + 'fra-eng.zip', '94646ad1522d915e7b0f9296181140edcf86a4f5')

def read_data_nmt():
    """加载“英语-法语”数据集"""
    data_dir = download_extract('fra-eng')
    with open(os.path.join(data_dir, 'fra.txt'), 'r', encoding='utf-8') as f:
        return f.read()

def preprocess_nmt(text):
    """预处理“英语-法语”数据集"""
    def no_space(char, prev_char):
        """判断是否需要在两个字符之间添加空格"""
        return char in set(',.!?') and prev_char != ' '
    text = text.replace('\u202f', ' ').replace('\xa0', ' ').lower() # 将文本转换为小写，并替换特殊空格
    out = [' ' + char if i > 0 and no_space(char, text[i - 1]) else char for i, char in enumerate(text)] # 在标点符号前添加空格
    return ''.join(out)

def tokenize_nmt(text, num_examples=None):
    """将“英语-法语”数据集拆分为英语和法语的句子对"""
    source, target = [], []
    for i, line in enumerate(text.split('\n')):
        if num_examples and i >= num_examples:
            break
        parts = line.split('\t')
        if len(parts) == 2:
            source.append(parts[0].strip())
            target.append(parts[1].strip())
    return source, target

def truncate_pad(line, num_steps, padding_token):
    """截断或填充文本序列"""
    if len(line) > num_steps:
        return line[:num_steps]  # 截断
    return line + [padding_token] * (num_steps - len(line))  # 填充

def build_array_nmt(lines, vocab, num_steps):
    """返回小批量"""
    lines = [vocab[l] for l in lines]
    lines = [l + [vocab['<eos>']] for l in lines]  # 添加 <eos> 结束标记
    array = torch.tensor([truncate_pad(l, num_steps, vocab['<pad>']) for l in lines])
    valid_len = (array != vocab['<pad>']).type(torch.int32).sum(1)  # 计算有效长度
    return array, valid_len

def load_data_nmt(batch_size, num_steps, num_examples=600):
    """返回“英语-法语”数据集的迭代器和词表"""
    text = preprocess_nmt(read_data_nmt())
    source, target = tokenize_nmt(text, num_examples)
    src_vocab = Vocab(source, min_freq=2, reserved_tokens=['<pad>', '<bos>', '<eos>'])
    tgt_vocab = Vocab(target, min_freq=2, reserved_tokens=['<pad>', '<bos>', '<eos>'])
    src_array, src_valid_len = build_array_nmt(source, src_vocab, num_steps)
    tgt_array, tgt_valid_len = build_array_nmt(target, tgt_vocab, num_steps)
    data_arrays = (src_array, src_valid_len, tgt_array, tgt_valid_len)
    data_iter = load_array(data_arrays, batch_size)
    return data_iter, src_vocab, tgt_vocab