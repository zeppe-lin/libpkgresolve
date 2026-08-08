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
endif
