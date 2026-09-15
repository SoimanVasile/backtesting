#include "L1Tick.h"
#include <cstdint>
#pragma once


struct alignas(64) Metrics{
    double profit;
};

class Engine {
    const L1Tick* ticks;
    uint64_t slow_period, fast_period, size;

public:
    Engine(const L1Tick* ticks, uint64_t slow_period, uint64_t fast_period, uint64_t size);

    const L1Tick* get_ticks() const;
    uint64_t get_slow_period() const;
    uint64_t get_fast_period() const;
    uint64_t get_size() const;
    Metrics* get_metric() const;
};



void engine_run(Engine& engine, Metrics& metrics);
