#include "engine.h"
#include "L1Tick.h"
#include "investing_strategy.h"
#include <cstdint>

Engine::Engine(const L1Tick* ticks, uint64_t slow_period, uint64_t fast_period, uint64_t size): ticks(ticks), slow_period(slow_period), fast_period(fast_period), size(size) {};

uint64_t Engine::get_fast_period() const{
    return fast_period;
};

uint64_t Engine::get_slow_period() const{
    return slow_period;
};

const L1Tick* Engine::get_ticks() const{
    return this->ticks;
}

uint64_t Engine::get_size() const{
    return this->size;
}

void engine_run(Engine& engine, Metrics& metric){
    auto invest = InvestingStrategy(engine.get_ticks(), engine.get_slow_period(), engine.get_fast_period());

    auto number_of_ticks = engine.get_size() / sizeof(L1Tick);

    while (invest.get_number_of_ticks() < number_of_ticks){
        invest.invest(engine.get_ticks()[invest.get_number_of_ticks()]);
    }

    metric.profit = invest.get_profit();
};
