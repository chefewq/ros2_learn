#include "rclcpp/rclcpp.hpp"

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);

  auto node = rclcpp::Node::make_shared("ros_cpp_node");
  RCLCPP_INFO(node->get_logger(), "你好 C++ 节点!");

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}