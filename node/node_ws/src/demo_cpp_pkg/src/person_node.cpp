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
