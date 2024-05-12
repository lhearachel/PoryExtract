#ifndef PORYEXTRACT_KNARC_H
#define PORYEXTRACT_KNARC_H

#include <filesystem>

#include "wrap.h"

namespace fs = std::filesystem;

namespace knarc {

path_unpacked unpack(fs::path &knarc, fs::path &src, bool force);

};

#endif // PORYEXTRACT_KNARC_H

