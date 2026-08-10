#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail() { echo "release-metadata-test: $*" >&2; exit 1; }
grep -q "version: '3.0.0'" "$root/meson.build" || fail 'project version is not 3.0.0'
grep -q "soversion: '3'" "$root/src/meson.build" || fail 'SONAME is not 3'
for dependency in \
  "libpkgsource >= 3.0.1" "libpkgsource < 4.0.0" \
  "libpkgcatalog >= 3.0.1" "libpkgcatalog < 4.0.0" \
  "libpkgstate >= 3.1.0" "libpkgstate < 4.0.0"
do
  grep -F "$dependency" "$root/src/meson.build" >/dev/null ||
    fail "release metadata omits $dependency"
done
grep -F "version: ['>=3.0.1', '<4.0.0']" "$root/meson.build" >/dev/null ||
  fail 'source/catalog generation ceilings are missing'
grep -F "version: ['>=3.1.0', '<4.0.0']" "$root/meson.build" >/dev/null ||
  fail 'state generation ceiling is missing'
! grep -F 'acquisition_adapter=disabled' "$root/meson.build" >/dev/null ||
  fail 'obsolete catalog adapter fallback option remains'
! grep -F 'source_adapter=disabled' "$root/meson.build" >/dev/null ||
  fail 'obsolete state adapter fallback option remains'
grep -q 'libpkgresolve 3.0.0' "$root/HISTORY.md" || fail 'history release missing'
