project('PoryExtract', 'cpp')

subdir('src')

argparse = dependency('argparse')
json = dependency('nlohmann_json')

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
    ]
)

executable('poryextract',
    sources: cpp_sources,
    include_directories: include_directories('include'),
    dependencies: [
        argparse,
        json,
    ]
)
