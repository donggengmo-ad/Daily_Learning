# XSS 跨站脚本攻击
[TOC]

## 简介
攻击者将恶意脚本注入到网页中，其他用户访问时，浏览器执行恶意脚本，达到窃取信息、引导或冒充用户操作等。

## 分类
### 反射型 XSS
恶意脚本藏在 URL 参数、Post Body 等位置，服务器将其嵌入页面，反射给用户，被用户浏览器执行。

```php
# 漏洞代码
$keyword = $_GET['q'];
echo "搜索结果: $keyword";
```
```php
# 利用方式
GET /search.php?q=<script>alert('XSS')</script>
```

### 存储型 XSS
恶意脚本被提交到数据库，其他用户访问相关页面时，脚本被取出，嵌入页面并执行。

```php
# 留言板
$content = $_POST['content'];
$db->query("INSERT INTO comments (content) VALUES ('$content')");

# 显示留言
$result = $db->query("SELECT content FROM comments");
while($row = $result->fetch()) {
    echo "<div>" . $row['content'] . "</div>";
}
```
```php
# 利用方式
POST /comment.php

content=<script>alert('XSS')</script>
```
如果恶意脚本能操作用户写入新的 XSS 脚本，可以形成自我传播的蠕虫式攻击。

### DOM 型 XSS
恶意脚本通过修改页面 DOM 结构，或者利用浏览器的客户端脚本执行环境，直接在客户端执行。

```html
<script>
    // 从 URL hash 中取值，直接写入页面
    var name = location.hash.substring(1);
    document.getElementById("welcome").innerHTML = "欢迎，" + name;
</script>
```
```js
// 利用方式
GET /page.html/#<img src=x onerror=alert(1)>
```

## 攻击方式
本质是获得了在他人浏览器中执行 JavaScript 的能力。

### 窃取 Cookie

### Todo

## 注入点

