#include <cstdint>
#include "L1Tick.h"

#pragma once

#define SLOW_PERIOD 200001
#define FAST_PERIOD 100001

class InvestingStrategy {
private:
    uint64_t last_pointer_fast = 0, last_pointer_slow = 0, number_of_ticks = 0;
    double fast_sum = 0.0, slow_sum = 0.0, buying_price = 0.0, profit = 0.0;
    bool currently_holding = false;
    const L1Tick* ticks;

public:
    InvestingStrategy(const L1Tick* ticks);
    void invest(const L1Tick& tick);
    uint64_t get_number_of_ticks() const;
    double get_profit() const;
};
