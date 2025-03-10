#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "gazebo_msgs/srv/set_model_state.hpp"

class YodaBridge : public rclcpp::Node
{
public:
  YodaBridge() : Node("yoda_bridge"), camera_enabled_(false), lidar_enabled_(false), imu_enabled_(false)
  {
    // Create service clients
    speed_client_ = this->create_client<gazebo_msgs::srv::SetModelState>("/gazebo/set_model_state");
    steering_client_ = this->create_client<gazebo_msgs::srv::SetModelState>("/gazebo/set_model_state");

    // Create subscribers
    cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
      "/yoda/cmd_vel", 10, std::bind(&YodaBridge::cmd_vel_callback, this, std::placeholders::_1));

    // Sensor subscribers with updated topic names
    camera_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
      "/yoda/camera/image_raw", 10, std::bind(&YodaBridge::camera_callback, this, std::placeholders::_1));
    
    lidar_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
      "/yoda/lidar/scan", 10, std::bind(&YodaBridge::lidar_callback, this, std::placeholders::_1));
    
    imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
      "/yoda/imu/data", 10, std::bind(&YodaBridge::imu_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "YodaBridge initialized and waiting for sensor data.");
  }

private:
  void cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg)
  {
    auto speed_request = std::make_shared<gazebo_msgs::srv::SetModelState::Request>();
    auto steering_request = std::make_shared<gazebo_msgs::srv::SetModelState::Request>();

    speed_request->model_state.model_name = "yoda";
    steering_request->model_state.model_name = "yoda";

    speed_request->model_state.twist.linear.x = msg->linear.x;
    steering_request->model_state.twist.angular.z = msg->angular.z;

    if (speed_client_->wait_for_service(std::chrono::seconds(1)))
    {
      speed_client_->async_send_request(speed_request);
    }

    if (steering_client_->wait_for_service(std::chrono::seconds(1)))
    {
      steering_client_->async_send_request(steering_request);
    }

    RCLCPP_INFO(this->get_logger(), "Received cmd_vel - linear: %f, angular: %f", msg->linear.x, msg->angular.z);
  }

  void camera_callback(const sensor_msgs::msg::Image::SharedPtr msg)
  {
      if (!camera_enabled_) {
          RCLCPP_INFO(this->get_logger(), "[Camera] Enabled - Resolution: %dx%d", msg->width, msg->height);
          camera_enabled_ = true;
      }
  }

  void lidar_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
  {
      if (!lidar_enabled_) {
          RCLCPP_INFO(this->get_logger(), "[LiDAR] Enabled - %zu ranges detected.", msg->ranges.size());
          lidar_enabled_ = true;
      }
  }

  void imu_callback(const sensor_msgs::msg::Imu::SharedPtr msg)
  {
      if (!imu_enabled_) {
          RCLCPP_INFO(this->get_logger(), "[IMU] Enabled - Orientation: x=%.2f, y=%.2f, z=%.2f, w=%.2f",
                      msg->orientation.x, msg->orientation.y, msg->orientation.z, msg->orientation.w);
          imu_enabled_ = true;
      }
  }

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr camera_sub_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr lidar_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
  rclcpp::Client<gazebo_msgs::srv::SetModelState>::SharedPtr speed_client_;
  rclcpp::Client<gazebo_msgs::srv::SetModelState>::SharedPtr steering_client_;
  
  bool camera_enabled_;
  bool lidar_enabled_;
  bool imu_enabled_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<YodaBridge>());
  rclcpp::shutdown();
  return 0;
}
