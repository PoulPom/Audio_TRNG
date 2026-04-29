#include "./miniaudio.c"
#include "miniaudio.h"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <print>

struct DeviceInfo {
  ma_device_info *info = nullptr;
  ma_uint32 count = 0;

  ma_device_info *begin() const { return info; }
  ma_device_info *end() const { return info + count; }
};

class Audio {
  ma_context _context;
  // devices
  ma_device _captureDevice;
  ma_device _playbackDevice;
  // list of devices
  DeviceInfo _playbackInfo;
  DeviceInfo _captureInfo;

  bool _isContextInitialized = false;
  bool _isCaptureDeviceInitialized = false;
  bool _isPlaybackDeviceInitialized = false;

public:
  Audio() = default;
  Audio(const Audio &) = delete;
  Audio &operator=(const Audio &) = delete;

  ~Audio() {
    if (_isContextInitialized) {
      ma_context_uninit(&_context);
    }
  }

  ma_result init() {
    if (_isContextInitialized) {
      return MA_SUCCESS;
    }
    ma_result result = ma_context_init(NULL, 0, NULL, &_context);
    if (result == MA_SUCCESS) {
      _isContextInitialized = true;
    }
    return result;
  }

  ma_result setupDevices() {
    if (!_isContextInitialized) {
      return MA_ERROR;
    }
    return ma_context_get_devices(&_context, &_playbackInfo.info,
                                  &_playbackInfo.count, &_captureInfo.info,
                                  &_captureInfo.count);
  }

  const DeviceInfo &getCaptureDevices() const { return _captureInfo; }
  const DeviceInfo &getPlaybackDevice() const { return _playbackInfo; }

  ma_result initDevice(ma_device_type deviceType, int deviceNum = 0) {
    if (!_isContextInitialized) {
      return MA_ERROR;
    }
    ma_device_config config = ma_device_config_init(deviceType);
    if (deviceType == ma_device_type_capture) {
      config.capture.pDeviceID = &_captureInfo.info[deviceNum].id;
      config.capture.format = ma_format_u8;
      config.capture.channels = 2;
      config.sampleRate = 44100;
    }
  }
};

int main() {
  Audio audio;
  if (audio.init() != MA_SUCCESS) {
    std::cerr << "Couldnt initialize context";
    return EXIT_FAILURE;
  }
  if (audio.setupDevices() != MA_SUCCESS) {
    std::cerr << "Couldnt get devices";
    return EXIT_FAILURE;
  }

  for (const auto &d : audio.getPlaybackDevice()) {
    std::println("{}", d.name);
  }

  return 0;
}
