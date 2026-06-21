#include <functional>
#include <iostream>

int main() {
  // 定义一个 lambda 表达式
  auto lambda = [](int x, int y) { return x + y; };

  // 调用 lambda 表达式
  std::cout << lambda(5, 3) << std::endl;

  return 0;
}
