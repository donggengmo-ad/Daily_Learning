# cron

[TOC]

## 任务语法
```
 ┌ 分钟 (0 - 59)
 │ ┌ 小时 (0 - 23)
 │ │ ┌ 日期 (1 - 31)
 │ │ │ ┌ 月份 (1 - 12)
 │ │ │ │ ┌ 星期几 (0 - 7) (0和7均为周日)
 * * * * *  <user>(仅系统级) <command>
```
- `*`: 任意值（每个单位都执行一次）
- `,`:  列举
- `-`: 范围
- `/`: 步长（每隔多少个单位执行一次）
- `@reboot`: 系统启动时执行一次

## 个人 crontab
### 位置
`/var/spool/cron/crontabs/`

每人一个，文件名为用户名

### 命令
`crontab <option>`
- `-l`: 列出当前用户的任务
- `-e`: 编辑当前用户的 crontab 文件
- `-r`: 删除当前用户的 crontab 文件
- `-u <user> <option>`: 指定用户（仅 root 可用）

## 系统 crontab
`/etc/crontab`

指定用户和命令，可以设环境变量

## cron.d
`/etc/cron.d/`

每个任务单独一个文件

## 脚本目录
- `/etc/cron.hourly/`: 每小时执行一次
- `/etc/cron.daily/`: 每天执行一次
- `/etc/cron.weekly/`: 每周执行一次
- `/etc/cron.monthly/`: 每月执行一次   


