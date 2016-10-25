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
  user_tracker_.removeNewFrameListener(this);
  if (user_tracker_.isValid()) {
    user_tracker_.destroy();
  }
}

void SensorDevice::onNewFrame(nite::UserTracker &tracker) {
  update();
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
  user_tracker_.addNewFrameListener(this);
}

void SensorDevice::update() {
  static const cv::Scalar colors[] = {
    cv::Scalar(1, 0, 0),
    cv::Scalar(0, 1, 0),
    cv::Scalar(0, 0, 1),
    cv::Scalar(1, 1, 0),
    cv::Scalar(1, 0, 1),
    cv::Scalar(0, 1, 1),
    cv::Scalar(0.5, 0, 0),
    cv::Scalar(0, 0.5, 0),
    cv::Scalar(0, 0, 0.5),
    cv::Scalar(0.5, 0.5, 0),
  };
  nite::UserTrackerFrameRef user_frame;
  checkStatus(user_tracker_.readFrame(&user_frame), "Failed to read user frame.");
  auto depth_frame = user_frame.getDepthFrame();

  if (depth_frame.isValid()) {
    auto image = cv::Mat(depth_frame.getHeight(), depth_frame.getWidth(), CV_8UC4);
    auto depth = (openni::DepthPixel *) depth_frame.getData();
    const auto user_labels = user_frame.getUserMap().getPixels();

    image.forEach<cv::Vec3b>([&](cv::Vec3b &p, const int position[2]){
      int i = position[0] * depth_frame.getWidth() + position[1];
      auto label = user_labels[i];
      if (label != 0) {
        p[0] *= colors[label][0];
        p[1] *= colors[label][1];
        p[2] *= colors[label][2];
      } else {
        auto gray = static_cast<unsigned char>(~((depth[i] * 255) / 10000));
        p[0] = gray;
        p[1] = gray;
        p[2] = gray;
      }
    });

    const auto &users = user_frame.getUsers();

    for (int i = 0; i < users.getSize(); i++) {
      const auto &user = users[i];

      if (user.isNew()) {
        checkStatus(user_tracker_.startSkeletonTracking(user.getId()), "Failed to start tracking skeleton.");
      } else if (!user.isLost()) {
        const auto &skeleton = user.getSkeleton();
        if (skeleton.getState() != nite::SKELETON_TRACKED) { continue; }
        for (int j = 0; j <= 14; j++) {
          const auto &joint = skeleton.getJoint((nite::JointType) j);
          if (joint.getPositionConfidence() < 0.7f) { continue; }
          const auto &pos = joint.getPosition();
          float x = 0, y = 0;
          checkStatus(user_tracker_.convertJointCoordinatesToDepth(pos.x, pos.y, pos.z, &x, &y),
                      "Failed to convert joint coords to depth.");
          cv::circle(image, cvPoint((int) x, (int) y), 5, cv::Scalar(0, 0, 255), -1);
        }
      }
    }

    image.copyTo(image_);
  }
}
