cmake_minimum_required(VERSION 3.16)
project(localization_fusion)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(eurobot_common REQUIRED)
find_package(eurobot_interfaces REQUIRED)

add_executable(fusion_node src/fusion_node.cpp)
target_include_directories(fusion_node PRIVATE include)
target_link_libraries(fusion_node eurobot_common::eurobot_common)
ament_target_dependencies(fusion_node rclcpp geometry_msgs eurobot_interfaces)

install(TARGETS fusion_node DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_circle_path test/test_circle_path.cpp)
  target_include_directories(test_circle_path PRIVATE include)
endif()

ament_package()
