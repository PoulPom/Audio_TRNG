#include "EntropyCollector.hpp"
#include <iostream>
#include <print>

void EntropyCollector::feed(const uint8_t *samples, uint32_t count) {
  std::lock_guard<std::mutex> lock(_mutex);
  for (size_t i = 0; i < count; i++) {
    uint8_t sample = samples[i] & 0x07;

    _counts[sample]++;
    _totalSamples++;

    _bitBuffer |= (static_cast<uint64_t>(sample) << _collectedBits);
    _collectedBits += 3;
    while (_collectedBits >= 8) {
      _pool.push_back(static_cast<uint8_t>(_bitBuffer & 0xff));
      _bitBuffer >>= 8;
      _collectedBits -= 8;
    }
  }
}

void EntropyCollector::displayHistogram() {
  std::lock_guard<std::mutex> lock(_mutex);
  if (_totalSamples == 0)
    return;

  std::print("\033[H\033[J");
  std::println("Histogram 3-bitowych paczek (000 - 111):");
  std::println("Przetworzonych próbek: {}\n", _totalSamples);

  for (int i = 0; i < 8; ++i) {
    double percentage = (double)_counts[i] / _totalSamples * 100.0;
    int barWidth = static_cast<int>(percentage);

    std::print("{:03b} ({}) | ", i, i);
    for (int k = 0; k < barWidth; ++k)
      std::print("█");
    std::println(" {:.2f}%", percentage);
  }
}

std::vector<uint8_t> EntropyCollector::extract() {
  std::lock_guard<std::mutex> lock(_mutex);
  return std::move(_pool);
}

size_t EntropyCollector::available() const {
  std::lock_guard<std::mutex> lock(_mutex);
  return _pool.size();
}
