#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <unordered_map>

#include <argparse/argparse.hpp>

#include "action.h"
#include "env.h"
#include "skip_unpack.hpp"
#include "subprocess.hpp"

namespace fs = std::filesystem;

fs::path unpack_rom(fs::path &rom_path, bool force_unpack);

static const std::unordered_map<std::string, ExtractFunc> EXTRACT_ACTIONS = {
    { "poke_sprites", extract_poke_sprite },
};

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("poryextract", "1.0");

    program.add_argument("-f", "--force")
        .help("if set, force unpacking ROM, even if a contents folder is found")
        .flag();

    program.add_argument("input")
        .metavar("ROM")
        .help("path to a US Platinum ROM to unpack")
        .nargs(1);

    program.add_argument("repo_root")
        .metavar("REPO_ROOT")
        .help("path to your pokeplatinum repository")
        .nargs(1);

    program.add_argument("actions")
        .metavar("ACTIONS")
        .help("an element to unpack from ROM")
        .nargs(argparse::nargs_pattern::any);

    try {
        program.parse_args(argc, argv);
    } catch (std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program << std::endl;
        std::exit(1);
    }

    fs::path rom_path(program.get("input"));
    fs::path repo_path(program.get("repo_root"));
    auto actions = program.get<std::vector<std::string>>("actions");
    auto force_unpack = program.get<bool>("force");

    auto rom_contents_root = unpack_rom(rom_path, force_unpack);

    for (auto &action : actions) {
        auto match = EXTRACT_ACTIONS.find(action);
        if (match == EXTRACT_ACTIONS.end()) {
            std::cout << "Unrecognized action: " << action << std::endl;
            continue;
        }

        match->second(rom_contents_root, repo_path, force_unpack);
    }

    return 0;
}

fs::path unpack_rom(fs::path &rom_path, bool force_unpack)
{
    fs::path unpack_target = rom_path.filename().string() + "_contents";
    if (!force_unpack && fs::exists(unpack_target)) {
        skip_unpack_msg("ROM", unpack_target.filename());
        return unpack_target;
    }

    fs::create_directory(unpack_target);
    fs::path ndstool_path = std::getenv(NDSTOOL);

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
    unpack.wait();

    return unpack_target;
}

