#include "investing_strategy.h"
#include "L1Tick.h"
#include <cstdint>

InvestingStrategy::InvestingStrategy(const L1Tick* ticks) { this->ticks = ticks; }

uint64_t InvestingStrategy::get_number_of_ticks() const { return number_of_ticks; }

void InvestingStrategy::invest(const L1Tick& tick) {
    
    this->number_of_ticks++;
    this->fast_sum += tick.price;
    this->slow_sum += tick.price;

    if (number_of_ticks < FAST_PERIOD){
        return;
    }
    this->fast_sum -= ticks[last_pointer_fast++].price;

    if (number_of_ticks < SLOW_PERIOD){
        return;
    }
    this->slow_sum -= ticks[last_pointer_slow++].price;

    double fast_avg = this->fast_sum / (FAST_PERIOD - 1);
    double slow_avg = this->slow_sum / (SLOW_PERIOD - 1);

    // std::cout<<"FAST AVG: " << fast_avg << " -------- SLOW AVG: " << slow_avg << '\n';
    if ( fast_avg > slow_avg && !this->currently_holding){
        this->buying_price = tick.price;
        currently_holding = true;
    }

    if ( fast_avg < slow_avg && this->currently_holding) {
        this->currently_holding = false;
        this->profit += tick.price - this->buying_price;
    }
}

double InvestingStrategy::get_profit() const { return profit; }
