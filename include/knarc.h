#ifndef PORYEXTRACT_KNARC_H
#define PORYEXTRACT_KNARC_H

#include <filesystem>

namespace fs = std::filesystem;

namespace knarc {

void unpack(fs::path &knarc, fs::path &narc_path, fs::path &narc_contents);

};

#endif // PORYEXTRACT_KNARC_H

