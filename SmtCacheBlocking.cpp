#include <vector>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <future>
#include <atomic>
#include <cmath>

void cacheBenchmark(std::vector<char>& vect)
{
    unsigned long x = 123456789;
    unsigned long y = 362436069;
    unsigned long z = 521288629;

    std::size_t size = vect.size();

    for (std::size_t i = 0; i < size * 100; i++)
    {
        // xorshf96 random number generator
        unsigned long t;
        x ^= x << 16;
        x ^= x >> 5;
        x ^= x << 1;
        t = x;
        x = y;
        y = z;
        z = t ^ x ^ y;

        vect[z & (size - 1)] += z & 127;
    }
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

    for (std::size_t i = 0; i < vectors.size(); i++)
        vectors[i].resize(bytes, 0);

    // each thread executes 1 task
    auto task = [&](int threadId)
    {
        while (i++ < iters)
            cacheBenchmark(vectors[threadId]);

        std::size_t sum = 0;

        for (std::size_t j = 0; j < vectors[threadId].size(); j++)
            sum += vectors[threadId][j];

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
