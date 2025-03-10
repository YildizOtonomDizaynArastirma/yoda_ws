from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
from launch.substitutions import Command
from launch_ros.parameter_descriptions import ParameterValue
import os

def generate_launch_description():
    package_name = 'yoda_cpp_package'

    # SDF dosyasının tam yolunu belirle
    yoda_car_sdf_file_path = "/home/safa/yoda_ws/src/yoda_cpp_package/urdf/model.urdf"
    yoda_map_sdf_file_path = "/home/safa/yoda_ws/src/yoda_cpp_package/models/track_model.sdf"
    control_yaml = os.path.join(
        get_package_share_directory(package_name),
        'config',
        'yoda_car_control.yaml'
    )

    return LaunchDescription([
        # Gazebo'yu başlat
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource([os.path.join(
                get_package_share_directory('gazebo_ros'), 'launch'), '/gazebo.launch.py']),
        ),

        # Yoda modelini spawn et
        Node(
            package='gazebo_ros',
            executable='spawn_entity.py',
            name='spawn_yoda',
            arguments=['-entity', 'yoda_car', '-file', yoda_car_sdf_file_path, '-x', '0', '-y', '0', '-z', '0'],
            parameters=[{'use_sim_time': True}],
            output='screen'
        ),

        # Map modelini spawn et
        Node(
            package='gazebo_ros',
            executable='spawn_entity.py',
            name='spawn_map',
            parameters=[{'use_sim_time': True}],
            arguments=[
                '-entity', 'track_map',  # Entity ismini değiştirdim
                '-file', yoda_map_sdf_file_path,  # SDF dosya yolunu belirttim
                '-x', '0', '-y', '0', '-z', '0'
            ],
            output='screen'
        ),

        # Static Transform Publisher ekle
        Node(
            package='tf2_ros',
            executable='static_transform_publisher',
            name='static_transform_publisher',
            parameters=[{'use_sim_time': True}],
            arguments=['0', '0', '0', '0', '0', '0', 'map', 'base_link', '--ros-args', '--log-level', 'info']
        ),

        # Yoda bridge node'unu başlat
        Node(
            package=package_name,
            executable='yoda_bridge',
            name='yoda_bridge',
            output='screen',
            parameters=[{'use_sim_time': True}],
            arguments=['--ros-args', '--log-level', 'info']  # Log seviyesini info olarak ayarlıyoruz
        ),

        # Controller Manager parametrelerini yükle
        Node(
            package='controller_manager',
            executable='ros2_control_node',
            parameters=[control_yaml],
            output='screen'
        ),

    ])
