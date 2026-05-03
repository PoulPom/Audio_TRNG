#include "Audio.hpp"
#include "EntropyCollector.hpp"
#include "Hyperchaos.hpp"
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
  Hyperchaos hyperchaos;

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

  std::ofstream outputFileRaw("entropy_data.csv", std::ios::app);
  std::ofstream outputFileProcessed("processed_entropy_data.csv", std::ios::app);
  if (!outputFileRaw)
  {
    std::cerr << "Could not open output file" << std::endl;
    return EXIT_FAILURE;
  }
  if (!outputFileProcessed)
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
  std::array<uint8_t, 8> hyperBuffer{};
  size_t hyperBufferCount = 0;

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
        outputFileRaw << static_cast<int>(data[i]) << "\n";

        hyperBuffer[hyperBufferCount] = data[i];
        hyperBufferCount++;

        if (hyperBufferCount == 8)
        {
          auto hyperResult = hyperchaos.processBatch(hyperBuffer);

          for (size_t i = 0; i < hyperResult.size(); ++i)
          {
            outputFileProcessed << static_cast<int>(hyperResult[i]) << "\n";
          }
          hyperBufferCount = 0; // Reset the buffer count
        }
      }

      outputFileRaw.flush();
      outputFileProcessed.flush();

      totalBytesSaved += bytesToWrite;
    }
    collector.displayHistogram();
  }

  return EXIT_SUCCESS;
}
