# C++ 智能指针精炼笔记

## 1. 为什么需要智能指针

传统动态内存管理流程：

```text
new -> use -> delete
```

示例：

```cpp
int* p = new int(10);
std::cout << *p << std::endl;
delete p;
```

这种方式需要程序员手动释放内存，容易出错。

常见问题：

- 忘记 `delete`：内存泄漏
- `delete` 后继续使用：悬挂指针，未定义行为
- 同一块内存 `delete` 两次：重复释放，可能导致程序崩溃
- 中途抛出异常：后面的 `delete` 可能执行不到

---

## 2. 智能指针的本质

智能指针本质上是：

```text
封装了原始指针的类模板
```

它内部保存一个原始指针，并在对象销毁时自动释放资源。

可以理解为：

```text
智能指针 = 原始指针 + 自动释放机制
```

示例：

```cpp
auto p = std::make_unique<int>(10);
```

这里不需要手动 `delete`。

---

## 3. RAII 原理

智能指针主要基于 RAII。

RAII 全称：

```text
Resource Acquisition Is Initialization
```

核心思想：

```text
对象创建时获取资源
对象销毁时释放资源
```

栈上的对象会自动销毁，而堆上的资源需要手动释放。

RAII 就是：

```text
利用栈对象的生命周期，管理堆上的资源
```

智能指针就是典型的 RAII 应用。

---

## 4. 栈和堆的关系

普通局部变量通常在栈上：

```cpp
void test() {
  int a = 10;
}
```

函数结束后，`a` 自动销毁。

动态对象通常在堆上：

```cpp
void test() {
  int* p = new int(10);
}
```

函数结束后，`p` 会销毁，但 `new int(10)` 不会自动释放。

智能指针的做法：

```cpp
void test() {
  auto p = std::make_unique<int>(10);
}
```

函数结束后：

```text
p 自动销毁
p 的析构函数释放堆内存
```

---

## 5. 常见智能指针

需要头文件：

```cpp
#include <memory>
```

常见三种：

```text
std::unique_ptr
std::shared_ptr
std::weak_ptr
```

---

## 6. std::unique_ptr

`unique_ptr` 表示独占所有权。

特点：

```text
同一时间只能有一个 unique_ptr 管理资源
不能复制
可以移动
```

示例：

```cpp
auto p1 = std::make_unique<int>(10);

// auto p2 = p1;  // 错误，不能复制

auto p2 = std::move(p1);  // 可以，转移所有权
```

适合：

```text
资源只有一个明确拥有者
```

---

## 7. std::shared_ptr

`shared_ptr` 表示共享所有权。

特点：

```text
多个 shared_ptr 可以管理同一块资源
内部使用引用计数
最后一个 shared_ptr 销毁时释放资源
```

示例：

```cpp
auto p1 = std::make_shared<int>(10);
auto p2 = p1;

std::cout << p1.use_count() << std::endl;
```

适合：

```text
资源需要被多个对象共享
生命周期不容易由单一对象决定
```

---

## 8. std::weak_ptr

`weak_ptr` 是弱引用，通常配合 `shared_ptr` 使用。

特点：

```text
不拥有资源
不增加引用计数
不能直接使用对象
需要 lock() 转成 shared_ptr
```

示例：

```cpp
auto sp = std::make_shared<int>(10);
std::weak_ptr<int> wp = sp;

if (auto p = wp.lock()) {
  std::cout << *p << std::endl;
}
```

主要用途：

```text
解决 shared_ptr 循环引用问题
```

---

## 9. 推荐写法

推荐：

```cpp
auto p1 = std::make_unique<int>(10);
auto p2 = std::make_shared<int>(20);
```

少写：

```cpp
std::unique_ptr<int> p1(new int(10));
std::shared_ptr<int> p2(new int(20));
```

原因：

```text
更简洁
更安全
减少裸 new
```

---

## 10. ROS 2 中的例子

你的 ROS 2 C++ 代码里：

```cpp
auto node = std::make_shared<PersonNode>("person_node", "张三", 18);
```

含义：

```text
在堆上创建 PersonNode 对象
使用 shared_ptr 自动管理它
```

访问成员函数：

```cpp
node->eat("鱼香肉丝");
```

等价于：

```cpp
(*node).eat("鱼香肉丝");
```

ROS 2 常用 `shared_ptr`，因为节点对象可能会被 executor、timer、publisher、subscription 等多个组件共同使用。

---

## 11. 一句话总结

```text
智能指针就是把堆资源交给一个栈对象管理，
栈对象销毁时，通过析构函数自动释放堆资源。
```

三种智能指针：

```text
unique_ptr: 独占资源
shared_ptr: 共享资源
weak_ptr: 弱引用，不拥有资源
```

现代 C++ 建议：

```text
少用裸 new/delete
优先使用 make_unique/make_shared
```
