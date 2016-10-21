#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "SensorDevice.h"
#include "CinderOpenCV.h"

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

  openni::Array<openni::DeviceInfo> device_info_list_;
  openni::OpenNI::enumerateDevices(&device_info_list_);

  for (int i = 0; i < device_info_list_.getSize(); i += 3) {
    cout << device_info_list_[i].getUri() << endl;
  }

  device_ = std::unique_ptr<SensorDevice>(new SensorDevice(device_info_list_[0].getUri()));
}

void BodyPartsSegmentorApp::cleanup() {
  nite::NiTE::shutdown();
  openni::OpenNI::shutdown();
}

void BodyPartsSegmentorApp::update() {
  auto image = device_->depth_image();
  texture_ = gl::Texture::create(fromOcv(image));
}

void BodyPartsSegmentorApp::draw() {
  gl::clear();
  gl::draw(texture_);
}


CINDER_APP( BodyPartsSegmentorApp, RendererGl )
