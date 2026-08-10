# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later

if get_option('tests').enabled()
  foreach case : [
    ['identity', 'unit/identity_test.cpp'],
    ['model', 'unit/model_test.cpp'],
    ['witness', 'unit/witness_test.cpp'],
  ]
    executable_value = executable(
      case[0] + '-test',
      case[1],
      dependencies: libpkgresolve_dep,
      install: false,
    )
    test(case[0], executable_value, suite: 'unit')
  endforeach

  foreach case : [
    ['request', 'integration/request_test.cpp'],
    ['runtime-closure', 'integration/runtime_closure_test.cpp'],
    ['installed-authority', 'integration/installed_authority_test.cpp'],
    ['build-check', 'integration/build_check_test.cpp'],
    ['lifecycle', 'integration/lifecycle_test.cpp'],
    ['profile-goal', 'integration/profile_goal_test.cpp'],
    ['architecture', 'integration/architecture_test.cpp'],
    ['mixed-authority', 'integration/mixed_authority_test.cpp'],
    ['cycle', 'integration/cycle_test.cpp'],
    ['result-integrity', 'integration/result_integrity_test.cpp'],
    ['failure', 'integration/failure_test.cpp'],
  ]
    executable_value = executable(
      case[0] + '-test',
      case[1],
      dependencies: libpkgresolve_dep,
      install: false,
    )
    test(case[0], executable_value, suite: 'integration')
  endforeach

  resolver_headers = [
    'error.h',
    'identity.h',
    'libpkgresolve.h',
    'model.h',
    'request.h',
    'resolver.h',
    'result.h',
  ]
  foreach header : resolver_headers
    header_test = executable(
      'public-header-' + header.underscorify(),
      'unit/public_header_test.cpp',
      cpp_args: [
        '-DPKGRESOLVE_TEST_HEADER="libpkgresolve/' + header + '"',
      ],
      dependencies: libpkgresolve_dep,
      install: false,
    )
    test('public-header-' + header, header_test, suite: 'header')
  endforeach

  pkgconfig_metadata = find_program('contracts/check_pkgconfig_metadata.sh')
  test(
    'pkgconfig-metadata',
    pkgconfig_metadata,
    args: [meson.project_build_root()],
    suite: 'contract',
  )

  release_metadata = find_program('contracts/check_release_metadata.sh')
  test(
    'release-metadata',
    release_metadata,
    args: [meson.project_source_root()],
    suite: 'contract',
  )

  authority_contract = find_program('contracts/check_authority_contract.sh')
  test(
    'authority-contract',
    authority_contract,
    args: [meson.project_source_root()],
    suite: 'contract',
  )

  test_layout = find_program('contracts/check_test_layout.sh')
  test(
    'test-layout',
    test_layout,
    args: [meson.project_source_root()],
    suite: 'contract',
  )

  abi_layout = executable(
    'abi-layout-test',
    'contracts/abi_layout_test.cpp',
    dependencies: libpkgresolve_dep,
    install: false,
  )
  test('abi-layout', abi_layout, suite: 'contract')

  abi_contract = find_program('contracts/check_abi_contract.sh')
  test(
    'abi-contract',
    abi_contract,
    args: [meson.project_source_root()],
    suite: 'contract',
  )

  ci_contract = find_program('contracts/check_ci_contract.sh')
  test(
    'ci-contract',
    ci_contract,
    args: [meson.project_source_root()],
    suite: 'contract',
  )

  if library_kind == 'shared'
    abi_surface = find_program('contracts/check_abi_surface.sh')
    test(
      'abi-surface',
      abi_surface,
      args: [libpkgresolve.full_path(),
             meson.project_source_root() / 'abi/libpkgresolve.exports'],
      suite: 'contract',
    )
    dependency_abi = find_program('contracts/check_dependency_abi.sh')
    test(
      'dependency-abi',
      dependency_abi,
      args: [libpkgresolve.full_path()],
      suite: 'contract',
    )
  endif
endif
