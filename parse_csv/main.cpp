#include <charconv>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/mman.h>
#include <unistd.h>


const size_t BATCH_COUNT = 10000;
// const char* file_name = "test.csv";
const char* file_name = "BTCUSDT-trades-2023-01.csv";
const char* output_file = "tick.bin";

struct Tick{
    double price, qty;
    uintmax_t timestamp;
};

const char* parse_line(const char* ptr, const  char* end, Tick& tick) {
    auto coma = (const char*) memchr(ptr, ',', end - ptr);
    ptr = coma+1;
    coma = (const char*) memchr(ptr, ',', end - ptr);
    std::from_chars(ptr, coma, tick.price);

    ptr = coma + 1;
    coma = (const char*) memchr(ptr, ',', end - ptr);

    std::from_chars(ptr, coma, tick.qty);

    coma = (const char*) memchr(coma + 1, ',', end - ptr);
    ptr = coma+1;
    coma = (const char*) memchr(ptr, ',', end - ptr);
    
    std::from_chars(ptr, coma, tick.timestamp);

    coma = (const char*) memchr(ptr, ',', end - ptr);

    ptr = (const char*) memchr(ptr, '\n', end - ptr);
    return (ptr) ? ptr+1 : end;
}


int main(){
    uint32_t fd = open(file_name, O_RDONLY);
    uint32_t fd_write = open(output_file, O_CREAT |  O_WRONLY | O_TRUNC, 0644);

    std::error_code ec;
    uintmax_t size = std::filesystem::file_size(file_name, ec);

    if (ec) {
        std::cerr<<" Failed to get the file size: " << ec.message() << '\n';
        return -1;
    }

    auto ptr = (const char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    const char* end = ptr + size;

    Tick batch[BATCH_COUNT];
    size_t count = 0;
    while (ptr < end){
        ptr = parse_line(ptr, end, batch[count]);
        count++;

        if (count >= BATCH_COUNT - 1){
            write(fd_write, &batch, count * sizeof(Tick));
            count = 0;
        }
    }
    
    if (count > 0){
        write(fd_write, &batch, count * sizeof(Tick));
    }
    return 0;
}
