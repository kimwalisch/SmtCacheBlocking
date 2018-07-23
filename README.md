# SmtCacheBlocking

This benchmark tries to answer the question:

In a multi-threaded program should we try to assign half of the cache
to the first thread and the other half of the cache to the second thread
in order to improve cache efficiency? Remember that most CPUs out there
use a technique called Hyper-Threading or simultaneous multithreading
(SMT) where each physical CPU core consists of multiple logical CPU cores
called threads. And multiple threads (e.g. 2) usually share the same L1
and L2 caches.

# Usage

You need to have installed a C++ compiler and make.

```bash
sudo apt install g++ make

make
./benchmark.sh
```

# Linux

On Linux if you have ```perf``` installed you can get detailed
cache statistics.

```bash
# Install perf
sudo apt install linux-tools-generic

make
sudo ./benchmark.sh
```
