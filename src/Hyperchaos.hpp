#include <vector>
#include <array>
#include <cstdint>
#include <cstring>

class Hyperchaos
{
private:
    static constexpr int L = 8;
    static constexpr double ALPHA = 1.99999;
    static constexpr double EPSILON = 0.05;
    static constexpr double OMEGA = 0.5;

    std::array<double, L> x = {
        0.141592, 0.653589, 0.793238, 0.462643,
        0.383279, 0.502884, 0.197169, 0.399375};
    double tentMap(double val) const;
    uint64_t swapBits(uint64_t val) const;

public:
    std::vector<uint8_t> processBatch(const std::array<uint8_t, L> &y);
};