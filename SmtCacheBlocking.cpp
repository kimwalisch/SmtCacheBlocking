#include <vector>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <future>
#include <atomic>
#include <cmath>
#include <cstdint>

std::uint64_t rotl64(std::uint64_t x, std::uint64_t n)
{
  return (x << n) | (x >> (64 - n));
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " cacheBytes iters" << std::endl;
        return 1;
    }

    std::size_t bytes = std::atol(argv[1]);
    std::size_t iters = std::atol(argv[2]);

    int threads = std::thread::hardware_concurrency();

    // bytes must be a power of 2
    double lg2 = std::log2((double) bytes);
    bytes = 1ull << (std::size_t) lg2;

    std::cout << "Vector bytes: " << bytes << std::endl;
    std::cout << "Threads: " << threads << std::endl;
    std::cout << "Iters: " << iters << std::endl;

    std::atomic<std::size_t> i(0);
    std::vector<std::vector<char>> vectors;
    vectors.resize(threads);

    // each thread executes 1 task
    auto task = [&](int threadId)
    {
        auto& vect = vectors[threadId];
        vect.resize(bytes, 0);

        std::uint64_t x = 123456789;
        std::uint64_t y = 362436069;
        std::uint64_t z = 521288629;

        while (i++ < iters)
        {
            for (std::size_t s = 0; s < bytes; s++)
            {
                // xorshf96: fast random number generator
                std::uint64_t t;
                x ^= x << 16;
                x ^= x >> 5;
                x ^= x << 1;
                t = x;
                x = y;
                y = z;
                z = t ^ x ^ y;

                vect[z & (bytes - 1)] += 1;
                vect[rotl64(z, 8) & (bytes - 1)] += 1;
                vect[rotl64(z, 16) & (bytes - 1)] += 1;
                vect[rotl64(z, 32) & (bytes - 1)] += 1;
                vect[rotl64(z, 40) & (bytes - 1)] += 1;
                vect[rotl64(z, 48) & (bytes - 1)] += 1;
                vect[rotl64(z, 56) & (bytes - 1)] += 1;
            }
        }

        std::size_t sum = 0;

        for (auto n : vect)
            sum += n;

        return sum;
    };

    auto t1 = std::chrono::system_clock::now();
    std::vector<std::future<std::size_t>> futures;
    futures.reserve(threads);

    for (int t = 0; t < threads; t++)
        futures.emplace_back(std::async(std::launch::async, task, t));

    std::size_t sum = 0;

    for (auto &f : futures)
        sum += f.get();

    auto t2 = std::chrono::system_clock::now();
    std::chrono::duration<double> seconds = t2 - t1;

    std::cout << "Sum: " << sum << std::endl;
    std::cout << "Seconds: " << seconds.count() << std::endl;

    return 0;
}
