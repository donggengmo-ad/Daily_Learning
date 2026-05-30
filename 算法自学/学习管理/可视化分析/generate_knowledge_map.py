#!/usr/bin/env python3
"""生成洛谷难度知识地图 TikZ 代码"""

import math

# === 颜色定义 ===
COLORS = {
    "红": "E74C3C",
    "橙": "F39C12",
    "黄": "FFC116",
    "绿": "52C41A",
    "蓝": "3498DB",
    "紫": "9D3DCF",
    "黑": "0E1D69",
}

# === 圆环参数 ===
R_CENTER = 1.5      # 中心圆半径
RING_HEIGHT = 1.5   # 每环宽度
N_RINGS = 6         # 橙色到黑色共6环
N_SECTORS = 7       # 7个领域

# 难度名称
DIFF_NAMES = ["红", "橙", "黄", "绿", "蓝", "紫", "黑"]
DIFF_LABELS = ["入门", "普及-", "普及/提高-", "普及+/提高", "提高+/省选-", "省选/NOI-", "NOI/NOI+/CTSC"]
SECTOR_NAMES = ["搜索", "动态规划", "数据结构", "图论", "字符串", "数学", "计算几何"]

# === 知识点数据 [ring][sector] ===
# ring 0 = 红色(中心), ring 1-6 对应橙到黑
# sector 0-6 对应7个领域

DATA = {
    # 橙色 (普及-)
    1: {
        0: "", 1: "", 2: "", 3: "",
        4: "查找替换\n大小写转换",
        5: "进制转换\n高精度",
        6: "",
    },
    # 黄色 (普及/提高-)
    2: {
        0: "DFS\nBFS\n排列组合",
        1: "线性DP\n背包DP",
        2: "栈 队列\n并查集\nset/map",
        3: "", 4: "",
        5: "素数 gcd/lcm\n快速幂 组合数",
        6: "",
    },
    # 绿色 (普及+/提高)
    3: {
        0: "记忆化搜索\n剪枝\n双向BFS",
        1: "区间DP 树形DP\n状压入门\n数位DP",
        2: "线段树(单点)\n树状数组\nST表 分块",
        3: "Dijkstra Floyd\nKruskal Prim\n拓扑 二分图",
        4: "KMP\nTrie\n字符串哈希",
        5: "exgcd CRT\nLucas 高斯消元\n欧拉函数",
        6: "凸包(Graham)\n叉积/点积",
    },
    # 蓝色 (提高+/省选-)
    4: {
        0: "",
        1: "状压DP\n单调队列优化\n斜率优化入门",
        2: "平衡树 树剖\n主席树(可持久化)\n莫队 CDQ分治",
        3: "网络流(Dinic)\n费用流 Tarjan\n2-SAT 差分约束",
        4: "AC自动机\nManacher\n后缀数组 SA\nSAM入门",
        5: "FFT/NTT\n线性基\n莫比乌斯反演\n杜教筛 SG函数",
        6: "半平面交\n旋转卡壳",
    },
    # 紫色 (省选/NOI-)
    5: {
        0: "",
        1: "插头DP\n动态DP\n决策单调性\n斯坦纳树",
        2: "LCT(动态树)\n树套树 KD-Tree\n点分治 虚树",
        3: "上下界网络流\n最大权闭合子图\n最小树形图 支配树",
        4: "SAM进阶\n回文自动机 PAM\n广义SAM",
        5: "多项式全家桶\nFWT\nBurnside/Polya\n二次剩余",
        6: "圆的面积并\n三维几何\n动态凸包",
    },
    # 黑色 (NOI/NOI+/CTSC)
    6: {
        0: "Meet-in-the-\nmiddle",
        1: "论文级DP",
        2: "ETT\n树分块",
        3: "一般图匹配\n(带花树)\n平面图算法",
        4: "字符串+\n生成函数",
        5: "多项式多点求值\n快速插值\nBM算法",
        6: "三维凸包",
    },
}


