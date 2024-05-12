#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <tuple>
#include <vector>

#include "action.h"
#include "env.h"
#include "knarc.h"
#include "nitrogfx.h"
#include "skip_unpack.hpp"
#include "species.h"

static std::tuple<fs::path, fs::path> unpack_narcs(fs::path &rom_contents, fs::path &knarc, bool force_unpack);
static void convert_pokegra_files(fs::path &pl_pokegra_contents, fs::path &nitrogfx, fs::path &repo_root);

void extract_poke_sprite(fs::path &rom_contents, fs::path &repo_root, bool force_unpack)
{
    fs::path knarc = std::getenv(KNARC);
    fs::path nitrogfx = std::getenv(NITROGFX);

    auto [pl_pokegra_contents, pl_otherpoke_contents] = unpack_narcs(rom_contents, knarc, force_unpack);
    convert_pokegra_files(pl_pokegra_contents, nitrogfx, repo_root);
}

static std::tuple<fs::path, fs::path> unpack_narcs(fs::path &rom_contents, fs::path &knarc, bool force_unpack)
{
    fs::path pokegra_path = rom_contents / "filesys" / "poketool" / "pokegra";
    fs::path pl_pokegra_narc = pokegra_path / "pl_pokegra.narc";
    fs::path pl_otherpoke_narc = pokegra_path / "pl_otherpoke.narc";

    fs::path pl_pokegra_contents = pl_pokegra_narc.string() + "_contents";
    fs::path pl_otherpoke_contents = pl_otherpoke_narc.string() + "_contents";

    if (!force_unpack && fs::exists(pl_pokegra_contents)) {
        skip_unpack_msg("pl_pokegra", pl_pokegra_contents);
    } else {
        knarc::unpack(knarc, pl_pokegra_narc, pl_pokegra_contents);
    }

    if (!force_unpack && fs::exists(pl_otherpoke_contents)) {
        skip_unpack_msg("pl_otherpoke", pl_otherpoke_contents);
    } else {
        knarc::unpack(knarc, pl_otherpoke_narc, pl_otherpoke_contents);
    }

    return {
        pl_pokegra_contents,
        pl_otherpoke_contents,
    };
}

static inline std::tuple<fs::path, fs::path> map_bin_ntr(fs::path &base, std::string ntr_ext)
{
    auto bin = base.string();
    auto ntr = base.replace_extension(ntr_ext);

    return { bin, ntr };
}

static void convert_pokegra_files(fs::path &pl_pokegra_contents, fs::path &nitrogfx, fs::path &repo_root)
{
    std::vector<fs::path> files;
    for (auto &entry : fs::directory_iterator(pl_pokegra_contents)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            files.push_back(entry.path());
        }
    }

    if (files.size() % 6 != 0) {
        std::cerr << "FATAL: pl_pokegra file count not divisible by 6" << std::endl;
        std::exit(1);
    }

    auto base_res_dir = repo_root / "res" / "pokemon";
    size_t species = 0;
    for (size_t j = 0; j < files.size(); j += 6, species++) {
        std::vector<std::tuple<fs::path, fs::path>> file_map = {
            map_bin_ntr(files[j],     "NCGR"), // back, female
            map_bin_ntr(files[j + 1], "NCGR"), // back, male
            map_bin_ntr(files[j + 2], "NCGR"), // front, female
            map_bin_ntr(files[j + 3], "NCGR"), // front, male
            map_bin_ntr(files[j + 4], "NCLR"), // normal
            map_bin_ntr(files[j + 5], "NCLR"), // shiny
        };

        for (auto [bin, ntr] : file_map) {
            fs::rename(bin, ntr);
        }

        std::vector<std::string> sprite_args = {
            "-palette", std::get<1>(file_map[4]).string(),
            "-object",
            "-scanfronttoback",
        };

        std::vector<std::string> palette_args = {
            "-bitdepth", "8",
        };

        auto species_dir = base_res_dir / gSpeciesNames[species];
        auto female_back_target = species_dir / "female_back.png";
        auto male_back_target = species_dir / "male_back.png";
        auto female_front_target = species_dir / "female_front.png";
        auto male_front_target = species_dir / "male_front.png";
        auto normal_pal_target = species_dir / "normal.pal";
        auto shiny_pal_target = species_dir / "shiny.pal";

        nitrogfx::call(nitrogfx, std::get<1>(file_map[0]), female_back_target, sprite_args);
        nitrogfx::call(nitrogfx, std::get<1>(file_map[1]), male_back_target, sprite_args);
        nitrogfx::call(nitrogfx, std::get<1>(file_map[2]), female_front_target, sprite_args);
        nitrogfx::call(nitrogfx, std::get<1>(file_map[3]), male_front_target, sprite_args);
        nitrogfx::call(nitrogfx, std::get<1>(file_map[4]), normal_pal_target, palette_args);
        nitrogfx::call(nitrogfx, std::get<1>(file_map[5]), shiny_pal_target, palette_args);
    }
}

