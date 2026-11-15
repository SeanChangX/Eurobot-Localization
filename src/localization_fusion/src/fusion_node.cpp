// Localization fusion node.
//
//   LOCALIZATION_POSE   -> navigation, main   (rate_hz, sensor QoS)
//   VISION_RIVAL_POSES  <- vision             (only to measure its latency here)
//
// Latency check: vision stamps its message with the capture time; the age on
// arrival (eurobot_common::stamp_age_ms) is the end-to-end camera delay that
// 2026 struggled with. It is reported in the heartbeat so it shows up in the
// logs and on the status overview, instead of being guessed.
#include <chrono>
#include <cstdio>
#include <memory>
#include <string>

#include <geometry_msgs/msg/pose_array.hpp>
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include <rclcpp/rclcpp.hpp>

#include "eurobot_common/contract.hpp"
#include "eurobot_common/latency.hpp"
#include "eurobot_common/startup_reporter.hpp"
#include "localization_fusion/circle_path.hpp"

namespace contract = eurobot_common::contract;
using eurobot_interfaces::msg::ModuleHeartbeat;
using geometry_msgs::msg::PoseArray;
using geometry_msgs::msg::PoseWithCovarianceStamped;

namespace localization_fusion {

class FusionNode : public rclcpp::Node {
public:
  FusionNode()
      : Node("fusion"),
        path_(declare_parameter("center_x", 1.5),
              declare_parameter("center_y", 1.0),
              declare_parameter("radius", 0.6),
              declare_parameter("angular_speed", 0.3)),
        frame_id_(declare_parameter("frame_id", std::string("map"))),
        latency_warn_ms_(declare_parameter("rival_latency_warn_ms", 100.0)),
        started_(std::chrono::steady_clock::now()) {
    pose_pub_ = create_publisher<PoseWithCovarianceStamped>(contract::LOCALIZATION_POSE.name,
                                                            contract::LOCALIZATION_POSE.qos());
    rival_sub_ = create_subscription<PoseArray>(contract::VISION_RIVAL_POSES.name,
                                                contract::VISION_RIVAL_POSES.qos(),
                                                [this](const PoseArray & msg) { on_rival(msg); });

    const double rate = declare_parameter("rate_hz", 50.0);
    timer_ = create_wall_timer(std::chrono::duration<double>(1.0 / rate), [this] { publish(); });

    reporter_ = std::make_unique<eurobot_common::StartupReporter>(*this, "localization", "fusion");
    reporter_->ready();
  }

private:
  void publish() {
    // REPLACE: this is where the real filter's latest estimate goes.
    const double t =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - started_).count();
    const Pose2D p = path_.at(t);

    PoseWithCovarianceStamped msg;
    msg.header.stamp = now();
    msg.header.frame_id = frame_id_;
    msg.pose.pose.position.x = p.x;
    msg.pose.pose.position.y = p.y;
    msg.pose.pose.orientation.z = std::sin(p.yaw / 2);
    msg.pose.pose.orientation.w = std::cos(p.yaw / 2);
    msg.pose.covariance[0] = msg.pose.covariance[7] = 0.01 * 0.01;  // x, y: 1 cm sigma
    msg.pose.covariance[35] = 0.02 * 0.02;                          // yaw: ~1 deg sigma
    pose_pub_->publish(msg);
  }

  void on_rival(const PoseArray & msg) {
    const double age_ms = eurobot_common::stamp_age_ms(*this, msg.header.stamp);
    char detail[64];
    std::snprintf(detail, sizeof(detail), "rival latency %.1f ms", age_ms);
    reporter_->set_health(age_ms > latency_warn_ms_ ? ModuleHeartbeat::WARN : ModuleHeartbeat::OK,
                          detail);
    RCLCPP_DEBUG(get_logger(), "%s", detail);
  }

  CirclePath path_;
  std::string frame_id_;
  double latency_warn_ms_;
  std::chrono::steady_clock::time_point started_;
  rclcpp::Publisher<PoseWithCovarianceStamped>::SharedPtr pose_pub_;
  rclcpp::Subscription<PoseArray>::SharedPtr rival_sub_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::unique_ptr<eurobot_common::StartupReporter> reporter_;
};

}  // namespace localization_fusion

int main(int argc, char ** argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<localization_fusion::FusionNode>());
  rclcpp::shutdown();
  return 0;
}
