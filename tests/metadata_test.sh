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
test "$version" = 2.0.0 ||
  fail "pkg-config module version is '$version', expected '2.0.0'"
requires=$(sed -n \
  -e 's/^Requires:[[:space:]]*//p' \
  -e 's/^Requires\.private:[[:space:]]*//p' \
  "$metadata" | tr '\n' ',')
printf '%s\n' "$requires" |
  grep -Eq '(^|,)[[:space:]]*libpkgcatalog[[:space:]]*>=[[:space:]]*2\.0\.0([[:space:]]*,|$)' ||
  fail 'pkg-config metadata omits libpkgcatalog >= 2.0.0'
printf '%s\n' "$requires" |
  grep -Eq '(^|,)[[:space:]]*libpkgstate[[:space:]]*>=[[:space:]]*2\.1\.0([[:space:]]*,|$)' ||
  fail 'pkg-config metadata omits libpkgstate >= 2.1.0'
libs=$(sed -n 's/^Libs:[[:space:]]*//p' "$metadata")
printf ' %s \n' "$libs" | grep -F ' -lpkgresolve ' >/dev/null ||
  fail 'pkg-config metadata omits libpkgresolve'
