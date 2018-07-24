# SmtCacheBlocking

This benchmark tries to answer the question:

In a multi-threaded program should we try to assign half of the cache
to the first thread and the other half of the cache to the second thread
in order to improve cache efficiency? Remember that most CPUs out there
use a technique called Hyper-Threading or simultaneous multithreading
(SMT) where each physical CPU core consists of multiple logical CPU cores
called threads. And multiple threads (e.g. 2) usually share the same L1
and L2 caches.

# Build instructions

You need to have installed a C++ compiler and make.

```bash
sudo apt install g++ make

make
./benchmark.sh
```

# Linux perf

On Linux if you have ```perf``` installed you can get detailed
cache statistics.

```bash
# Install perf
sudo apt install linux-tools-generic

make
sudo ./benchmark.sh perf
```

# Usage example

The ```benchmark.sh``` script runs 4 different tests:

* 2 tests for L1 data cache
* 2 tests for L2 cache

All 4 tests do exactly the same amount of work, they
only differ by their cache blocking strategy. The fastest
timing indicates what's the best cache blocking strategy
for each cache.


```bash
$ ./benchmark.sh 
Intel(R) Xeon(R) Platinum 8124M CPU @ 3.00GHz
CPU cores: 8
Number of threads: 16
L1 data cache size: 32 KB
L2 cache size: 1024 KB

=== L1 cache benchmark ===

Vector bytes: 32768
Threads: 16
Iters: 100000
Sum: 18446744073709234688
Seconds: 2.43015

Vector bytes: 16384
Threads: 16
Iters: 200000
Sum: 18446744073709372928
Seconds: 2.2743

=== L2 cache benchmark ===

Vector bytes: 1048576
Threads: 16
Iters: 3125
Sum: 686466560
Seconds: 6.50853

Vector bytes: 524288
Threads: 16
Iters: 6250
Sum: 18446744073663905024
Seconds: 4.28933
```
