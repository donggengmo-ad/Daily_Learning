from matplotlib_inline import backend_inline
from matplotlib import pyplot as plt
from PIL import Image
import torch

def use_svg_display():  #@save
    """使用svg格式在Jupyter中显示绘图"""
    backend_inline.set_matplotlib_formats('svg')

def set_figsize(figsize=(3.5, 2.5)):  #@save
    """设置matplotlib的图表大小"""
    use_svg_display()
    plt.rcParams['figure.figsize'] = figsize

def set_axes(axes, xlabel, ylabel, xlim, ylim, xscale, yscale, legend):
    """设置matplotlib的轴"""
    axes.set_xlabel(xlabel)
    axes.set_ylabel(ylabel)
    axes.set_xscale(xscale)
    axes.set_yscale(yscale)
    axes.set_xlim(xlim)
    axes.set_ylim(ylim)
    if legend:
        axes.legend(legend)
    axes.grid()

def plot(X, Y=None, xlabel=None, ylabel=None, legend=None, xlim=None,
         ylim=None, xscale='linear', yscale='linear',
         fmts=('-', 'm--', 'g-.', 'r:'), figsize=(3.5, 2.5), axes=None):
    """绘制数据点"""
    if legend is None:
        legend = []

    set_figsize(figsize)
    axes = axes if axes else plt.gca()

    # 如果X有一个轴，输出True
    def has_one_axis(X):
        return (hasattr(X, "ndim") and X.ndim == 1 or isinstance(X, list)
                and not hasattr(X[0], "__len__"))

    if has_one_axis(X):
        X = [X]
    if Y is None:
        X, Y = [[]] * len(X), X
    elif has_one_axis(Y):
        Y = [Y]
    if len(X) != len(Y):
        X = X * len(Y)
    axes.cla()
    #通过数据在空白画布轴上画画，（x数据，y数据，线的样式）
    for x, y, fmt in zip(X, Y, fmts):
        if len(x):
            axes.plot(x, y, fmt)
        else:
            axes.plot(y, fmt)
    #将空白画布裱进画框里，设置画框的参数（画布，x轴名称，y轴名称，x轴的范围，y轴的范围，线的名称）
    set_axes(axes, xlabel, ylabel, xlim, ylim, xscale, yscale, legend)

def show_images(imgs, num_rows, num_cols, titles=None, scale=1.5):
    """画多张图片
    :param imgs: 图片列表
    :param num_rows: 图片行数
    :param num_cols: 图片列数
    :param titles: 图片标题列表
    :param scale: 图片缩放比例
    """
    figsize = (num_cols * scale, num_rows * scale)
    _, axes = plt.subplots(num_rows, num_cols, figsize=figsize)
    axes = axes.flatten()
    for i, (ax, img) in enumerate(zip(axes, imgs)):
        try:
            img = img.detach().numpy() # 如果是torch的tensor类型，先转为numpy类型
        except:
            pass
        ax.imshow(img)
        ax.axes.get_xaxis().set_visible(False)
        ax.axes.get_yaxis().set_visible(False)
        if titles:
            ax.set_title(titles[i])
    return axes

def box_corner_to_center(boxes: torch.Tensor) -> torch.Tensor:
    """从左上角和右下角的坐标形式转换为中心点坐标形式
    :param boxes: (N, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :return: (N, 4)的张量，4列分别是 (中心x, 中心y, 宽, 高)
    """
    # boxes (N, 4)，4 列分别是 (左上x, 左上y, 右下x, 右下y)
    x1, y1, x2, y2 = boxes[:, 0], boxes[:, 1], boxes[:, 2], boxes[:, 3]
    cx = (x1 + x2) / 2
    cy = (y1 + y2) / 2
    w = x2 - x1
    h = y2 - y1
    return torch.stack((cx, cy, w, h), dim=-1) # (N, 4)

def box_center_to_corner(boxes: torch.Tensor) -> torch.Tensor:
    """从中心点坐标形式转换为左上角和右下角的坐标形式
    :param boxes: (N, 4)的张量，4列分别是 (中心x, 中心y, 宽, 高)
    :return: (N, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    """
    # boxes (N, 4)，4 列分别是 (中心x, 中心y, 宽, 高)
    cx, cy, w, h = boxes[:, 0], boxes[:, 1], boxes[:, 2], boxes[:, 3]
    x1 = cx - 0.5 * w
    y1 = cy - 0.5 * h
    x2 = cx + 0.5 * w
    y2 = cy + 0.5 * h
    return torch.stack((x1, y1, x2, y2), dim=-1) # (N, 4)

