#ifndef PORYEXTRACT_BINARY_H
#define PORYEXTRACT_BINARY_H

#include <cstdint>
#include <fstream>
#include <vector>

std::vector<uint8_t> extract_short_table(std::ifstream &binary, size_t offset, size_t len);

#endif // PORYEXTRACT_BINARY_H

