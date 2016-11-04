//
// Created by Masayuki IZUMI on 10/21/16.
//

#include <stdexcept>
#include <thread>

#include "SensorDevice.h"

SensorDevice::SensorDevice(const std::string &uri) {
  initialize(uri);
}

SensorDevice::~SensorDevice() {
  if (user_tracker_.isValid()) {
    user_tracker_.destroy();
  }
}

void SensorDevice::checkStatus(nite::Status status, std::string msg) {
  checkStatus(status == nite::STATUS_OK, msg);
}

void SensorDevice::checkStatus(bool is_ok, std::string msg) {
  if (!is_ok) {
    throw std::runtime_error(msg);
  }
}

void SensorDevice::initialize(const std::string &uri) {
  if (device_.open(uri.empty() ? openni::ANY_DEVICE : uri.c_str()) != openni::STATUS_OK) {
    throw std::runtime_error("Failed to open openni::Device.");
  }
  checkStatus(user_tracker_.create(&device_), "Failed to create nite::UserTracker.");
}

void SensorDevice::update() {
  nite::UserTrackerFrameRef user_frame;
  checkStatus(user_tracker_.readFrame(&user_frame), "Failed to read user frame.");
  auto depth_frame = user_frame.getDepthFrame();

  if (depth_frame.isValid()) {
    auto image = cv::Mat(depth_frame.getHeight(), depth_frame.getWidth(), CV_16UC1, (unsigned short*) depth_frame.getData());
    image.copyTo(image_);

    const auto &users = user_frame.getUsers();
    skeletons_.clear();

    for (int i = 0; i < users.getSize(); i++) {
      const auto &user = users[i];
      if (user.isNew()) {
        checkStatus(user_tracker_.startSkeletonTracking(user.getId()), "Failed to start tracking skeleton.");
      } else if (!user.isLost()) {
        const auto &skeleton = user.getSkeleton();
        if (skeleton.getState() != nite::SKELETON_TRACKED) { continue; }
        Skeleton sk = { user.getId(), {} };
        for (int j = 0; j <= 14; j++) {
          nite::JointType type = (nite::JointType) j;
          const auto &joint = skeleton.getJoint(type);
          if (joint.getPositionConfidence() < 0.7f) { continue; }
          const auto &pos = joint.getPosition();
          float x = 0, y = 0;
          checkStatus(user_tracker_.convertJointCoordinatesToDepth(pos.x, pos.y, pos.z, &x, &y),
                      "Failed to convert joint coords to depth.");
          sk.joints.push_back({ x, y, type });
        }
        skeletons_.emplace_back(sk);
      }
    }
  }
}
