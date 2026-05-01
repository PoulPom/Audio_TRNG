#include "Audio.hpp"
#include "EntropyCollector.hpp"
#include <chrono>
#include <iostream>
#include <print>
#include <thread>

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                   ma_uint32 frameCount)
{
  if (pInput)
  {
    auto *collector = static_cast<EntropyCollector *>(pDevice->pUserData);
    collector->feed(static_cast<const int16_t *>(pInput), frameCount);
  }
}

int main()
{
  Audio audio;
  EntropyCollector collector;

  if (audio.init() != MA_SUCCESS)
  {
    std::cerr << "Couldnt initialize context" << std::endl;
    return EXIT_FAILURE;
  }

  if (audio.setupDevices() != MA_SUCCESS)
  {
    std::cerr << "Couldnt get devices" << std::endl;
    return EXIT_FAILURE;
  }

  const auto &captureDevices = audio.getCaptureDevices();
  for (size_t i = 0; i < captureDevices.count; ++i)
  {
    std::println("{} - {}", i, captureDevices.info[i].name);
  }

  std::cout << "Select device number: ";
  int deviceNum;
  if (!(std::cin >> deviceNum) || deviceNum < 0 ||
      (ma_uint32)deviceNum >= captureDevices.count)
  {
    std::cerr << "Invalid device selection" << std::endl;
    return EXIT_FAILURE;
  }

  if (audio.startCapture(data_callback, &collector, deviceNum) != MA_SUCCESS)
  {
    std::cerr << "Couldn't start capture" << std::endl;
    return EXIT_FAILURE;
  }

  while (true)
  {
    collector.extract();
    collector.displayHistogram();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return EXIT_SUCCESS;
}
