//
// Created by Masayuki IZUMI on 10/21/16.
//

#ifndef BODYPARTSSEGMENTOR_SENSORDEVICE_H
#define BODYPARTSSEGMENTOR_SENSORDEVICE_H

#include <mutex>

#include <OpenNI.h>
#include <NiTE.h>
#include <opencv2/opencv.hpp>

class SensorDevice : public nite::UserTracker::NewFrameListener {
public:
  SensorDevice(const char *uri);
  ~SensorDevice();

  virtual void onNewFrame(nite::UserTracker &tracker) override;

  cv::Mat depth_image() {
    return depth_image_;
  }


private:
  openni::Device device_;
  openni::VideoStream depth_stream_;
  nite::UserTracker user_tracker_;

  std::thread worker_;
  std::mutex mutex_;
  cv::Mat depth_image_;

  void checkStatus(openni::Status status, std::string msg);
  void checkStatus(nite::Status status, std::string msg);
  void checkStatus(bool is_ok, std::string msg);
  void initialize(const char *uri);
  void startDepthStream();
  void update();
};

#endif //BODYPARTSSEGMENTOR_SENSORDEVICE_H
