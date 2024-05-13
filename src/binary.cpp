#include <cstdint>
#include <fstream>
#include <vector>

#include "binary.h"

std::vector<uint8_t> extract_short_table(std::ifstream &binary, size_t offset, size_t len)
{
    std::vector<uint8_t> table(len);

    binary.seekg(offset);
    binary.read(reinterpret_cast<char*>(table.data()), len);

    table.resize(binary.gcount());
    return table;
}

