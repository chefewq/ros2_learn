# C++ 智能指针学习笔记

## 1. 为什么要引入智能指针

在 C++ 中，传统的动态内存管理通常是：

```cpp
int* p = new int(10);

// use p

delete p;
```

也就是：

```text
new -> use -> delete
```

这种方式的问题是：内存的释放完全依赖程序员手动管理。只要某一步忘了、写错了、提前返回了，程序就可能出现严重问题。

常见风险包括：

- 内存泄漏
- 悬挂指针
- 重复释放
- 异常导致资源没有释放

智能指针就是为了解决这些问题而产生的。

---

## 2. 手动管理内存的常见问题

### 2.1 内存泄漏

如果使用 `new` 申请了堆内存，但是忘记使用 `delete` 释放，就会造成内存泄漏。

```cpp
void test() {
  int* p = new int(10);

  // 忘记 delete p
}
```

函数结束后，局部变量 `p` 会被销毁，但是 `p` 指向的堆内存不会自动释放。

也就是说：

```text
p 这个指针变量消失了
但是 new 出来的 int 还留在堆上
```

这块内存无法再被访问，也无法被释放，就形成了内存泄漏。

---

### 2.2 悬挂指针

释放内存后，指针变量本身仍然保存着原来的地址。

```cpp
int* p = new int(10);

delete p;

std::cout << *p << std::endl;  // 错误
```

`delete p` 之后，`p` 指向的堆内存已经无效。

但是 `p` 这个指针变量还在，并且还保存着旧地址。

这种指针叫做：

```text
悬挂指针
```

继续使用悬挂指针会导致未定义行为。

---

### 2.3 重复释放

同一块堆内存被释放两次，也会导致严重错误。

```cpp
int* p = new int(10);

delete p;
delete p;  // 错误
```

重复释放可能导致程序崩溃，也可能造成更隐蔽的内存破坏。

---

### 2.4 异常导致 delete 没有执行

手动管理内存还有一个问题：如果中途抛出异常，后面的 `delete` 可能不会执行。

```cpp
void test() {
  int* p = new int(10);

  throw std::runtime_error("error");

  delete p;  // 执行不到
}
```

这样也会造成内存泄漏。

---

## 3. 智能指针的本质

智能指针的本质是：

```text
封装了原始 C++ 指针的类模板
```

它内部保存一个原始指针，并在合适的时候自动释放这个指针指向的资源。

可以简单理解为：

```text
智能指针 = 原始指针 + 自动释放机制
```

例如：

```cpp
std::shared_ptr<int> p = std::make_shared<int>(10);
```

这里不需要手动写：

```cpp
delete p;
```

因为 `std::shared_ptr` 会自动管理这块堆内存的生命周期。

---

## 4. RAII 原理

智能指针的核心思想是 RAII。

RAII 全称是：

```text
Resource Acquisition Is Initialization
```

通常翻译为：

```text
资源获取即初始化
```

更直观的理解是：

```text
对象创建时获取资源
对象销毁时释放资源
```

---

## 5. 栈空间和堆空间

### 5.1 栈空间

栈上的局部变量由编译器自动管理。

```cpp
void test() {
  int a = 10;
}
```

当函数 `test()` 结束时，变量 `a` 会自动销毁。

栈空间的特点：

```text
自动创建
自动销毁
生命周期清晰
不需要手动 delete
```

---

### 5.2 堆空间

堆上的对象通常需要程序员手动管理。

```cpp
void test() {
  int* p = new int(10);
}
```

`new int(10)` 创建的对象在堆上。

如果没有执行：

```cpp
delete p;
```

这块堆内存就不会自动释放。

堆空间的特点：

```text
手动申请
手动释放
生命周期灵活
但容易出错
```

---

## 6. RAII 如何管理堆资源

RAII 的核心是：

```text
利用栈对象的自动生命周期，管理堆上的资源
```

普通指针的问题：

```cpp
void test() {
  int* p = new int(10);
}
```

函数结束后：

```text
p 是栈上的局部变量，会自动销毁
但是 p 指向的堆内存不会自动释放
```

智能指针的做法：

```cpp
void test() {
  std::unique_ptr<int> p = std::make_unique<int>(10);
}
```

执行过程：

```text
std::make_unique<int>(10)
        ↓
在堆上创建 int
        ↓
unique_ptr p 保存堆内存地址
        ↓
函数结束，p 作为栈对象自动销毁
        ↓
p 的析构函数自动释放它管理的堆内存
```

所以智能指针就是：

```text
用一个栈对象来控制一块堆资源
```

---

## 7. 手动管理和智能指针对比

### 7.1 手动管理

```cpp
void test() {
  int* p = new int(10);

  std::cout << *p << std::endl;

  delete p;
}
```

问题：

```text
可能忘记 delete
可能重复 delete
可能 delete 后继续使用
异常时 delete 可能执行不到
```

---

### 7.2 使用智能指针

```cpp
#include <memory>
#include <iostream>

void test() {
  std::unique_ptr<int> p = std::make_unique<int>(10);

  std::cout << *p << std::endl;
}
```

函数结束时，`p` 自动销毁，它管理的堆内存也会自动释放。

优点：

```text
不需要手动 delete
减少内存泄漏
减少重复释放
减少悬挂指针
异常情况下也能正常释放资源
```

