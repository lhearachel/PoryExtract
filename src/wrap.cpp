#include <filesystem>
#include <functional>
#include <iostream>
#include <string>

#include "wrap.h"

fs::path wrap_unpack(std::string fname, std::function<path_unpacked()> job)
{
    std::cout << "\e[1m" << "Unpacking " << fname << "... ";
    auto [contents, unpacked] = job();

    std::cout << (unpacked ? "\e[1;32m✔ Done."
                           : "\e[1;33mℹ Skipped.")
              << "\e[22;0m" << std::endl;

    return contents;
}

