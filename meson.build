project('PoryExtract', 'cpp')

subdir('src')

argparse = dependency('argparse')
json = dependency('nlohmann_json')
indicators = dependency('indicators')

knarc_exe = find_program('knarc', native: true)
nitrogfx_exe = find_program('nitrogfx', native: true)
ndstool_exe = find_program('ndstool', native: true)
configure_env = find_program('configure_env', native: true)

custom_target('configure',
    output: '.env',
    command: [
        configure_env,
        '--knarc', knarc_exe,
        '--nitrogfx', nitrogfx_exe,
        '--ndstool', ndstool_exe,
    ],
    build_by_default: true
)

executable('poryextract',
    sources: cpp_sources,
    cpp_args: [
        '-std=c++20',
    ],
    include_directories: include_directories('include'),
    dependencies: [
        argparse,
        json,
        indicators,
    ]
)
