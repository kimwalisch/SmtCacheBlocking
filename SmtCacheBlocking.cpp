#include <vector>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <future>
#include <atomic>
#include <cmath>

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

        unsigned long x = 123456789;
        unsigned long y = 362436069;
        unsigned long z = 521288629;

        while (i++ < iters)
        {
            for (std::size_t s = 0; s < bytes; s++)
            {
                // xorshf96: fast random number generator
                unsigned long t;
                x ^= x << 16;
                x ^= x >> 5;
                x ^= x << 1;
                t = x;
                x = y;
                y = z;
                z = t ^ x ^ y;

                // compute 4 random indexes
                unsigned long i1 = z;
                unsigned long i2 = (z >> 16) + ((z & 0xffff) << 48);
                unsigned long i3 = (z >> 32) + (z << 32);
                unsigned long i4 = (z >> 48) + (z << 48);

                vect[i1 & (bytes - 1)] += (char) i1;
                vect[i2 & (bytes - 1)] += (char) i2;
                vect[i3 & (bytes - 1)] += (char) i3;
                vect[i4 & (bytes - 1)] += (char) i4;
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
