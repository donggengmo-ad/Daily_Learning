# tmux
> Terminal Multiplexer

[TOC]

## 基础命令
- `tmux`: 启动 tmux 会话
- `tmux <command>`: 执行 tmux 命令

## 会话管理
  - `new`: 创建新会话
    - `-s <session_name>`: 指定会话名称
  - `attach`: 恢复指定会话
    - `-t <session_name>`: 指定会话名称
  - `detach`: 脱离当前会话（同`CTRL + b d`）
  - `ls`: 列出所有会话

## kill 命令
  - `kill-server`: 杀死所有会话
  - `kill-session`: 杀死会话
  - `kill-window`: 杀死窗口
  - `kill-pane`: 杀死分屏
  > `-t <session_name>`: 指定名称

## list 命令
  - `list-sessions`: 列出所有会话
  - `list-windows`: 列出当前会话的所有窗口
  - `list-panes`: 列出当前窗口的所有分屏

## 快捷键
<kbd>Ctrl</kbd>+<kbd>B</kbd> 后跟命令
### 窗口快捷键
  - <kbd>C</kbd>: 创建新窗口
  - <kbd>N</kbd>: 切换到下一个窗口
  - <kbd>P</kbd>: 切换到上一个窗口
  - <kbd>0-9</kbd>: 切换到指定编号的窗口
  
### 分屏快捷键
  - <kbd>%</kbd>: 左右分屏
  - <kbd>"</kbd>: 上下分屏
  - <kbd>O</kbd>: 切换分屏
  - <kbd>Up</kbd>/<kbd>Down</kbd>/<kbd>Left</kbd>/<kbd>Right</kbd>: 按方向切换分屏
  - <kbd>X</kbd>: 关闭当前分屏

## 配置文件
> 位置: `~/.tmux.conf`
  - `set -g mouse on`: 启用鼠标

