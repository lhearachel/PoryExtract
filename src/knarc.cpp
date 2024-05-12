#include "knarc.h"
#include "subprocess.hpp"

void knarc::unpack(fs::path &knarc, fs::path &narc_path, fs::path &narc_contents)
{
    subprocess::popen unpack(knarc, {
        "-u", narc_path,
        "-d", narc_contents,
    });

    unpack.wait();
}

