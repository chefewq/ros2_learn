# 鱼香ROS2 学习

## 第一章ROS的基本组件



## node

 功能包组织ros2 节点

```shell
#cmake 版本
ros pkg create name --build-type ament_cmake --license Apache-2.0
#python 版本
ros pkg create name --build-type ament_python --license Apache-2.0
```



### **c++版本生成节点**

````c++
#include "rclcpp/rclcp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("demo_cpp_node");
  RCLCPP_INFO(node->get_logger(), "Hello, ROS2 from C++!");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
````

同时需要正确设置 CMakeLists.txt

```cmake
add_executable(demo_cpp_node src/demo_cpp_node.cpp)

ament_target_dependencies(demo_cpp_node rclcpp)

install(TARGETS
  demo_cpp_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()
```

正确设置 package.xml

````xml
  <buildtool_depend>ament_cmake</buildtool_depend>
  <depend>rclcpp</depend>
````

### python 版本生成节点

````python
import rclpy 
from rclpy.node import Node

def main(args=None):
    rclpy.init(args=args)
    node = Node('python_node')
    node.get_logger().info('Hello from Python node!')
    rclpy.spin(node)
    rclpy.shutdown()
````

package.xml基本配置同c++版本

最后导出部分有差异

```xml
  <export>
    <build_type>ament_python</build_type>
  </export>
```

还有setup.py 的setup() 中需要设置进入点

```python
    entry_points={
        'console_scripts': [
            'python_node = demo_python_pkg.python_node:main',
        ],
    },
```



### ROS2 工作空间

```shell
mkdir -p node_ws/src
```

colcon build 会扫描 src路径中的所有包并进行编译

在package.xml 中声明依赖项即可控制不同包的构建顺序，以及依赖控制

```xml
 <!-- 在demo_python_pkg中加入即可依赖cpp_pkg -->
<depend>demo_cpp_pkg</depend>
```

### ROS2 的基础编程

#### 面向对象

无论是c++还是python 一般不会仅仅创建一个普通的node节点，因为业务需要自定义，所以需要继承node节点然后针对业务创建节点

##### **python 节点的声明方法**

```python
import rclpy
from rclpy.node import Node
##父类是Node 子类是PersonNode 实现比c++更简单
class PersonNode(Node):
    def __init__(self, node_name: str, name: str, age: int) -> None:
        super().__init__(node_name)# 调用Node的构造函数 没有无法调用Node的方法 
        self.age = age
        self.name = name

    def eat(self, food_name: str):
        self.get_logger().info(f'我叫{self.name},今年{self.age}岁，我现在正在吃{food_name}')

def main():
    rclpy.init()
    node = PersonNode('person_node', '法外狂徒张三', '18')
    node.eat('鱼香肉丝')
    rclpy.spin(node)
    rclpy.shutdown()
```

##### **c++节点的声明方法**

c++的继承比python的更加严格

````c++
#include "rclcpp/rclcpp.hpp"

class PersonNode : public rclcpp::Node {
 public:
  PersonNode(const std::string& node_name, const std::string& name, const int age)
      : Node(node_name), name(name), age(age) {
    RCLCPP_INFO(this->get_logger(), "PersonNode 的构造函数被调用了");
  }

  void eat(const std::string& food_name) {
    RCLCPP_INFO(this->get_logger(), "我叫%s，今年%d岁，我现在正在吃%s", name.c_str(), age,
                food_name.c_str());
  }

 private:
  std::string name;
  int age;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PersonNode>("person_node", "张三", 18);
  node->eat("鱼香肉丝");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}

````

#### **八股文知识**

##### **c++类的内部权限**

public:    类内部、子类、外部 都能访问
protected: 类内部、子类 能访问，外部不能访问
private:   只有类内部能访问，子类和外部都不能访问

c++的继承分为哪几类？区别是什么？

按权限分为三类

public:    不改变 成员的权限 private  子类无法访问 （用的最多 ） 子类就是父类的一种
protected:  将public成员的权限改为 proteced（用的很少）
private:   将public和protected 都改为private （） 子类只是借用父类实现   并不是父类

按结构分为 五类

单继承 多继承 多层继承 菱形继承 虚继承

菱形继承有二义性问题 存在多个祖父类的属性 虚继承主要用于解决菱形继承有二义性问题

#### c++新特性

##### 自动类型推导

通过auto 关键字可以不用提前声明变量类型

```c++
#include <iostream>

int main()
{
    auto x = 5;
    auto y = 3.14;
    auto z = 'a';

    std::cout << x << std::endl;
    std::cout << y << std::endl;
    std::cout << z << std::endl;

    return 0;
}

```

##### 智能指针

**<u>为啥引入智能指针：</u>**

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

**智能指针的本质**

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

**RAII原理**

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

**栈和堆的关系**

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

**常见智能指针**

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

**std::unique_ptr**

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

**std::shared_ptr**

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

**std::weak_ptr**

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

**推荐写法**

推荐：

```cpp
auto p1{std::make_unique<int>(10)};
auto p2{std::make_shared<int>(20)};
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

**ROS 2 中的例子**

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

**总结**

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

##### Lambda表达式 （匿名函数）

可以不写函数名字，但是正常调用

格式

````
[capture list](parameters) -> return_typer {fucthion body}
````

示例

```c++
#include <iostream>
#include <algorithm>

int main()
{
    auto add = [](int a, int b) -> int { return a + b; };
    int sum = add(3, 5);
    auto print_sum = [sum]()->void { std::cout << "3 + 5 = " << sum << std::endl; };
    print_sum();
    return 0;
}

```

##### 函数包装器

基本格式

```
std::function<返回值类型(参数类型列表)> 变量名;
```

可以作为函数参数 



用于实现回调函数

````c++
#include <functional>
#include <iostream>

class Button {
public:
  void set_on_click(std::function<void()> callback) {
    on_click = callback;
  }

  void click() {
    if (on_click) {
      on_click();
    }
  }

private:
  std::function<void()> on_click;
};

int main() {
  Button button;

  button.set_on_click([]() {
    std::cout << "button clicked" << std::endl;
  });

  button.click();

  return 0;
}
````



可以用bind绑定类的成员函数 也可以用Lambda实现



```c++
#include <functional>
#include <iostream>

class Person {
public:
  void say() {
    std::cout << "hello" << std::endl;
  }
};

class FileSave {
 public:
  void save_with_member_fun(const std::string& file_name) {
    std::cout << "调用了成员方法，保存:" << file_name << std::endl;
  };
};

int main() {
  Person person;
	FileSave file_save;
  std::function<void()> f = [&person]() {
    person.say();
  };
  
  std::function<void()> f2 = std::bind(&Person::say, &person); //这是无參函数 有參函数需要占位
  std::function<void(const std::string &)> save3 = std::bind(&FileSave::save_with_member_fun, &file_save, std::placeholders::_1);

  f();

  return 0;
}
```

#### 多线程与回调函数

c++ 创建线程 格式为

```
std::thread name(函数包装器对象,parameter);
```

```
std::thread name(class::function,obj);
```



## Topic

## Service

## Action

## Parameter