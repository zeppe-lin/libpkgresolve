# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
public_inc = include_directories('../include')
private_inc = include_directories('.')

libpkgresolve = library(
  'pkgresolve',
  [
    'error.cpp',
    'identity.cpp',
    'identity_support.cpp',
    'model.cpp',
    'request.cpp',
  ],
  include_directories: [public_inc, private_inc],
  dependencies: [libcrypto_dep, libpkgcatalog_dep, libpkgstate_dep],
  install: true,
  version: meson.project_version(),
  soversion: '0',
)

libpkgresolve_dep = declare_dependency(
  link_with: libpkgresolve,
  include_directories: public_inc,
  dependencies: [libcrypto_dep, libpkgcatalog_dep, libpkgstate_dep],
)
meson.override_dependency('libpkgresolve', libpkgresolve_dep)

install_headers(
  '../include/libpkgresolve/libpkgresolve.h',
  '../include/libpkgresolve/error.h',
  '../include/libpkgresolve/identity.h',
  '../include/libpkgresolve/model.h',
  '../include/libpkgresolve/request.h',
  subdir: 'libpkgresolve',
)

pkgconf.generate(
  libpkgresolve,
  name: 'libpkgresolve',
  filebase: 'libpkgresolve',
  description: 'Native deterministic package-resolution authority',
  subdirs: '.',
  requires: [
    'libpkgcatalog >= 1.1.0',
    'libpkgstate >= 2.1.0',
  ],
  requires_private: ['libcrypto'],
)
