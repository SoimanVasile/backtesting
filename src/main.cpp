#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <thread>
#include <vector>
#include "L1Tick.h"
#include "engine.h"

using std::vector;
const char* input_file = "tick.bin";

#define SLOW_PERIOD 1001
#define FAST_PERIOD 301

const size_t number_of_cores = std::thread::hardware_concurrency();

int main(){

    int32_t fd = open(input_file, O_RDONLY);

    if ( fd == -1 ) {
        std::cerr << "Couldnt open the File: " << input_file;
        return -1;
    }

    uint64_t size = std::filesystem::file_size(input_file);
    auto ptr = (const char*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);

    if (ptr == MAP_FAILED ){
        std::cerr << "Failed to create the mmap!";
        return -2;
    }
    const L1Tick* ticks = reinterpret_cast<const L1Tick*>(ptr);

    vector<std::thread> workers;
    workers.reserve(number_of_cores);

    vector<Metrics> metrics;
    metrics.reserve(number_of_cores);

    for ( size_t i{}; i < number_of_cores; i++){
        uint64_t slow_period = (i+1) * SLOW_PERIOD;
        uint64_t fast_period = (i+1) * FAST_PERIOD;
        auto engine_thread = Engine(ticks, slow_period, fast_period, size);

        workers.emplace_back(engine_run, std::ref(engine_thread), std::ref(metrics[i]));

        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(i, &cpu_set);

        pthread_setaffinity_np(workers.back().native_handle(), sizeof(cpu_set) , &cpu_set);
    }

    for (auto& thread : workers){
        if (thread.joinable()){
            thread.join();
        }
    }




    for ( size_t i{}; i < number_of_cores; i++){
        auto slow_period = (i+1) * SLOW_PERIOD;
        auto fast_period = (i+1) * FAST_PERIOD;
        auto profit = metrics[i].profit;

        std::cout<< "THREAD: " << i << " --- with slow period: " << slow_period << " ----- " << " fast period: " << fast_period << " --- made profit of: " << profit << '\n';
    }
}
