#pragma once

#include "../miniaudio.h"

struct DeviceInfo {
  ma_device_info *info = nullptr;
  ma_uint32 count = 0;

  ma_device_info *begin() const { return info; }
  ma_device_info *end() const { return info + count; }
};

class Audio {
  ma_context _context;
  ma_device _captureDevice;
  DeviceInfo _playbackInfo;
  DeviceInfo _captureInfo;

  bool _isContextInitialized = false;
  bool _isCaptureDeviceInitialized = false;

public:
  Audio();
  Audio(const Audio &) = delete;
  Audio &operator=(const Audio &) = delete;
  ~Audio();

  ma_result init();
  ma_result setupDevices();
  const DeviceInfo &getCaptureDevices() const { return _captureInfo; }
  const DeviceInfo &getPlaybackDevice() const { return _playbackInfo; }

  ma_result startCapture(ma_device_data_proc callback, void *userData,
                         int deviceNum);
};
