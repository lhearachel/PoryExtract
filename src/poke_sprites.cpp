#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iostream>
#include <tuple>
#include <vector>

#include <indicators/progress_bar.hpp>
#include <indicators/block_progress_bar.hpp>

#include "action.h"
#include "env.h"
#include "knarc.h"
#include "nitrogfx.h"
#include "species.h"
#include "wrap.h"

#define MAKE_SPRITE_ARGS(palette_path) { \
    "-palette", palette_path, \
    "-scanfronttoback", \
};

static void convert_pokegra_files(fs::path &pl_pokegra_contents, fs::path &nitrogfx, fs::path &repo_root, indicators::ProgressBar &bar);
static void convert_otherpoke_files(fs::path &pl_otherpoke_contents, fs::path &nitrogfx, fs::path &repo_root, indicators::ProgressBar &bar);

void extract_poke_sprite(fs::path &rom_contents, fs::path &repo_root, bool force_unpack)
{
    fs::path knarc = std::getenv(KNARC);
    fs::path nitrogfx = std::getenv(NITROGFX);

    fs::path parent = rom_contents / "filesys" / "poketool" / "pokegra";
    fs::path pl_pokegra_narc   = parent / "pl_pokegra.narc";
    fs::path pl_otherpoke_narc = parent / "pl_otherpoke.narc";

    auto unpack_pokegra   = std::bind(knarc::unpack, knarc, pl_pokegra_narc, force_unpack);
    auto unpack_otherpoke = std::bind(knarc::unpack, knarc, pl_otherpoke_narc, force_unpack);

    auto pokegra_dir   = wrap_unpack(pl_pokegra_narc.filename().string(), unpack_pokegra);
    auto otherpoke_dir = wrap_unpack(pl_otherpoke_narc.filename().string(), unpack_otherpoke);

    indicators::ProgressBar pokegra_bar {
        indicators::option::BarWidth { 80 },
        indicators::option::ForegroundColor { indicators::Color::white },
        indicators::option::FontStyles {
            std::vector<indicators::FontStyle> { indicators::FontStyle::bold },
        },
        indicators::option::PrefixText { "pl_pokegra   " },
    };

    indicators::ProgressBar otherpoke_bar {
        indicators::option::BarWidth { 80 },
        indicators::option::ForegroundColor { indicators::Color::white },
        indicators::option::FontStyles {
            std::vector<indicators::FontStyle> { indicators::FontStyle::bold },
        },
        indicators::option::PrefixText { "pl_otherpoke " },
    };

    std::cout << "\nExtracting Pokemon Sprites..." << std::endl;

    convert_pokegra_files(pokegra_dir, nitrogfx, repo_root, pokegra_bar);
    convert_otherpoke_files(otherpoke_dir, nitrogfx, repo_root, otherpoke_bar);

    std::cout << "\e[1;32m✔ Done\e[22;0m" << std::endl;
}

static inline std::tuple<fs::path, fs::path> map_bin_ntr(fs::path &base, std::string ntr_ext)
{
    auto bin = base.string();
    auto ntr = base.replace_extension(ntr_ext);

    return { bin, ntr };
}

static void convert_pokegra_files(fs::path &pl_pokegra_contents, fs::path &nitrogfx, fs::path &repo_root, indicators::ProgressBar &bar)
{
    std::vector<fs::path> files;
    for (auto &entry : fs::directory_iterator(pl_pokegra_contents)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            files.push_back(entry.path());
        }
    }

    if (files.size() % 6 != 0) {
        std::cerr << "FATAL: pl_pokegra file count not divisible by 6" << std::endl;
        bar.mark_as_completed();
        return;
    }

    auto max_species = files.size() / 6;
    bar.set_option(indicators::option::MaxProgress { max_species });

    auto base_res_dir = repo_root / "res" / "pokemon";
    size_t species = 0;
    std::vector<std::string> palette_args = { "-bitdepth", "8" };

    for (size_t j = 0; j < files.size(); j += 6, species++) {
        bar.set_option(indicators::option::PostfixText {
            "converting " + gSpeciesNames[species]
        });

        std::vector<std::tuple<fs::path, fs::path>> file_map = {
            map_bin_ntr(files[j],     "NCGR"), // back, female
            map_bin_ntr(files[j + 1], "NCGR"), // back, male
            map_bin_ntr(files[j + 2], "NCGR"), // front, female
            map_bin_ntr(files[j + 3], "NCGR"), // front, male
            map_bin_ntr(files[j + 4], "NCLR"), // normal
            map_bin_ntr(files[j + 5], "NCLR"), // shiny
        };

        for (auto [bin, ntr] : file_map) {
            fs::copy_file(bin, ntr, fs::copy_options::overwrite_existing);
        }

        std::vector<std::string> sprite_args = MAKE_SPRITE_ARGS(std::get<1>(file_map[4]).string());

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

        bar.tick();
    }
}

