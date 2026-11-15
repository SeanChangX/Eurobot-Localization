"""Start the localization domain: ros2 launch localization_bringup bringup.launch.py robot:=<robot>.

The localization container runs exactly this (see docker/Dockerfile in the shell repo),
so what you test with `ros2 launch` on your laptop is what runs on the robot.
"""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from localization_bringup.params import param_files


def nodes(context):
    robot = LaunchConfiguration('robot').perform(context)
    params = param_files(get_package_share_directory('localization_bringup'), robot)
    return [
        Node(
            package='localization_fusion',
            executable='fusion_node',
            name='fusion',
            parameters=params,
            output='screen',
            emulate_tty=True,
        ),
    ]


def generate_launch_description():
    return LaunchDescription(
        [
            DeclareLaunchArgument(
                'robot',
                default_value=os.environ.get('ROBOT', 'sim'),
                description='config/robots/<robot>.yaml overrides config/default.yaml',
            ),
            OpaqueFunction(function=nodes),
        ]
    )
