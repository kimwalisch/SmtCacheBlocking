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
sudo ./benchmark.sh
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
Intel(R) Core(TM) i5-2300 CPU @ 2.80GHz
CPU cores: 4
Number of threads: 4
L1 data cache size: 32 KB
L2 cache size: 256 KB

=== L1 cache benchmark ===

Vector bytes: 32768
Threads: 4
Iters: 100000
Sum: 18446744073709469952
Seconds: 10.4486

Vector bytes: 16384
Threads: 4
Iters: 200000
Sum: 18446744073709485568
Seconds: 9.96097

=== L2 cache benchmark ===

Vector bytes: 262144
Threads: 4
Iters: 12500
Sum: 18446744073708991744
Seconds: 13.0473

Vector bytes: 131072
Threads: 4
Iters: 25000
Sum: 18446744073709358080
Seconds: 11.3245
```
