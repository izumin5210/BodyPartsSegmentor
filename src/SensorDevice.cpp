//
// Created by Masayuki IZUMI on 10/21/16.
//

#include <stdexcept>
#include <string>
#include <thread>

#include "SensorDevice.h"

SensorDevice::SensorDevice(const char *uri) {
  initialize(uri);
}

SensorDevice::~SensorDevice() {
  user_tracker_.removeNewFrameListener(this);
  if (user_tracker_.isValid()) {
    user_tracker_.destroy();
  }
  if (worker_.joinable()) {
    worker_.join();
  }
}

void SensorDevice::onNewFrame(nite::UserTracker &tracker) {
  if (worker_.joinable()) {
    worker_.join();
  }
  worker_ = std::thread([&]{
    update();
  });
}

void SensorDevice::checkStatus(openni::Status status, std::string msg) {
  checkStatus(status == openni::STATUS_OK, msg);
}

void SensorDevice::checkStatus(nite::Status status, std::string msg) {
  checkStatus(status == nite::STATUS_OK, msg);
}

void SensorDevice::checkStatus(bool is_ok, std::string msg) {
  if (!is_ok) {
    throw std::runtime_error(msg);
  }
}

void SensorDevice::initialize(const char *uri) {
  std::cout << uri << std::endl;
  checkStatus(device_.open(uri), "Failed to open openni::Device.");
  startDepthStream();
  checkStatus(user_tracker_.create(&device_), "Failed to create nite::UserTracker.");
  user_tracker_.addNewFrameListener(this);
}

void SensorDevice::startDepthStream() {
  checkStatus(depth_stream_.create(device_, openni::SENSOR_DEPTH), "Failed to create depth stream.");
  const auto supported_video_modes = &(depth_stream_.getSensorInfo().getSupportedVideoModes());
  checkStatus(depth_stream_.setVideoMode((*supported_video_modes)[1]), "Failed to set video mode to depth stream.");
  checkStatus(depth_stream_.start(), "Failed to start depth stream.");
}

void SensorDevice::update() {
  nite::UserTrackerFrameRef user_frame;
  checkStatus(user_tracker_.readFrame(&user_frame), "Failed to read user frame.");
  auto depth_frame = user_frame.getDepthFrame();
  if (depth_frame.isValid()) {
    cv::Rect roi(0, 0, 512, 424);
    auto depth_image = cv::Mat(depth_frame.getHeight(), depth_frame.getWidth(), CV_16UC1, (unsigned char *) depth_frame.getData())(roi);
    auto image = cv::Mat(512, 424, CV_8UC4);
    const auto user_labels = user_frame.getUserMap().getPixels();

    for (int y = 0; y < 424; y++) {
      for (int x = 0; x < 512; x++) {
        auto row = image.ptr(y);
        auto depth = depth_image.ptr(y);
        if (user_labels[y * 640 + x] != 0) {
          std::cout << x << ", " << y << std::endl;
          row[0] = 0xff;
          row[1] = 0x00;
          row[2] = 0x00;
        } else {
          uchar gray = static_cast<uchar>(~((depth[x] * 255) / 10000));
          row[0] = gray;
          row[1] = gray;
          row[2] = gray;
        }
      }
    }
    std::lock_guard<std::mutex> lg(mutex_);
    image.copyTo(depth_image_);
  }
}
