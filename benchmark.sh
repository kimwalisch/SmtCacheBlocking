#!/bin/bash

command -v c++ >/dev/null 2>/dev/null

if [ $? -ne 0 ]
then
    echo "Error: no C++ compiler installed!"
    exit 1
fi

if [ ! -f $(pwd)/SmtCacheBlocking ]
then
    c++ -O3 -march=native -std=c++11 SmtCacheBlocking.cpp -o SmtCacheBlocking -lpthread
fi

osName=$(uname)

if [ "$osName" = "Linux" ]
then
    l1CacheSize=$(cat /sys/devices/system/cpu/cpu0/cache/index0/size | sed 's|[^0-9]||g')
    l2CacheSize=$(cat /sys/devices/system/cpu/cpu0/cache/index2/size | sed 's|[^0-9]||g')
    cpuInfo=$(cat /proc/cpuinfo | grep 'model name' | uniq | cut -f2 -d':')
    echo $cpuInfo
    echo "CPU cores: $(grep -m 1 'cpu cores' /proc/cpuinfo | cut -f2 -d':' | sed 's| ||g')"
    echo "Number of threads: $(grep 'processor' /proc/cpuinfo | wc -l)"
    echo "L1 data cache size: $l1CacheSize KB"
    echo "L2 cache size: $l2CacheSize KB"
    echo ""
elif [ "$osName" = "Darwin" ]
then
    l1CacheSize=$(($(sysctl -a hw.l1dcachesize | cut -f2 -d' ') / 1024))
    l2CacheSize=$(($(sysctl -a hw.l2cachesize | cut -f2 -d' ') / 1024))
    cpuInfo=$(sysctl -a machdep.cpu.brand_string | cut -f2 -d':')
    echo $cpuInfo
    echo "CPU cores: $(sysctl -a hw.physicalcpu | cut -f2 -d' ')"
    echo "Number of threads: $(sysctl -a hw.logicalcpu | cut -f2 -d' ')"
    echo "L1 data cache size: $l1CacheSize KB"
    echo "L2 cache size: $l2CacheSize KB"
    echo ""
else
    echo "Error: $osName not supported!"
    exit 1
fi

itersL1=100000
itersL2=$(($itersL1 / ($l2CacheSize / $l1CacheSize)))
sleepSeconds=5

# Run benchmark

command -v perf >/dev/null 2>/dev/null

if [ $? -eq 0 ] && [ "$(whoami)" = "root" ]
then
    echo "=== L1 cache benchmark ==="
    echo ""
    perf stat -e L1-dcache-loads,L1-dcache-stores,L1-dcache-load-misses ./SmtCacheBlocking $(($l1CacheSize * 1024 / 1)) $(($itersL1 * 1))
    echo ""
    sleep $sleepSeconds
    perf stat -e L1-dcache-loads,L1-dcache-stores,L1-dcache-load-misses ./SmtCacheBlocking $(($l1CacheSize * 1024 / 2)) $(($itersL1 * 2))
    echo ""
    sleep $sleepSeconds
    echo "=== L2 cache benchmark ==="
    echo ""
    perf stat -e L1-dcache-loads,L1-dcache-load-misses,LLC-loads ./SmtCacheBlocking $(($l2CacheSize * 1024 / 1)) $(($itersL2 * 1))
    echo ""
    sleep $sleepSeconds
    perf stat -e L1-dcache-loads,L1-dcache-load-misses,LLC-loads ./SmtCacheBlocking $(($l2CacheSize * 1024 / 2)) $(($itersL2 * 2))
else
    echo "=== L1 cache benchmark ==="
    echo ""
    ./SmtCacheBlocking $(($l1CacheSize * 1024 / 1)) $(($itersL1 * 1))
    echo ""
    sleep $sleepSeconds
    ./SmtCacheBlocking $(($l1CacheSize * 1024 / 2)) $(($itersL1 * 2))
    echo ""
    sleep $sleepSeconds
    echo "=== L2 cache benchmark ==="
    echo ""
    ./SmtCacheBlocking $(($l2CacheSize * 1024 / 1)) $(($itersL2 * 1))
    echo ""
    sleep $sleepSeconds
    ./SmtCacheBlocking $(($l2CacheSize * 1024 / 2)) $(($itersL2 * 2))
fi
