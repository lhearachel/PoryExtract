#include <filesystem>
#include <string>
#include <vector>

#include "nitrogfx.h"
#include "subprocess.hpp"

namespace fs = std::filesystem;

void nitrogfx::call(fs::path &nitrogfx, fs::path &in, fs::path &out, std::vector<std::string> &args...)
{
    std::vector<std::string> all_args = { in, out };
    for (auto &extra_arg : args) {
        all_args.push_back(extra_arg);
    }

    subprocess::popen call(nitrogfx, all_args);
    call.wait();
}

