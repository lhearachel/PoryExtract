#ifndef PORYEXTRACT_ACTION_H
#define PORYEXTRACT_ACTION_H

#include <filesystem>

namespace fs = std::filesystem;

typedef void (*ExtractFunc)(fs::path &rom_contents, fs::path &repo_root, bool force_unpack);

void extract_poke_sprite(fs::path &rom_contents, fs::path &repo_root, bool force_unpack);

#endif // PORYEXTRACT_ACTION_H