def generate_tikz():
    lines = []
    lines.append(r"\documentclass[tikz,border=10pt]{standalone}")
    lines.append(r"\usepackage{ctex}")
    lines.append(r"\usepackage{xcolor}")
    lines.append(r"\usetikzlibrary{math}")
    lines.append("")
    lines.append(r"\begin{document}")
    lines.append(r"\begin{tikzpicture}[")
    lines.append(r"  font=\sffamily,")
    lines.append(r"]")
    lines.append("")

    # Define colors
    for name, hexv in COLORS.items():
        lines.append(rf"\definecolor{{{name}}}{{HTML}}{{{hexv}}}")

    # Light version of each color
    for name, hexv in COLORS.items():
        r = int(hexv[0:2], 16)
        g = int(hexv[2:4], 16)
        b = int(hexv[4:6], 16)
        lr = min(255, r + 150)
        lg = min(255, g + 150)
        lb = min(255, b + 150)
        lines.append(rf"\definecolor{{{name}L}}{{HTML}}{{{lr:02X}{lg:02X}{lb:02X}}}")

    lines.append("")

    # Center circle (red) - no sector division
    lines.append(r"  % === 中心圆：红色-入门-语法基础 ===")
    lines.append(r"  \fill[红] (0,0) circle (1.5);")
    lines.append(r"  \node[white, font=\bfseries\large, align=center] at (0,0) {\textbf{语法}\\基础};")
    lines.append("")

    # Draw rings
    for ring_idx in range(1, N_RINGS + 1):
        outer_r = R_CENTER + ring_idx * RING_HEIGHT
        inner_r = R_CENTER + (ring_idx - 1) * RING_HEIGHT
        mid_r = (inner_r + outer_r) / 2
        color_name = DIFF_NAMES[ring_idx]
        light_color = f"{color_name}L"
        diff_label = DIFF_LABELS[ring_idx]

        lines.append(rf"  % === {color_name}色 - {diff_label} (r={inner_r}~{outer_r}) ===")

        for sector_idx in range(N_SECTORS):
            start_angle = sector_idx * 360 / N_SECTORS
            end_angle = (sector_idx + 1) * 360 / N_SECTORS
            mid_angle = (start_angle + end_angle) / 2

            content = DATA[ring_idx].get(sector_idx, "")
            if not content:
                lines.append(rf"  \fill[{light_color}] ({start_angle}:{inner_r}) arc ({start_angle}:{end_angle}:{inner_r}) -- ({end_angle}:{outer_r}) arc ({end_angle}:{start_angle}:{outer_r}) -- cycle;")
            else:
                lines.append(rf"  \fill[{light_color}] ({start_angle}:{inner_r}) arc ({start_angle}:{end_angle}:{inner_r}) -- ({end_angle}:{outer_r}) arc ({end_angle}:{start_angle}:{outer_r}) -- cycle;")
                content_tex = content.replace('\n', '\\\\')
                num_lines = content.count('\n') + 1
                if num_lines >= 3:
                    fontsize = r"\tiny"
                elif num_lines == 2:
                    fontsize = r"\scriptsize"
                else:
                    fontsize = r"\footnotesize"
                lines.append(rf"  \node[{fontsize}, {color_name}!90!black, align=center, anchor=center, inner sep=1pt, text width={2.2 if num_lines <= 2 else 2.0}cm] at ({mid_angle}:{mid_r}) {{\textbf{{{content_tex}}}}};")

            lines.append(rf"  \draw[{color_name}, thin] ({start_angle}:{inner_r}) -- ({start_angle}:{outer_r});")

        lines.append(rf"  \draw[{color_name}, thin] (0:{outer_r}) arc (0:360:{outer_r});")

    # Sector divider lines
    for sector_idx in range(N_SECTORS):
        start_angle = sector_idx * 360 / N_SECTORS
        lines.append(rf"  \draw[black!30, thin] ({start_angle}:{R_CENTER}) -- ({start_angle}:{R_CENTER + N_RINGS * RING_HEIGHT});")

    # Sector labels at outermost edge
    lines.append("")
    lines.append(r"  % === 扇区标签 ===")
    outer_most = R_CENTER + N_RINGS * RING_HEIGHT + 0.6
    for sector_idx in range(N_SECTORS):
        mid_angle = (sector_idx + 0.5) * 360 / N_SECTORS
        lines.append(rf"  \node[font=\small\bfseries, align=center] at ({mid_angle}:{outer_most}) {{{SECTOR_NAMES[sector_idx]}}};")

    # Difficulty labels on the right side
    lines.append("")
    lines.append(r"  % === 难度标签(右侧) ===")
    for ring_idx in range(N_RINGS, -1, -1):
        outer_r = R_CENTER + ring_idx * RING_HEIGHT
        inner_r = R_CENTER + (ring_idx - 1) * RING_HEIGHT if ring_idx > 0 else 0
        mid_r = (inner_r + outer_r) / 2 if ring_idx > 0 else R_CENTER / 2
        color_name = DIFF_NAMES[ring_idx]
        diff_label = DIFF_LABELS[ring_idx]
        y_pos = mid_r if ring_idx > 0 else R_CENTER / 2
        lines.append(rf"  \node[{color_name}, font=\scriptsize, anchor=west] at (0:{y_pos + 0.1}) {{\textcolor{{{color_name}}}{{\rule{{4pt}}{{4pt}}}} {diff_label}}};")

    lines.append(r"\end{tikzpicture}")
    lines.append(r"\end{document}")
    return "\n".join(lines)


if __name__ == "__main__":
    tikz_code = generate_tikz()
    print(tikz_code)
