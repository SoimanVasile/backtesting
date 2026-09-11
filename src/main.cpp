#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <sys/mman.h>

const char* input_file = "tick.bin";

struct L1Tick{
    double price, qty;
    uint64_t timestamp;
};

#define SLOW_PERIOD 200001
#define FAST_PERIOD 100001

class InvestingStrategy {
private:
    uint64_t last_pointer_fast = 0, last_pointer_slow = 0, number_of_ticks = 0;
    double fast_sum = 0.0, slow_sum = 0.0, buying_price = 0.0, profit = 0.0;
    bool currently_holding = false;
    const L1Tick* ticks;

public:
    InvestingStrategy(const L1Tick* ticks){this->ticks = ticks;}

    uint64_t get_number_of_ticks() const {return number_of_ticks;}
    void invest(const L1Tick& tick){
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

    double get_profit() const { return profit;}
};

int main(){

    int32_t fd = open(input_file, O_RDONLY);

    if ( fd == -1 ) {
        std::cerr << "Couldnt open the File: " << input_file;
        return -1;
    }

    uint64_t size = std::filesystem::file_size(input_file);
    auto ptr = (const char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

    auto total_ticks = size / sizeof(L1Tick);
    if (ptr == MAP_FAILED ){
        std::cerr << "Failed to create the mmap!";
        return -2;
    }
    const L1Tick* ticks = reinterpret_cast<const L1Tick*>(ptr);
    auto invest = InvestingStrategy(ticks);

    while (invest.get_number_of_ticks() < total_ticks){
        invest.invest(ticks[invest.get_number_of_ticks()]);
    }

    std::cout << "Profit: " << invest.get_profit();
    return 0;
}
