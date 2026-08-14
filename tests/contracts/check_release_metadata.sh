#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail() { echo "release-metadata-test: $*" >&2; exit 1; }
grep -q "version: '3.0.0'" "$root/meson.build" || fail 'project version is not 3.0.0'
grep -q "soversion: '3'" "$root/src/meson.build" || fail 'SONAME is not 3'
for dependency in libpkgsource_dep libpkgcatalog_dep libpkgstate_dep
do
  count=$(grep -Fxc "    $dependency," "$root/src/meson.build" || true)
  test "$count" -eq 1 ||
    fail "pkg-config public requirement '$dependency' occurs $count times, expected once"
done
count=$(grep -Fxc '  requires_private: [libcrypto_dep],' "$root/src/meson.build" || true)
test "$count" -eq 1 ||
  fail "pkg-config private requirement 'libcrypto_dep' occurs $count times, expected once"
check_dependency_range()
{
  variable=$1
  module=$2
  interval=$3
  block=$(sed -n "/^$variable = dependency(/,/^)/p" "$root/meson.build")
  printf '%s\n' "$block" | grep -F "  '$module'," >/dev/null ||
    fail "$variable does not resolve $module"
  printf '%s\n' "$block" | grep -F "  version: $interval," >/dev/null ||
    fail "$module dependency interval is not $interval"
}
check_dependency_range libpkgsource_dep libpkgsource "['>=4.0.0', '<5.0.0']"
check_dependency_range libpkgcatalog_dep libpkgcatalog "['>=3.0.1', '<4.0.0']"
check_dependency_range libpkgstate_dep libpkgstate "['>=3.1.0', '<4.0.0']"
! grep -F 'acquisition_adapter=disabled' "$root/meson.build" >/dev/null ||
  fail 'obsolete catalog adapter fallback option remains'
! grep -F 'source_adapter=disabled' "$root/meson.build" >/dev/null ||
  fail 'obsolete state adapter fallback option remains'
grep -q 'libpkgresolve 3.0.0' "$root/HISTORY.md" || fail 'history release missing'
