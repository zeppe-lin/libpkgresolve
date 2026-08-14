# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
public_inc = include_directories('../include')
private_inc = include_directories('.')

elf_export_generator = find_program(
  meson.project_source_root() / 'tools/generate-elf-export-script.sh',
)
elf_export_script_name = 'libpkgresolve.exports.map'
elf_export_script_path = meson.current_build_dir() / elf_export_script_name
elf_export_script = configure_file(
  input: '../abi/libpkgresolve.exports',
  output: elf_export_script_name,
  command: [elf_export_generator, '@INPUT@'],
  capture: true,
)

libpkgresolve = library(
  'pkgresolve',
  [
    'error.cpp',
    'identity.cpp',
    'identity_support.cpp',
    'model.cpp',
    'request.cpp',
    'result.cpp',
    'resolver.cpp',
  ],
  include_directories: [public_inc, private_inc],
  dependencies: [
    libcrypto_dep, libpkgsource_dep, libpkgcatalog_dep, libpkgstate_dep,
  ],
  link_args: ['-Wl,--version-script=' + elf_export_script_path],
  link_depends: [elf_export_script],
  install: true,
  version: meson.project_version(),
  soversion: '4',
)

libpkgresolve_consumer_deps = [
  libpkgsource_dep, libpkgcatalog_dep, libpkgstate_dep,
]
if want_static
  libpkgresolve_consumer_deps += [libcrypto_dep]
endif

libpkgresolve_dep = declare_dependency(
  link_with: libpkgresolve,
  include_directories: public_inc,
  dependencies: libpkgresolve_consumer_deps,
  version: meson.project_version(),
)
meson.override_dependency('libpkgresolve', libpkgresolve_dep)

install_headers(
  '../include/libpkgresolve/libpkgresolve.h',
  '../include/libpkgresolve/error.h',
  '../include/libpkgresolve/identity.h',
  '../include/libpkgresolve/model.h',
  '../include/libpkgresolve/request.h',
  '../include/libpkgresolve/result.h',
  '../include/libpkgresolve/resolver.h',
  subdir: 'libpkgresolve',
)

pkgconf.generate(
  libpkgresolve,
  name: 'libpkgresolve',
  filebase: 'libpkgresolve',
  description: 'Native deterministic package-resolution authority',
  subdirs: '.',
  requires: [
    libpkgsource_dep,
    libpkgcatalog_dep,
    libpkgstate_dep,
  ],
  requires_private: [libcrypto_dep],
)
