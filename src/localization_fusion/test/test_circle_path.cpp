#include <gtest/gtest.h>

#include <cmath>

#include "localization_fusion/circle_path.hpp"

using localization_fusion::CirclePath;
using localization_fusion::normalize_angle;

TEST(CirclePath, StaysOnTheCircle) {
  CirclePath path(1.5, 1.0, 0.6, 0.3);
  for (double t = 0; t < 30; t += 0.7) {
    auto p = path.at(t);
    EXPECT_NEAR(std::hypot(p.x - 1.5, p.y - 1.0), 0.6, 1e-9);
  }
}

TEST(CirclePath, HeadingIsTangent) {
  CirclePath path(0, 0, 1, 1.0);
  auto p = path.at(0);  // at (1, 0) driving counter-clockwise -> heading +90 deg
  EXPECT_NEAR(p.yaw, M_PI_2, 1e-9);
}

TEST(NormalizeAngle, WrapsIntoPlusMinusPi) {
  EXPECT_NEAR(normalize_angle(3 * M_PI), M_PI, 1e-9);
  EXPECT_NEAR(normalize_angle(-M_PI_2 - 2 * M_PI), -M_PI_2, 1e-9);
}
