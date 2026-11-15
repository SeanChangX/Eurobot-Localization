# Eurobot-Localization

The **localization** domain of the DIT Eurobot software. Where the robot is: one fused pose for everyone else.

This repo is not built on its own: it is checked out into the workspace
([SeanChangX/DIT-X](https://github.com/SeanChangX/DIT-X)) by `ditx sync`,
built on top of the shared base image (ROS 2 + `eurobot_interfaces` + `eurobot_common`)
and runs in its own container on the robot.

## Packages

| package | what |
|---|---|
| `localization_bringup` | launch + per-robot parameters |
| `localization_fusion` | C++ — publishes the fused pose (fake circle for now) and measures the camera latency |

## Interface

Everything that crosses into another domain is registered in the workspace's
[`contract.yaml`](https://github.com/SeanChangX/DIT-X/blob/develop/base/eurobot_common/contract.yaml)
(name, type, QoS, owner). Use the generated constants (`contract::LOCALIZATION_POSE.name`,
`contract.LOCALIZATION_POSE.qos()`), never a string literal.

| | topic / service | type | with |
|---|---|---|---|
| publishes | `/robot/localization/pose` | PoseWithCovarianceStamped (sensor QoS, 50 Hz) | navigation, main |
| subscribes | `/robot/vision/rival_poses` | PoseArray | vision |

Every module also sends a heartbeat and reports ready through `eurobot_common::StartupReporter`
(Python: `eurobot_common.startup.StartupReporter`).

## Develop

From the workspace root (`eurobot/`):

```bash
ditx build -d localization                 # incremental build in the dev container
ditx test -d localization                  # unit tests
ditx test --integration           # all domains together: handshake + contract check
ditx deploy -d localization -r sim         # run your build on the simulated robot (this laptop)
ditx deploy -d localization -r white       # push only your build output to the robot; auto rollback on failure
```

Or just run `ditx` and pick from the menu.

## Where to change things

- **The fake parts:** `CirclePath` in `fusion_node.cpp` fakes the estimate. Replace it with the real filter (wheel odometry + IMU + lidar beacons + camera). Keep `header.stamp` = time of the newest fused measurement: navigation uses the pose age to decide its speed. The rival-latency check shows how to measure end-to-end delay with `eurobot_common::stamp_age_ms` (it lands in the heartbeat, i.e. in the logs).
- **Parameters:** `localization_bringup/config/default.yaml`; per robot only the differences in
  `config/robots/<robot>.yaml` (a robot is a config file, never a branch).
- **New package:** `ditx new localization localization_<name> --lang cpp|python`, then add it to
  `localization_bringup/launch/bringup.launch.py`.
- **New cross-domain topic:** add it to `contract.yaml` in the workspace repo first (PR there),
  then use the generated constant here.

## Rules

- Package names start with `localization_`. Depend only on base packages and ROS/system packages, never on
  another domain's package (`ditx lint` checks it).
- Conventional Commits (`feat(fusion): ...`, `fix: ...`), branches `feat/*` / `fix/*` → `develop` → `main`.
- Update `CHANGELOG.md` under *Unreleased* in the same PR.
