cmake_minimum_required(VERSION 3.8)
project(yoda_cpp_package)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(gazebo_ros REQUIRED) # Gazebo ROS2 paketi için
find_package(std_srvs REQUIRED)  # std_srvs paketi için ekleyin
find_package(gazebo_msgs REQUIRED)

# Include directories
include_directories(include)

# Declare C++ executable
add_executable(yoda_bridge src/yoda_bridge.cpp)

# Specify libraries to link executable target against
ament_target_dependencies(yoda_bridge
  rclcpp
  std_msgs
  sensor_msgs
  geometry_msgs
  gazebo_ros
  std_srvs
  gazebo_msgs
)

# Install targets
install(TARGETS
  yoda_bridge
  DESTINATION lib/${PROJECT_NAME}
)


install(
  DIRECTORY urdf
  DESTINATION share/${PROJECT_NAME}
  PATTERN "*.urdf"
  PATTERN "*.dae"
)

# Install launch files
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/launch
)

ament_package()