def bbox_to_rect(bbox, color):
    """将边缘框（左上角和右下角）格式转换为 Matplotlib 格式（左上坐标、高宽）
    :param bbox: (4,)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :param color: 颜色
    :return: Matplotlib 的矩形对象
    """
    return plt.Rectangle(xy=(bbox[0], bbox[1]),
                         width=bbox[2]-bbox[0],
                         height=bbox[3]-bbox[1],
                         fill=False, edgecolor=color, linewidth=2)

def show_bboxes(axes, bboxes, labels=None, colors=None):
    """显示边界框
    :param axes: Matplotlib 的轴对象
    :param bboxes: (N, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :param labels: (N,)的列表，边界框的标签
    :param colors: (N,)的列表，边界框的颜色
    """
    def make_list(obj, default_values=None):
        if obj is None:
            obj = default_values
        elif not isinstance(obj, (list, tuple)):
            obj = [obj]
        return obj
    labels = make_list(labels)
    colors = make_list(colors, ['b', 'g', 'r', 'm', 'c'])
    for i, bbox in enumerate(bboxes):
        color = colors[i % len(colors)]
        rect = bbox_to_rect(bbox.detach().numpy(), color)
        axes.add_patch(rect)
        if labels and len(labels) > i:
            text_color = 'k' if color == 'w' else 'w'
            axes.text(rect.xy[0], rect.xy[1], labels[i],
                      va='center', ha='center', fontsize=9, color=text_color,
                      bbox=dict(facecolor=color, lw=0))

def multibox_prior(data, sizes, ratios):
    """生成一个像素点上的不同形状的锚框
    :param data: (N, C, H, W)的张量，表示输入图像
    :param sizes: 锚框的大小列表，表示锚框的高占输入图像高宽的比例
    :param ratios: 锚框的宽高比列表，表示锚框的宽高比
    :return: (1, H*W*num_sizes*num_ratios, 4)的张量，表示锚框的坐标，4列分别是 (左上x, 左上y, 右下x, 右下y)
    """
    in_height, in_width = data.shape[-2:]
    device, num_sizes, num_ratios = data.device, len(sizes), len(ratios)
    boxes_per_pixel = (num_sizes + num_ratios - 1)
    size_tensor = torch.tensor(sizes, device=device)
    ratio_tensor = torch.tensor(ratios, device=device)
    # 偏移是为了将锚框移动到像素的中心。由于像素的高度=1，宽度=1，将中心偏移 0.5
    offset_h, offset_w = 0.5, 0.5
    steps_h = 1.0 / in_height  # 缩放后的 y 轴步长
    steps_w = 1.0 / in_width  # 缩放后的 x 轴步长
    # 生成所有锚框的中心点
    center_h = (torch.arange(in_height, device=device) + offset_h) * steps_h
    center_w = (torch.arange(in_width, device=device) + offset_w) * steps_w
    shift_y, shift_x = torch.meshgrid(center_h, center_w, indexing='ij')
    shift_y, shift_x = shift_y.reshape(-1), shift_x.reshape(-1)
    # 生成 `boxes_per_pixel` 个高度和宽度，之后将用于创建锚框的角坐标 (xmin, xmax, ymin, ymax)，来计算锚框的宽度和高度
    w = torch.cat((size_tensor * torch.sqrt(ratio_tensor[0]),
                   sizes[0] * torch.sqrt(ratio_tensor[1:])))\
                   * in_height / in_width  # 处理矩形输入
    h = torch.cat((size_tensor / torch.sqrt(ratio_tensor[0]),
                   sizes[0] / torch.sqrt(ratio_tensor[1:])))
    # 除以 2 得到锚框的半高和半宽
    anchor_manipulations = torch.stack((-w, -h, w, h)).T.repeat(
                                        in_height * in_width, 1) / 2
    # 每个中心点将有 `boxes_per_pixel` 个锚框，因此生成一个包含所有锚框中心的网格，并重复 `boxes_per_pixel` 次
    out_grid = torch.stack([shift_x, shift_y, shift_x, shift_y],
                dim=1).repeat_interleave(boxes_per_pixel, dim=0)
    output = out_grid + anchor_manipulations
    return output.unsqueeze(0)


