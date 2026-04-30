#pragma once

#include <vector>
#include <cstdint>
#include <mutex>
#include <array>

class EntropyCollector {
    std::vector<uint8_t> _pool;
    uint64_t _bitBuffer = 0;
    uint8_t _collectedBits = 0;
    std::array<uint64_t, 8> _counts{};
    uint64_t _totalSamples = 0;
    mutable std::mutex _mutex;

public:
    void feed(const uint8_t *samples, uint32_t count);
    void displayHistogram();
    std::vector<uint8_t> extract();
    size_t available() const;
};
