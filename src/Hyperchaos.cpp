#include "Hyperchaos.hpp"

double Hyperchaos::tentMap(double val) const
{
    if (val >= 0.0 && val < 0.5)
    {
        return ALPHA * val;
    }
    else
    {
        return ALPHA * (1.0 - val);
    }
}

uint64_t Hyperchaos::swapBits(uint64_t val) const
{
    return (val >> 32) | (val << 32);
}

std::vector<uint8_t> Hyperchaos::processBatch(const std::array<uint8_t, L> &y)
{
    for (int i = 0; i < L; ++i)
    {
        x[i] = (((OMEGA * y[i]) / 255.0) + x[i]) * (1.0 / (1.0 + OMEGA));
    }
    int iterations = L / 2;
    for (int t = 0; t < iterations; ++t)
    {
        std::array<double, L> nextX;
        for (int i = 0; i < L; i++)
        {
            int left = (i - 1 + L) % L;
            int right = (i + 1) % L;

            nextX[i] = (1.0 - EPSILON) * tentMap(x[i]) + (EPSILON / 2.0) * (tentMap(x[right]) + tentMap(x[left]));
        }
        x = nextX;
    }
    // Szpont \/
    std::array<uint64_t, L> z;
    for (int i = 0; i < L; ++i)
    {
        std::memcpy(&z[i], &x[i], sizeof(uint64_t));
    }

    std::vector<uint8_t> finalOutput;
    finalOutput.reserve((L / 2) * 8);

    for (int i = 0; i < L / 2; ++i)
    {
        uint64_t output64 = z[i] ^ swapBits(z[i + L / 2]);
        for (int byteIdx = 0; byteIdx < 8; ++byteIdx)
        {
            finalOutput.push_back(static_cast<uint8_t>((output64 >> (byteIdx * 8)) & 0xFF));
        }
    }

    return finalOutput;
}
