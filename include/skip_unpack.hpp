#ifndef PORYEXTRACT_SKIP_UNPACK_HPP
#define PORYEXTRACT_SKIP_UNPACK_HPP

#include <iostream>

static inline void skip_unpack_msg(std::string name, std::string dir) {
    std::cout << "Skipping " << name << " unpack; contents directory " << dir << " already exists" << std::endl;
}

#endif // PORYEXTRACT_SKIP_UNPACK_HPP

