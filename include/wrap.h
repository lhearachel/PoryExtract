#ifndef PORYEXTRACT_INFO_MESSAGE_H
#define PORYEXTRACT_INFO_MESSAGE_H

#include <filesystem>
#include <functional>
#include <string>

namespace fs = std::filesystem;

typedef std::tuple<fs::path, bool> path_unpacked;

fs::path wrap_unpack(std::string name, std::function<path_unpacked()> job);

#endif  // PORYEXTRACT_INFO_MESSAGE_H

