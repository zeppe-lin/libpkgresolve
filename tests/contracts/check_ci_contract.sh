#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=${1:?source root required}
fail() { echo "ci-contract: $*" >&2; exit 1; }
workflow=$root/.github/workflows/ci.yml
configure=$root/ci/configure-and-test.sh
[ -s "$workflow" ] || fail 'hosted CI workflow is absent'
[ -x "$configure" ] || fail 'qualification driver is absent or not executable'
for compiler in 'GCC shared' 'GCC static' 'Clang shared' 'Clang static'; do
  grep -F "$compiler" "$workflow" >/dev/null || fail "CI omits $compiler"
done
grep -F 'address,undefined' "$workflow" >/dev/null || fail 'CI omits ASan/UBSan qualification'
for pin in \
  'd5f30663a4e56c2319f301ca762741106dea1bd0' \
  '16976cac176f576871e327d5d2f6fe9d9dfa0666' \
  'f74df278b47b48e798c3de01c922c59b58319d13'
do
  grep -F "$pin" "$workflow" >/dev/null || fail "CI omits current authority pin $pin"
done
grep -F 'pkg-config --static --libs libpkgresolve' "$configure" >/dev/null ||
  fail 'static installed consumer does not use pkg-config --static'
grep -F 'tests/installed/consumer.cpp' "$configure" >/dev/null ||
  fail 'installed consumer is not executed'
grep -F 'MESON_SANITIZE' "$configure" >/dev/null ||
  fail 'qualification driver does not forward sanitizer configuration'
