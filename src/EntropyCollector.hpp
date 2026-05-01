#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <vector>

class EntropyCollector
{
  std::vector<uint8_t> _pool;
  uint64_t _bitBuffer = 0;
  uint8_t _collectedBits = 0;
  std::array<uint64_t, 256> _counts{};
  uint64_t _totalSamples = 0;
  mutable std::mutex _mutex;

  // Zmienne to innego podejścia

  uint8_t _octet = 0;
  int16_t _currentSample = 0;
  uint8_t _bitCounter = 0;

  double calculateEntropy() const;

public:
  void feed(const int16_t *samples, uint32_t count);
  void displayHistogram();
  std::vector<uint8_t> extract();
  size_t available() const;
};
