#include "knarc.h"
#include "subprocess.hpp"
#include "wrap.h"

path_unpacked knarc::unpack(fs::path &knarc, fs::path &src, bool force)
{
    fs::path dst = src.string() + "_contents";
    if (!force && fs::exists(dst)) {
        return { dst, false };
    }

    subprocess::popen unpack(knarc, {
        "-u", src,
        "-d", dst,
    });

    unpack.wait();
    return { dst, true };
}

