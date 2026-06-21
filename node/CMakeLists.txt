cmake_minimum_required(VERSION 3.10)
project(ros_cpp_node)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

add_executable(ros_cpp_node src/ros2_cpp_node.cpp)

# 为ros_cpp_node添加依赖项 需要完善package.xml文件
ament_target_dependencies(ros_cpp_node rclcpp)

install(TARGETS
  ros_cpp_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
