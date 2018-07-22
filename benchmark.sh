#!/bin/bash

command -v c++ >/dev/null 2>/dev/null

if [ $? -ne 0 ]
then
    echo "Error: no C++ compiler installed!"
    exit 1
fi

if [ ! -f $(pwd)/SmtCacheBlocking ]
then
    c++ -O2 -std=c++11 SmtCacheBlocking.cpp -o SmtCacheBlocking -lpthread
fi

osName=$(uname)

if [ "$osName" = "Linux" ]
then
    l1CacheSize=$(cat /sys/devices/system/cpu/cpu0/cache/index0/size | sed 's|[^0-9]||g')
    l2CacheSize=$(cat /sys/devices/system/cpu/cpu0/cache/index2/size | sed 's|[^0-9]||g')
    cpu=$(cat /proc/cpuinfo | grep 'model name' | uniq | cut -f2 -d':')
    echo $cpu
    cores=$(grep -m 1 'cpu cores' /proc/cpuinfo | cut -f2 -d':')
    echo "CPU cores: $cores"
    echo "Number of threads: $(grep 'processor' /proc/cpuinfo | wc -l)"
    echo "L1 data cache size: $l1CacheSize KB"
    echo "L2 cache size: $l2CacheSize KB"
    echo ""
elif [ "$osName" = "Darwin" ]
then
    l1CacheSize=$(($(sysctl -a hw.l1dcachesize | cut -f2 -d' ') / 1024))
    l2CacheSize=$(($(sysctl -a hw.l2cachesize | cut -f2 -d' ') / 1024))
    cpu=$(sysctl -a machdep.cpu.brand_string | cut -f2 -d':')
    echo $cpu
    cores=$(sysctl -a hw.physicalcpu | cut -f2 -d' ')
    echo "CPU cores: $cores"
    echo "Number of threads: $(sysctl -a hw.logicalcpu | cut -f2 -d' ')"
    echo "L1 data cache size: $l1CacheSize KB"
    echo "L2 cache size: $l2CacheSize KB"
    echo ""
else
    echo "Error: $osName not supported!"
    exit 1
fi

# Run benchmark

echo "=== L1 cache benchmark ==="
echo ""
./SmtCacheBlocking $(($l1CacheSize * 1024 / 1)) $((800000 * 1))
echo ""
sleep 5
./SmtCacheBlocking $(($l1CacheSize * 1024 / 2)) $((800000 * 2))
echo ""
sleep 5

echo "=== L2 cache benchmark ==="
echo ""
./SmtCacheBlocking $(($l2CacheSize * 1024 / 1)) $((200000 * 1))
echo ""
sleep 5
./SmtCacheBlocking $(($l2CacheSize * 1024 / 2)) $((200000 * 2))
