#include "Audio.hpp"
#include "EntropyCollector.hpp"
#include <chrono>
#include <iostream>
#include <print>
#include <thread>
#include <fstream>
#include <string>

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

  std::cout << "How many samples to capture: ";
  uint64_t sampleLimit;
  if (!(std::cin >> sampleLimit))
  {
    std::cerr << "Invalid sample limit" << std::endl;
    return EXIT_FAILURE;
  }

  std::ofstream outputFile("entropy_data.csv");
  if (!outputFile)
  {
    std::cerr << "Could not open output file" << std::endl;
    return EXIT_FAILURE;
  }

  if (audio.startCapture(data_callback, &collector, deviceNum) != MA_SUCCESS)
  {
    std::cerr << "Couldn't start capture" << std::endl;
    return EXIT_FAILURE;
  }

  uint64_t totalBytesSaved = 0;
  uint64_t bytes_Skipped = 0;
  const uint64_t threshold = 10000;

  while (totalBytesSaved < sampleLimit)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto data = collector.extract();
    if (!data.empty())
    {

      if (bytes_Skipped < threshold)
      {
        bytes_Skipped += data.size();
        continue;
      }

      size_t bytesToWrite = data.size();

      if (totalBytesSaved + bytesToWrite > sampleLimit)
      {
        bytesToWrite = sampleLimit - totalBytesSaved;
      }
      for (size_t i = 0; i < bytesToWrite; ++i)
      {
        outputFile << static_cast<int>(data[i]) << "\n";
      }
      outputFile.flush();
      totalBytesSaved += bytesToWrite;
    }
    collector.displayHistogram();
  }

  return EXIT_SUCCESS;
}
