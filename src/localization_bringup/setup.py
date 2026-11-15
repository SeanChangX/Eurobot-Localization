from glob import glob

from setuptools import setup

package_name = 'localization_bringup'

setup(
    name=package_name,
    version='0.1.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/launch', glob('launch/*.launch.py')),
        ('share/' + package_name + '/config', glob('config/*.yaml')),
        ('share/' + package_name + '/config/robots', glob('config/robots/*.yaml')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    extras_require={'test': ['pytest']},
    maintainer='DIT Robotics',
    maintainer_email='dit.robotics@gmail.com',
    description='Launch file and per-robot parameters of the localization domain.',
    license='MIT',
)
