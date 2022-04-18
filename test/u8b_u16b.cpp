#include <iostream>
#include <chrono>
#include "utf/unify.hpp"

int main()
{
    std::vector<char8_t>    i_buffer;
    std::vector<char16_t>   o_buffer;

    using   UnicodeBB   = utf::UniFy<char16_t, char8_t, true, true>;

    int residue         = 0;
    int read_size       = 10 * 1024;

    auto t1 = std::chrono::high_resolution_clock::now();
    while(!std::cin.eof())
    {
        int size = read_size + residue;

        i_buffer.resize(size);
        o_buffer.resize(size * sizeof(char32_t) / sizeof(char16_t));

        auto read_count     = std::cin.read(reinterpret_cast<char*>(i_buffer.data() + residue), sizeof(char8_t) * read_size).gcount();

        int64_t i_size      = read_count + residue;
        int64_t o_size      = 0;
        int64_t i_remaining = 0;

        auto [v_size, r_size] = UnicodeBB::transcode(o_buffer.data(), i_buffer.data(), i_size);
        o_size          = v_size;
        i_remaining     = r_size;

        std::cout.write(reinterpret_cast<char*>(o_buffer.data()), o_size * sizeof(char16_t));
        o_buffer.clear();

        residue             = i_size - i_remaining;

        for (int i = 0; i < residue; ++i)
            i_buffer[i]     = i_buffer[i_remaining + i];

        i_buffer.resize(residue);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

    return duration;
}
