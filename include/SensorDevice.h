//
// Created by Masayuki IZUMI on 10/21/16.
//

#ifndef BODYPARTSSEGMENTOR_SENSORDEVICE_H
#define BODYPARTSSEGMENTOR_SENSORDEVICE_H

#include <mutex>

#include <OpenNI.h>
#include <NiTE.h>
#include <opencv2/opencv.hpp>

#include "Skeleton.h"

class SensorDevice {
public:
  SensorDevice(const std::string &uri = "");
  ~SensorDevice();

  cv::Mat image() {
    return image_;
  }

  std::vector<Skeleton> skeletons() const {
    return skeletons_;
  };

  bool hasSkeletons() const {
    return !skeletons_.empty();
  }

  void update();


private:
  openni::Device device_;
  openni::VideoStream depth_stream_;
  nite::UserTracker user_tracker_;

  cv::Mat image_;
  std::vector<Skeleton> skeletons_;

  std::mutex mutex_;

  void checkStatus(nite::Status status, std::string msg);
  void checkStatus(bool is_ok, std::string msg);
  void initialize(const std::string &uri);
};

#endif //BODYPARTSSEGMENTOR_SENSORDEVICE_H