def box_iou(boxes1, boxes2):
    """计算两个锚框或边缘框列表之间的成对 IoU
    :param boxes1: (N, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :param boxes2: (M, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :return: (N, M)的张量，表示每个锚框或边缘框之间的 IoU
    """
    box_area = lambda boxes: ((boxes[:, 2] - boxes[:, 0]) *
                              (boxes[:, 3] - boxes[:, 1]))
    # boxes1、boxes2、areas1、areas2 的形状分别是 (boxes1 的数量, 4)、(boxes2 的数量, 4)、(boxes1 的数量, )、(boxes2 的数量, )
    areas1 = box_area(boxes1)
    areas2 = box_area(boxes2)
    # inter_upperlefts、inter_lowerrights、inters 的形状分别是 (boxes1 的数量, boxes2 的数量, 2)
    inter_upperlefts = torch.max(boxes1[:, None, :2], boxes2[:, :2])
    inter_lowerrights = torch.min(boxes1[:, None, 2:], boxes2[:, 2:])
    inters = (inter_lowerrights - inter_upperlefts).clamp(min=0)
    # inter_areas、union_areas 的形状分别是 (boxes1 的数量, boxes2 的数量)
    inter_areas = inters[:, :, 0] * inters[:, :, 1]
    union_areas = areas1[:, None] + areas2 - inter_areas
    return inter_areas / union_areas

def assign_anchor_to_bbox(ground_truth, anchors, device, iou_threshold=0.5):
    """把最接近的锚框分配给每个真实边界框
    :param ground_truth: (N, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :param anchors: (M, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :param device: 设备
    :param iou_threshold: IoU 阈值
    :return: (M,)的张量，表示每个锚框分配的真实边界框的索引，如果没有分配则为 -1
    """
    num_anchors, num_gt_boxes = anchors.shape[0], ground_truth.shape[0]
    # 在第 i 行和第 j 列的元素 x_ij 是锚框 i 和真实边界框 j 的 IoU
    jaccard = box_iou(anchors, ground_truth)
    # 初始化一个张量来保存每个锚框分配的真实边界框
    anchors_bbox_map = torch.full((num_anchors,), -1, dtype=torch.long, device=device)
    # 分配真实边界框给锚框
    max_ious, indices = torch.max(jaccard, dim=1)
    anc_i = torch.nonzero(max_ious >= iou_threshold).reshape(-1)
    box_j = indices[max_ious >= iou_threshold]
    anchors_bbox_map[anc_i] = box_j
    # 确保每个真实边界框至少分配给一个锚框
    col_discard = torch.full((num_anchors,), -1, dtype=torch.long, device=device)
    row_discard = torch.full((num_gt_boxes,), -1, dtype=torch.long, device=device)
    for _ in range(num_gt_boxes):
        max_idx = torch.argmax(jaccard)  # 找到最大 IoU 的索引
        box_idx = (max_idx % num_gt_boxes).long()
        anc_idx = (max_idx / num_gt_boxes).long()
        anchors_bbox_map[anc_idx] = box_idx
        # 将该锚框和真实边界框的行和列设置为 -1，以便在下一次迭代中忽略它们
        jaccard[:, box_idx] = col_discard
        jaccard[anc_idx, :] = row_discard
    return anchors_bbox_map

def offset_boxes(anchors, assigned_bb, eps=1e-6):
    """标记每个锚框的偏移量
    :param anchors: (N, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :param assigned_bb: (N, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :param eps: 防止除以零的一个小数
    :return: (N, 4)的张量，4列分别是 (偏移量x, 偏移量y, 偏移量宽, 偏移量高)
    """
    c_anc = box_corner_to_center(anchors)
    c_assigned_bb = box_corner_to_center(assigned_bb)
    offset_xy = 10 * (c_assigned_bb[:, :2] - c_anc[:, :2]) / c_anc[:, 2:]
    offset_wh = 5 * torch.log(eps + c_assigned_bb[:, 2:] / c_anc[:, 2:])
    offset = torch.cat([offset_xy, offset_wh], axis=1)
    return offset

def multibox_target(anchors, labels):
    """使用真实边界框标记锚框
    :param anchors: (1, num_anchors, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :param labels: (batch_size, num_objects, 5)的张量，5列分别是 (类别, 左上x, 左上y, 右下x, 右下y)
    :return: (batch_size, num_anchors, 4)的张量，表示每个锚框的偏移量
             (batch_size, num_anchors, 4)的张量，表示每个锚框是否被分配到真实边界框
             (batch_size, num_anchors)的张量，表示每个锚框的类别标签，如果没有分配则为 0
    """
    batch_size, anchors = labels.shape[0], anchors.squeeze(0)
    batch_offset, batch_mask, batch_class_labels = [], [], []
    device, num_anchors = anchors.device, anchors.shape[0]
    for i in range(batch_size):
        label = labels[i, :, :]
        anchors_bbox_map = assign_anchor_to_bbox(
            label[:, 1:], anchors, device)
        bbox_mask = ((anchors_bbox_map >= 0).float().unsqueeze(-1)).repeat(
            1, 4)
        # 初始化锚框的类别标签和分配的边界框坐标为零
        class_labels = torch.zeros(num_anchors, dtype=torch.long, device=device)
        assigned_bb = torch.zeros((num_anchors, 4), dtype=torch.float32, device=device)
        # 锚框的类别标签使用其分配的真实边界框进行标记。如果锚框没有分配任何边界框，标为背景（值为0）
        indices_true = torch.nonzero(anchors_bbox_map >= 0)
        bb_idx = anchors_bbox_map[indices_true]
        class_labels[indices_true] = label[bb_idx, 0].long() + 1
        assigned_bb[indices_true] = label[bb_idx, 1:]
        # offset transformation
        offset = offset_boxes(anchors, assigned_bb) * bbox_mask
        batch_offset.append(offset.reshape(-1))
        batch_mask.append(bbox_mask.reshape(-1))
        batch_class_labels.append(class_labels)
    bbox_offset = torch.stack(batch_offset)
    bbox_mask = torch.stack(batch_mask)
    class_labels = torch.stack(batch_class_labels)
    return bbox_offset, bbox_mask, class_labels

