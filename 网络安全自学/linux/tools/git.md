# git

[TOC]

## 基础概念
### 区域
| 概念 | 说明 |
|------|------|
| **仓库（repository）** | 项目的版本控制根目录，包含 `.git` 文件夹存所有历史 |
| **工作区** | 当前目录中能看到的文件 |
| **暂存区** | 改动存入暂存区等待提交 |
| **远程（remote）** | 存放在服务器上的仓库副本 |

### 版本
| 概念 | 说明 |
|------|------|
| **提交（commit）** | 一次快照，记录文件状态、提交者、时间、信息 |
| **分支（branch）** | 独立的开发线，默认 `master` 或 `main` |
| **HEAD** | 当前所在分支的最新提交指针 |

### 节点
| 概念 | 说明 |
|------|------|
| **提交哈希（commit hash）** | 提交的唯一标识，SHA-1 哈希值 |
| **标签（tag）** | 提交的别名，便于记忆和引用 |
| **提交信息（message）** | 提交时填写的描述信息 |

## 命令
`git <command> [<args>]`

### 仓库
- `init <directory>`: 初始化仓库

- `clone <url> <directory>`: 克隆远程仓库

### 查看信息
- `status`: 查看工作区和暂存区状态
  
- `log`: 查看提交历史（时间倒序）
  - `-1 <commit>`: 查看指定提交的详细信息
  - `--oneline`: 简化输出为一行
  - `--graph`: 显示分支合并图
  - `--decorate`: 显示分支和标签信息

- `show <commit>`: 查看指定提交的详细信息

- `diff <commit1> <commit2>`: 查看两个提交之间的差异
  - 默认比较工作区和暂存区的差异
  - `--staged`: 比较暂存区和最新提交的差异

### 提交
- `add <file>`: 将文件改动添加到暂存区

- `commit`: 提交
  - `-m <message>`: 添加提交信息
  - `-a`: 自动将已跟踪文件添加到暂存区并提交

### 撤销
- `reset <commit>`: 回退到指定提交
  - `--soft`: 保留工作区和暂存区
  - `--mixed`: 保留工作区，清空暂存区
  - `--hard`: 清空工作区和暂存区

- `revert <commit>`: 生成一个新的提交，用于撤销指定提交的更改 

### 分支
- `branch`: 查看分支列表  
  - `-a`: 查看本地和远程分支
  - `branch <branch>`: 创建分支
  
- `checkout <branch>`: 切换到目标分支
  - `-b <branch>`: 创建并切换到新分支
  - `-f <branch> <start-point>`: 强制切换分支
  
- `merge <target>`: 合并当前分支到目标分支
  
- `rebase <target>`: 当前分支变基为目标分支

### HEAD
- `checkout <commit>`: 切换到指定提交
  
- `checkout HEAD^`: 切换到当前分支的上一个提交

- `checkout HEAD~<n>`: 切换到当前分支 n 次前的提交
  
### 远程
- `remote`: 查看远程仓库列表
  - `-v`: 显示详细信息
  - `add <name> <url>`: 添加远程仓库
  - `remove <name>`: 删除远程仓库
  - `update <name> <url>`: 更新远程仓库 URL

- `fetch <name>`: 拉取远程仓库的更新到本地，但不合并

- `pull <name> <branch>`: 拉取远程仓库的更新并合并到当前分支

- `push <name> <branch>`: 将当前分支的提交推送到远程仓库

### 标签
- `tag`: 查看标签列表
  - `-n`: 显示 message 第一行
  - `tag <tag> <commit>`: 创建标签（默认当前提交）
  - `-a <tag>`: 创建带注释的标签
    - `-m <message>`: 添加标签信息
    - `-F <file>`: 从文件中读取标签信息
  - `tag -d <tag>`: 删除标签

- `show <tag>`: 查看标签信息

