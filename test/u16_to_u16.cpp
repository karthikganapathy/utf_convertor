#include <iostream>
#include <fstream>
#include <chrono>
#include "utf/unify.hpp"
#include "util/argument_parser.hpp"

int main(int argc, char** argv)
{
    util::ArgumentParser    arg_parser;

    arg_parser.add_options()
        ('h', "help",   "Display Usage", false)
        ('i', "input",  "Enter input file name",  true, true)
        ('o', "output", "Enter output file name", true, true)
        ('e', "iendian","Input Endianness <big|little> default: big", true)
        ('u', "oendian","Output Endianness <big|little> default: big", true);

    if (!arg_parser.parse(argc, argv))
    {
        std::cerr << "ERROR: Missing Required Arguments\n";
        arg_parser.usage();
        return 0;
    }

    if (arg_parser.is_set('h'))
    {
        arg_parser.usage();
        return 0;
    }

    std::string input_filename  = arg_parser.get("input");
    std::string output_filename = arg_parser.get("output");
    std::string input_endian    = arg_parser.get("iendian");
    std::string output_endian   = arg_parser.get("oendian");

    std::ifstream input(input_filename, std::ios::in);
    if (!input.is_open())
    {
        std::cerr << "ERROR: Not able to open input file " << input_filename << ", for reading\n";
        return 0;
    }

    std::ofstream output(output_filename, std::ios::out | std::ios::app);
    if (!output.is_open())
    {
        std::cerr << "ERROR: Not able to open output file " << output_filename << ", for writing\n";
        input.close();
        return 0;
    }

    std::vector<char16_t>   i_buffer;
    std::vector<char16_t>   o_buffer;

    enum class Endianness : std::uint8_t
    {
        Big_Big         = 0,
        Big_Little      = 1,
        Little_Big      = 2,
        Little_Little   = 3
    };

    Endianness endian;

    if (input_endian == "big" && output_endian == "big")
        endian  = Endianness::Big_Big;
    else if (input_endian == "big" && output_endian != "big")
        endian  = Endianness::Big_Little;
    else if (input_endian != "big" && output_endian == "big")
        endian  = Endianness::Little_Big;
    else
        endian  = Endianness::Little_Little;

    using   UnicodeBB   = utf::UniFy<char16_t, char16_t, true, true>;
    using   UnicodeBL   = utf::UniFy<char16_t, char16_t, false, true>;
    using   UnicodeLB   = utf::UniFy<char16_t, char16_t, true, false>;
    using   UnicodeLL   = utf::UniFy<char16_t, char16_t, false, false>;

    int residue         = 0;
    int read_size       = 10 * 1024;

    auto t1 = std::chrono::high_resolution_clock::now();
    while(!input.eof())
    {
        int size = read_size + residue;

        i_buffer.resize(size);
        o_buffer.resize(size * sizeof(char16_t) / sizeof(char16_t));

        auto read_count     = input.read(reinterpret_cast<char*>(i_buffer.data() + residue), sizeof(char16_t) * read_size).gcount();

        int64_t i_size      = read_count / sizeof(char16_t) + residue;
        int64_t o_size      = 0;
        int64_t i_remaining = 0;

        if (endian == Endianness::Big_Big)
        {
            auto [v_size, r_size] = UnicodeBB::transcode(o_buffer.data(), i_buffer.data(), i_size);
            o_size          = v_size;
            i_remaining     = r_size;
        }
        else if (endian == Endianness::Big_Little)
        {
            auto [v_size, r_size] = UnicodeBL::transcode(o_buffer.data(), i_buffer.data(), i_size);
            o_size          = v_size;
            i_remaining     = r_size;
        }
        else if (endian == Endianness::Little_Big)
        {
            auto [v_size, r_size] = UnicodeLB::transcode(o_buffer.data(), i_buffer.data(), i_size);
            o_size          = v_size;
            i_remaining     = r_size;
        }
        else
        {
            auto [v_size, r_size] = UnicodeLL::transcode(o_buffer.data(), i_buffer.data(), i_size);
            o_size          = v_size;
            i_remaining     = r_size;
        }

        output.write(reinterpret_cast<char*>(o_buffer.data()), o_size * sizeof(char16_t));
        o_buffer.clear();

        residue             = i_size - i_remaining;

        for (int i = 0; i < residue; ++i)
            i_buffer[i]     = i_buffer[i_remaining + i];

        i_buffer.resize(residue);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

    input.close();
    output.close();

    return duration;
}
