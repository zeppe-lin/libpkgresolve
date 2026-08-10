#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=${1:?source root required}
meson=$root/tests/meson.build

for directory in unit integration fixtures support installed contracts; do
  [ -d "$root/tests/$directory" ] || {
    echo "test-layout: missing qualification role: $directory" >&2
    exit 1
  }
done
for escaped in "$root"/tests/*.cpp "$root"/tests/*.h "$root"/tests/check_*.sh; do
  [ ! -e "$escaped" ] || {
    echo "test-layout: uncategorized test source: $escaped" >&2
    exit 1
  }
done
for suite in unit integration header contract; do
  grep -F "suite: '$suite'" "$meson" >/dev/null || {
    echo "test-layout: Meson omits $suite suite" >&2
    exit 1
  }
done
for path in \
  unit/identity_test.cpp \
  integration/runtime_closure_test.cpp \
  integration/installed_authority_test.cpp \
  integration/build_check_test.cpp \
  integration/lifecycle_test.cpp \
  integration/profile_goal_test.cpp \
  integration/architecture_test.cpp \
  integration/result_integrity_test.cpp \
  integration/failure_test.cpp \
  installed/consumer.cpp \
  contracts/abi_layout_test.cpp \
  contracts/check_test_layout.sh \
  contracts/check_abi_contract.sh \
  contracts/check_ci_contract.sh; do
  grep -F "$path" "$meson" "$root/TESTING.md" >/dev/null || {
    echo "test-layout: qualification wiring omits $path" >&2
    exit 1
  }
done
for support_file in \
  "$root/tests/fixtures/source_state.h" \
  "$root/tests/fixtures/resolution.h" \
  "$root/tests/support/test.h" \
  "$root/tests/support/result_query.h"; do
  [ -s "$support_file" ] || {
    echo "test-layout: missing categorized support material: $support_file" >&2
    exit 1
  }
done
