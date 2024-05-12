#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>

#include <argparse/argparse.hpp>

#include "subprocess.hpp"

namespace fs = std::filesystem;

fs::path unpack_rom(fs::path &rom_path, fs::path &ndstool_path, bool force_unpack);

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("poryextract", "1.0");

    program.add_argument("-f", "--force")
        .help("if set, force unpacking ROM, even if a contents folder is found")
        .flag();

    program.add_argument("input")
        .metavar("ROM")
        .help("path to a US Platinum ROM to unpack")
        .nargs(1);

    program.add_argument("actions")
        .metavar("ACTIONS")
        .help("an element to unpack from ROM")
        .nargs(argparse::nargs_pattern::any);

    fs::path knarc_exe = std::getenv("KNARC");
    fs::path nitrogfx_exe = std::getenv("NITROGFX");
    fs::path ndstool_exe = std::getenv("NDSTOOL");

    try {
        program.parse_args(argc, argv);
    } catch (std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program << std::endl;
        std::exit(1);
    }

    fs::path rom_path(program.get("input"));
    auto actions = program.get<std::vector<std::string>>("actions");
    auto force_unpack = program.get<bool>("force");

    unpack_rom(rom_path, ndstool_exe, force_unpack);

    return 0;
}

fs::path unpack_rom(fs::path &rom_path, fs::path &ndstool_path, bool force_unpack)
{
    fs::path unpack_target = rom_path.filename().string() + "_contents";
    if (!force_unpack && fs::exists(unpack_target)) {
        std::cout << "Skipping ROM unpack; contents directory " << unpack_target.filename() << " already exists" << std::endl;
        return unpack_target;
    }

    fs::create_directory(unpack_target);

    subprocess::popen unpack(ndstool_path, {
        "-x", rom_path,
        "-9", unpack_target / "arm9.bin",
        "-7", unpack_target / "arm7.bin",
        "-y9", unpack_target / "overarm9.bin",
        "-y7", unpack_target / "overarm7.bin",
        "-d", unpack_target / "filesys",
        "-y", unpack_target / "overlay",
        "-t", unpack_target / "banner.bin",
        "-h", unpack_target / "header.bin",
    });

    return unpack_target;
}