---

## 8. 常见智能指针

C++ 标准库中的智能指针需要包含头文件：

```cpp
#include <memory>
```

常见的智能指针有三种：

```text
std::unique_ptr
std::shared_ptr
std::weak_ptr
```

---

## 9. std::unique_ptr

`std::unique_ptr` 表示独占所有权。

意思是：

```text
同一时间只能有一个 unique_ptr 管理同一块资源
```

示例：

```cpp
std::unique_ptr<int> p = std::make_unique<int>(10);
```

`unique_ptr` 不能复制：

```cpp
std::unique_ptr<int> p1 = std::make_unique<int>(10);

// std::unique_ptr<int> p2 = p1;  // 错误
```

但是可以移动：

```cpp
std::unique_ptr<int> p1 = std::make_unique<int>(10);
std::unique_ptr<int> p2 = std::move(p1);
```

移动后：

```text
p2 接管资源
p1 不再拥有资源
```

适合场景：

```text
资源只有一个明确的拥有者
不需要多个对象共享
```

---

## 10. std::shared_ptr

`std::shared_ptr` 表示共享所有权。

意思是：

```text
多个 shared_ptr 可以共同管理同一块资源
最后一个 shared_ptr 销毁时，资源才会释放
```

示例：

```cpp
std::shared_ptr<int> p1 = std::make_shared<int>(10);
std::shared_ptr<int> p2 = p1;
```

此时 `p1` 和 `p2` 管理同一块堆内存。

可以查看引用计数：

```cpp
std::cout << p1.use_count() << std::endl;
```

特点：

```text
可以复制
内部有引用计数
最后一个拥有者销毁时释放资源
```

适合场景：

```text
资源需要被多个对象共享
资源生命周期不容易由单一对象决定
```

---

## 11. std::weak_ptr

`std::weak_ptr` 通常配合 `std::shared_ptr` 使用。

它表示弱引用。

特点：

```text
不拥有资源
不会增加引用计数
不能直接访问对象
需要先通过 lock() 转成 shared_ptr
```

示例：

```cpp
std::shared_ptr<int> sp = std::make_shared<int>(10);
std::weak_ptr<int> wp = sp;

if (auto p = wp.lock()) {
  std::cout << *p << std::endl;
}
```

`weak_ptr` 常用于解决 `shared_ptr` 的循环引用问题。

---

## 12. 为什么推荐 make_unique 和 make_shared

推荐写法：

```cpp
auto p1 = std::make_unique<int>(10);
auto p2 = std::make_shared<int>(20);
```

不推荐频繁写：

```cpp
std::unique_ptr<int> p1(new int(10));
std::shared_ptr<int> p2(new int(20));
```

原因：

```text
写法更简洁
异常安全性更好
make_shared 通常性能更好
减少裸 new 的出现
```

现代 C++ 中，应尽量少直接使用裸 `new` 和裸 `delete`。

---

## 13. ROS 2 中的智能指针

在 ROS 2 C++ 节点中，经常会看到：

```cpp
auto node = std::make_shared<PersonNode>("person_node", "张三", 18);
```

这里创建的是：

```cpp
std::shared_ptr<PersonNode>
```

可以理解为：

```text
在堆上创建一个 PersonNode 对象
用 shared_ptr 管理这个对象
node 离开作用域后自动释放
```

ROS 2 中常用 `shared_ptr`，是因为节点对象可能会被多个 ROS 2 内部对象或组件共同使用，例如：

```text
executor
timer
publisher
subscription
callback
```

所以节点对象的生命周期不总是由某一个单独对象决定。

---

## 14. 结合当前 PersonNode 代码

例如：

```cpp
int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);

  auto node = std::make_shared<PersonNode>("person_node", "张三", 18);

  node->eat("鱼香肉丝");

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
```

这里：

```cpp
auto node = std::make_shared<PersonNode>("person_node", "张三", 18);
```

做了两件事：

```text
1. 创建 PersonNode 对象
2. 用 shared_ptr 自动管理这个对象
```

因为 `node` 是智能指针，所以访问成员函数时使用：

```cpp
node->eat("鱼香肉丝");
```

`node->eat(...)` 等价于：

```cpp
(*node).eat(...);
```

---

## 15. 总结

引入智能指针的原因：

```text
手动管理内存风险高
new -> use -> delete 容易出错
```

手动管理内存的常见问题：

```text
内存泄漏
悬挂指针
重复释放
异常导致资源没有释放
```

智能指针的本质：

```text
智能指针是封装了原始 C++ 指针的类模板
它通过对象的析构函数自动释放资源
```

核心原理：

```text
RAII
```

RAII 的理解：

```text
栈上的对象生命周期自动管理
堆上的资源需要手动管理
RAII 利用栈对象的生命周期来管理堆资源
```

一句话记忆：

```text
智能指针就是把堆资源交给一个栈对象管理，栈对象销毁时自动释放堆资源。
```

常用智能指针：

```text
unique_ptr: 独占资源
shared_ptr: 共享资源
weak_ptr: 弱引用，不拥有资源
```

现代 C++ 建议：

```text
优先使用智能指针
尽量避免裸 new 和裸 delete
优先使用 make_unique 和 make_shared
```
