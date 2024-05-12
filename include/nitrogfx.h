#ifndef PORYEXTRACT_NITROGFX_H
#define PORYEXTRACT_NITROGFX_H

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace nitrogfx {

void call(fs::path &nitrogfx, fs::path &in, fs::path &out, std::vector<std::string> &args...);

};

#endif // PORYEXTRACT_NITROGFX_H

