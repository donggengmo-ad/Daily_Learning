# Shell Script

[TOC]

## 基础
### 基本结构
```bash
#!/bin/bash（指定解释器）
commands（执行命令）
```

### 运行
- `bash script.sh`: 使用 bash 解释器运行脚本
- `chmod +x script.sh`: 赋予脚本可执行权限
- `./script.sh`: 直接运行脚本（需可执行权限）

## 语法
### 变量
#### 定义
- `var=value`: 定义变量
- `readonly var=value`: 定义只读变量
- `export var=value`: 定义环境变量
- `unset var`: 删除变量

#### 使用
- `$var`: 使用变量
- `${var}`: 使用变量（可用于复杂表达式）

#### 命令替换
`var=$(command)`: 将命令输出赋值给变量

### 条件判断
#### if 语句
```bash
if condition; then
    commands
elif condition; then
    commands
else
    commands
fi
```

#### 条件表达式
> 两边必须有空格
- `[ expression ]`

| 字符串比较 | 数值比较 | 文件测试 |
|------------|----------|----------|
| `=` `==` 等于 | `-eq` 等于 | `-f` 是否为普通文件 |
| `!=` 不等于 | `-ne` 不等于 | `-d` 是否为目录 |
| `-z` 为空 | `-gt` 大于 | `-r` 是否可读 |
| `-n` 非空 | `-lt` 小于 | `-w` 是否可写 |
|  | `-ge` 大于等于 | `-x` 是否可执行 |

- `(( expression ))`: 数学运算

- 多重条件判断
  - `&&` 与
  - `||` 或
  - `!` 非


### 循环
#### for 循环
for in
```bash
for var in list; do
    commands
done
```

C 风格
```bash
for (( i=start; i<=end; i++ )); do
    commands
done
```

#### while 循环
```bash
while condition; do
    commands
done
``` 

### 函数
#### 定义
```bash
func() {
    local var=value  # 局部变量
    commands
    return value  # 返回值
}
```

#### 调用
```bash
func arguments
```

#### 传参
- `$0`: 脚本名
- `$n`: 第 n 个参数
- `${n}`: 第 n 个参数（两位以上的n）
- `$#`: 参数个数
- `$*`: 所有参数（合并成一个字符串）
- `$@`: 所有参数（独立列表）
