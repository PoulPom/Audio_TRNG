#include "Audio.hpp"

Audio::Audio() = default;

Audio::~Audio() {
  if (_isCaptureDeviceInitialized) {
    ma_device_uninit(&_captureDevice);
  }
  if (_isContextInitialized) {
    ma_context_uninit(&_context);
  }
}

ma_result Audio::init() {
  if (_isContextInitialized) {
    return MA_SUCCESS;
  }
  ma_result result = ma_context_init(NULL, 0, NULL, &_context);
  if (result == MA_SUCCESS) {
    _isContextInitialized = true;
  }
  return result;
}

ma_result Audio::setupDevices() {
  if (!_isContextInitialized) {
    return MA_ERROR;
  }
  return ma_context_get_devices(&_context, &_playbackInfo.info,
                                &_playbackInfo.count, &_captureInfo.info,
                                &_captureInfo.count);
}

ma_result Audio::startCapture(ma_device_data_proc callback, void *userData,
                              int deviceNum) {
  if (_isCaptureDeviceInitialized) {
    ma_device_uninit(&_captureDevice);
    _isCaptureDeviceInitialized = false;
  }

  ma_device_config config = ma_device_config_init(ma_device_type_capture);
  config.capture.pDeviceID = &_captureInfo.info[deviceNum].id;
  config.capture.format = ma_format_u8;
  config.capture.channels = 1;
  config.sampleRate = 44100;
  config.dataCallback = callback;
  config.pUserData = userData;

  ma_result result = ma_device_init(&_context, &config, &_captureDevice);
  if (result != MA_SUCCESS) {
    return result;
  }
  _isCaptureDeviceInitialized = true;
  return ma_device_start(&_captureDevice);
}
