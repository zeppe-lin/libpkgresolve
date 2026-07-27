# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  foreach case : [
    ['identity', 'identity_test.cpp'],
    ['model', 'model_test.cpp'],
    ['request', 'request_test.cpp'],
    ['resolver', 'resolver_test.cpp'],
    ['public-headers', 'public_headers.cpp'],
  ]
    executable_value = executable(
      case[0] + '-test',
      case[1],
      dependencies: libpkgresolve_dep,
      install: false,
    )
    test(case[0], executable_value)
  endforeach
  authority_contract = find_program('check_authority_contract.sh')
  test('authority-contract', authority_contract,
       args: [meson.project_source_root()])
endif
