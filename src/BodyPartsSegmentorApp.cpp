#include <chrono>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "CinderOpenCV.h"

#include "cereal/archives/json.hpp"

#include "SensorDevice.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BodyPartsSegmentorApp : public App {
public:
  void setup() override;
  void cleanup() override;
  void update() override;
  void draw() override;

private:
  std::unique_ptr<SensorDevice> device_;
  gl::TextureRef texture_;
};

void BodyPartsSegmentorApp::setup() {
  if (openni::OpenNI::initialize() != openni::STATUS_OK) {
    throw std::runtime_error("Failed to initialize OpenNI.");
  }
  if (nite::NiTE::initialize() != nite::STATUS_OK) {
    throw std::runtime_error("Failed to initialize NiTE.");
  }

  openni::Array<openni::DeviceInfo> device_info_list;
  openni::OpenNI::enumerateDevices(&device_info_list);
  if (device_info_list.getSize() > 0) {
    device_ = std::unique_ptr<SensorDevice>(new SensorDevice);
  } else {
    auto onifile = getOpenFilePath(fs::path(), { "oni" });
    if (onifile.empty()) {
      exit(1);
    } else {
      device_ = std::unique_ptr<SensorDevice>(new SensorDevice(onifile.string()));
    }
  }
}

void BodyPartsSegmentorApp::cleanup() {
  nite::NiTE::shutdown();
  openni::OpenNI::shutdown();
}

void BodyPartsSegmentorApp::update() {
  device_->update();
  auto image = device_->image();
  texture_ = gl::Texture::create(fromOcv(image));

  if (device_->hasSkeletons()) {
    auto stamp = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
    auto dir = getAssetPath("");
    cv::imwrite((dir / (to_string(stamp) + ".png")).string(), image);

    {
      std::ofstream ofs((dir / (to_string(stamp) + ".json")).string());
      cereal::JSONOutputArchive ar(ofs);
      ar(cereal::make_nvp("skeletons", device_->skeletons()));
    }

    CI_LOG_V("Save " + to_string(stamp) + ".{png,json} successfully.");
  }
}

void BodyPartsSegmentorApp::draw() {
  gl::clear();
  gl::draw(texture_);
}


CINDER_APP( BodyPartsSegmentorApp, RendererGl )
