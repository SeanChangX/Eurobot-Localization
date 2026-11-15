// Fake "fusion" output: the robot drives a circle. Exists only so the rest of
// the stack has a realistic pose stream to work with. Replace CirclePath in
// fusion_node.cpp with the real estimator (wheel odometry + IMU + lidar beacons,
// camera), keeping the output topic and its header.stamp semantics.
#pragma once

#include <cmath>

namespace localization_fusion {

struct Pose2D {
  double x;
  double y;
  double yaw;
};

inline double normalize_angle(double a) {
  return std::atan2(std::sin(a), std::cos(a));
}

class CirclePath {
public:
  CirclePath(double center_x, double center_y, double radius, double angular_speed)
      : cx_(center_x), cy_(center_y), r_(radius), w_(angular_speed) {}

  // Pose `t` seconds after the start; heading is tangent to the circle.
  Pose2D at(double t) const {
    const double a = w_ * t;
    const double heading = a + (w_ >= 0 ? M_PI_2 : -M_PI_2);
    return {cx_ + r_ * std::cos(a), cy_ + r_ * std::sin(a), normalize_angle(heading)};
  }

private:
  double cx_, cy_, r_, w_;
};

}  // namespace localization_fusion
