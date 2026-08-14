#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
build_root=$1
metadata=$build_root/meson-private/libpkgresolve.pc
fail()
{
  echo "metadata-test: $*" >&2
  if test -n "${metadata:-}" && test -f "$metadata"; then
    echo '--- generated metadata ---' >&2
    cat "$metadata" >&2
    echo '--- end generated metadata ---' >&2
  fi
  exit 1
}
if test ! -s "$metadata"; then
  metadata=$(find "$build_root" -type f -name libpkgresolve.pc -print |
    sed -n '1p')
fi
test -n "${metadata:-}" && test -s "$metadata" ||
  fail 'generated libpkgresolve.pc was not found'
name=$(sed -n 's/^Name:[[:space:]]*//p' "$metadata")
test "$name" = libpkgresolve ||
  fail "pkg-config module name is '$name', expected 'libpkgresolve'"
version=$(sed -n 's/^Version:[[:space:]]*//p' "$metadata")
test "$version" = 3.0.0 ||
  fail "pkg-config module version is '$version', expected '3.0.0'"
normalize_requirements()
{
  sed \
    -e 's/^[[:space:]]*//' \
    -e 's/[[:space:]]*$//' \
    -e 's/[[:space:]][[:space:]]*/ /g' \
    -e 's/ *\([<>]=\|[<>=]\) */ \1 /' \
    -e '/^$/d'
}
requires=$(sed -n 's/^Requires:[[:space:]]*//p' "$metadata" |
  tr ',' '\n' | normalize_requirements)
expected_requires='libpkgsource >= 4.0.0
libpkgsource < 5.0.0
libpkgcatalog >= 3.0.1
libpkgcatalog < 4.0.0
libpkgstate >= 3.1.0
libpkgstate < 4.0.0'
for requirement in \
  'libpkgsource >= 4.0.0' 'libpkgsource < 5.0.0' \
  'libpkgcatalog >= 3.0.1' 'libpkgcatalog < 4.0.0' \
  'libpkgstate >= 3.1.0' 'libpkgstate < 4.0.0'
do
  count=$(printf '%s\n' "$requires" | grep -Fxc "$requirement" || true)
  test "$count" -eq 1 ||
    fail "pkg-config metadata contains $count copies of '$requirement', expected exactly one"
done
actual_sorted=$(printf '%s\n' "$requires" | LC_ALL=C sort)
expected_sorted=$(printf '%s\n' "$expected_requires" | LC_ALL=C sort)
test "$actual_sorted" = "$expected_sorted" ||
  fail 'pkg-config public requirements are not the exact resolver dependency intervals'
requires_private=$(sed -n 's/^Requires\.private:[[:space:]]*//p' "$metadata" |
  tr ',' '\n' | normalize_requirements)
test "$requires_private" = libcrypto ||
  fail "pkg-config private requirements are '$requires_private', expected exactly 'libcrypto'"
libs=$(sed -n 's/^Libs:[[:space:]]*//p' "$metadata")
printf ' %s \n' "$libs" | grep -F ' -lpkgresolve ' >/dev/null ||
  fail 'pkg-config metadata omits libpkgresolve'
