# PoryExtract

A utility for extracting data and assets from a Pokemon Platinum ROM for use in a [pret/pokeplatinum](https://github.com/pret/pokeplatinum) project.
```
Usage: poryextract [--help] [--version] [--force] ROM REPO_ROOT ACTIONS

Positional arguments:
  ROM            path to a US Platinum ROM to unpack
  REPO_ROOT      path to your pokeplatinum repository
  ACTIONS        an element to unpack from ROM

Optional arguments:
  -h, --help     shows help message and exits
  -v, --version  prints version information and exits
  -f, --force    if set, force unpacking ROM, even if a contents folder is found
```

## Requirements

- `meson`
- `g++` - Version 10 or higher (for `c++20` support)

## Setup

1. Setup the `meson` chain: `meson setup build`
2. Compile the executable: `meson compile -C build`
3. Source the generated `.env` file: `source build/.env`
4. Run the executable as desired: `./build/poryextract ...`

## Supported Actions

- `poke_sprites` - Extract Pokemon battle sprites
- ...

