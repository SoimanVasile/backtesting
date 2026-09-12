#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <sys/mman.h>
#include "L1Tick.h"
#include "investing_strategy.h"

const char* input_file = "tick.bin";

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
