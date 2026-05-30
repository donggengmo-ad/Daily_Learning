#!/usr/bin/env python3
"""扫描算法理论/数据结构/模版 中的笔记和模板，生成目录索引。"""

import os
from pathlib import Path

# ===== 配置 =====
BASE = Path(__file__).resolve().parent.parent.parent  # 算法自学/
OUTPUT = Path(__file__).resolve().parent / "目录索引.md"

SCAN_DIRS = {
    "算法理论": BASE / "算法理论",
    "数据结构": BASE / "数据结构",
    "模版":     BASE / "模版",
}

EXT_MD  = ".md"
EXT_CPP = ".cpp"

# 用 emoji 标记文件类型
ICON = {EXT_MD: "📝", EXT_CPP: "💻"}

# ===== 辅助函数 =====

def heading(level, text):
    return "#" * level + " " + text


def fmt_file(rel_path, ext):
    """生成带图标的文件链接行"""
    icon = ICON.get(ext, "📄")
    name = rel_path.stem
    # 用相对路径（相对于输出文件所在目录）
    # 输出在 学习管理/笔记整理/，笔记在 算法自学/xxx/
    out_dir = Path(__file__).resolve().parent
    target = (BASE / rel_path).resolve()
    rel = os.path.relpath(target, out_dir)
    return f"- {icon} [{name}]({rel})"


def scan_tree(root, base_rel):
    """递归扫描目录，返回 (dirs, files) 结构"""
    entries = []
    if not root.exists():
        return entries

    for path in sorted(root.iterdir()):
        rel = base_rel / path.name
        if path.is_dir():
            children = scan_tree(path, rel)
            if children:  # 只保留有文件的目录
                entries.append(("dir", path.name, children))
        elif path.suffix in (EXT_MD, EXT_CPP):
            entries.append(("file", path, rel, path.suffix))
    return entries


def render_tree(entries, level=2):
    """渲染目录结构为 markdown"""
    lines = []
    for entry in entries:
        if entry[0] == "dir":
            _, name, children = entry
            lines.append("")
            lines.append(heading(level, f"📁 {name}"))
            lines.extend(render_tree(children, level + 1))
        else:
            _, path, rel, ext = entry
            lines.append(fmt_file(rel, ext))
    return lines


# ===== 主流程 =====

lines = ["# 📚 笔记目录索引", ""]
lines.append(f"> 自动生成于 {__import__('datetime').datetime.now().strftime('%Y-%m-%d %H:%M')}")
lines.append(f"> 扫描范围：算法理论 / 数据结构 / 模版")
lines.append("")

for section, dir_path in SCAN_DIRS.items():
    rel_start = Path(section)
    entries = scan_tree(dir_path, rel_start)
    if entries:
        lines.append("---")
        lines.append("")
        lines.append(heading(1, f"📂 {section}"))
        lines.extend(render_tree(entries, 2))
    else:
        lines.append(heading(1, f"📂 {section} — *暂无文件*"))

lines.append("")
lines.append("---")
lines.append(f"> 🍒 运行 `python3 {Path(__file__).name}` 重新生成")

output = "\n".join(lines)
OUTPUT.write_text(output, encoding="utf-8")
print(f"✅ 目录索引已生成：{OUTPUT}")
print(f"   共 {len([l for l in lines if l.startswith('- ')])} 个文件")