static void convert_otherpoke_files(fs::path &pl_otherpoke_contents, fs::path &nitrogfx, fs::path &repo_root, indicators::ProgressBar &bar)
{
    fs::path poke_res_root = repo_root / "res" / "pokemon";

    std::vector<fs::path> files;
    for (auto &entry : fs::directory_iterator(pl_otherpoke_contents)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            files.push_back(entry.path());
        }
    }

    if (files.size() != gOtherpokePaths.size()) {
        std::cerr << "FATAL: pl_otherpoke file count not equal to " << gOtherpokePaths.size() << std::endl;
        bar.mark_as_completed();
        return;
    }

    for (size_t i = 0; i < gOtherpokePaths.size(); i++) {
        auto target_path = fs::path(gOtherpokePaths[i]);
        auto bin_path = files[i].string();

        fs::path ntr_path;
        if (target_path.extension() == ".png") {
            ntr_path = files[i].replace_extension(".NCGR");
        } else { // .pal
            ntr_path = files[i].replace_extension(".NCLR");
        }

        fs::copy_file(bin_path, ntr_path, fs::copy_options::overwrite_existing);
    }

    bar.set_option(indicators::option::MaxProgress { gOtherpokePaths.size() });

    for (size_t i = 0; i < gOtherpokePals.size(); i++) {
        auto base_path = gOtherpokePaths[i];
        bar.set_option(indicators::option::PostfixText { "converting " + base_path });

        auto ntr_path = files[i].replace_extension(".NCGR");
        auto png_path = poke_res_root / fs::path(base_path);
        auto pal_path = files[gOtherpokePals[i]];

        std::vector<std::string> sprite_args = MAKE_SPRITE_ARGS(pal_path.string());

        nitrogfx::call(nitrogfx, ntr_path, png_path, sprite_args);
        bar.tick();
    }

    std::vector<std::string> palette_args = { "-bitdepth", "8" };
    for (size_t i = gOtherpokePals.size(); i < 248; i++) {
        auto base_path = gOtherpokePaths[i];
        bar.set_option(indicators::option::PostfixText { "converting " + base_path });

        auto ntr_path = files[i].replace_extension(".NCLR");
        auto pal_path = poke_res_root / fs::path(gOtherpokePaths[i]);

        nitrogfx::call(nitrogfx, ntr_path, pal_path, palette_args);
        bar.tick();
    }

    bar.set_option(indicators::option::PostfixText { "converting " + gOtherpokePaths[248] });
    auto ntr_path = files[248].replace_extension(".NCGR"); // substitute back sprite
    auto out_path = poke_res_root / fs::path(gOtherpokePaths[248]);
    auto pal_path = files[250].replace_extension(".NCLR");
    std::vector<std::string> sprite_args = MAKE_SPRITE_ARGS(pal_path);
    nitrogfx::call(nitrogfx, ntr_path, out_path, sprite_args);
    bar.tick();

    bar.set_option(indicators::option::PostfixText { "converting " + gOtherpokePaths[249] });
    ntr_path = files[249].replace_extension(".NCGR"); // substitute front sprite
    out_path = poke_res_root / fs::path(gOtherpokePaths[249]);
    nitrogfx::call(nitrogfx, ntr_path, out_path, sprite_args);
    bar.tick();

    bar.set_option(indicators::option::PostfixText { "converting " + gOtherpokePaths[250] });
    out_path = poke_res_root / fs::path(gOtherpokePaths[250]); // substitute palette
    nitrogfx::call(nitrogfx, pal_path, out_path, palette_args);
    bar.tick();

    bar.set_option(indicators::option::PostfixText { "converting " + gOtherpokePaths[251] });
    ntr_path = files[251].replace_extension(".NCGR"); // shadow sprite
    out_path = poke_res_root / fs::path(gOtherpokePaths[251]);
    pal_path = files[252].replace_extension(".NCLR");
    sprite_args = MAKE_SPRITE_ARGS(pal_path);
    nitrogfx::call(nitrogfx, ntr_path, out_path, sprite_args);
    bar.tick();

    bar.set_option(indicators::option::PostfixText { "converting " + gOtherpokePaths[252] });
    out_path = poke_res_root / fs::path(gOtherpokePaths[252]); // shadow palette
    nitrogfx::call(nitrogfx, pal_path, out_path, palette_args);
    bar.tick();
}