def offset_inverse(anchors, offset_preds):
    """根据带有预测偏移量的锚框来预测边界框
    :param anchors: (N, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :param offset_preds: (N, 4)的张量，4列分别是 (偏移量x, 偏移量y, 偏移量宽, 偏移量高)
    :return: (N, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    """
    anc = box_corner_to_center(anchors)
    pred_bbox_xy = (offset_preds[:, :2] * anc[:, 2:] / 10) + anc[:, :2]
    pred_bbox_wh = torch.exp(offset_preds[:, 2:] / 5) * anc[:, 2:]
    pred_bbox = torch.concat((pred_bbox_xy, pred_bbox_wh), axis=1)
    predicted_bbox = box_center_to_corner(pred_bbox)
    return predicted_bbox

def nms(boxes, scores, iou_threshold):
    """对预测边界框的置信度进行排序
    :param boxes: (N, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :param scores: (N,)的张量，表示每个边界框的置信度
    :param iou_threshold: IoU 阈值
    :return: (M,)的张量，表示保留预测边界框的索引
    """
    B = torch.argsort(scores, dim=-1, descending=True)
    keep = []  # 保留预测边界框的指标
    while B.numel() > 0:
        i = B[0]
        keep.append(i)
        if B.numel() == 1: break
        iou = box_iou(boxes[i, :].reshape(-1, 4),
                      boxes[B[1:], :].reshape(-1, 4)).reshape(-1)
        inds = torch.nonzero(iou <= iou_threshold).reshape(-1)
        B = B[inds + 1]
    return torch.tensor(keep, device=boxes.device)

def multibox_detection(cls_probs, offset_preds, anchors, nms_threshold=0.5,
                       pos_threshold=0.009999999):
    """使用非极大值抑制来预测边界框
    :param cls_probs: (batch_size, num_classes, num_anchors)的张量，表示每个锚框的类别概率
    :param offset_preds: (batch_size, num_anchors, 4)的张量，表示每个锚框的偏移量
    :param anchors: (1, num_anchors, 4)的张量，4列分别是 (左上x, 左上y, 右下x, 右下y)
    :param nms_threshold: 非极大值抑制的阈值
    :param pos_threshold: 正样本的阈值
    :return: (batch_size, num_anchors, 6)的张量，6列分别是 (类别, 置信度, 左上x, 左上y, 右下x, 右下y)
    """
    device, batch_size = cls_probs.device, cls_probs.shape[0]
    anchors = anchors.squeeze(0)
    num_classes, num_anchors = cls_probs.shape[1], cls_probs.shape[2]
    out = []
    for i in range(batch_size):
        cls_prob, offset_pred = cls_probs[i], offset_preds[i].reshape(-1, 4)
        conf, class_id = torch.max(cls_prob[1:], 0)
        predicted_bb = offset_inverse(anchors, offset_pred)
        keep = nms(predicted_bb, conf, nms_threshold)
        # 找到所有的non_keep索引，并将类设置为背景
        all_idx = torch.arange(num_anchors, dtype=torch.long, device=device)
        combined = torch.cat((keep, all_idx))
        uniques, counts = combined.unique(return_counts=True)
        non_keep = uniques[counts == 1]
        all_id_sorted = torch.cat((keep, non_keep))
        class_id[non_keep] = -1
        class_id = class_id[all_id_sorted]
        conf, predicted_bb = conf[all_id_sorted], predicted_bb[all_id_sorted]
        # pos_threshold是一个用于非背景预测的阈值
        below_min_idx = (conf < pos_threshold)
        class_id[below_min_idx] = -1
        conf[below_min_idx] = 1 - conf[below_min_idx]
        pred_info = torch.cat((class_id.unsqueeze(1),
                               conf.unsqueeze(1),
                               predicted_bb), dim=1)
        out.append(pred_info)
    return torch.stack(out)