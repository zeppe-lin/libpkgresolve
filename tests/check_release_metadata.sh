#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail() { echo "release-metadata-test: $*" >&2; exit 1; }
grep -q "version: '1.0.0'" "$root/meson.build" || fail 'project version is not 1.0.0'
grep -q "soversion: '1'" "$root/src/meson.build" || fail 'SONAME is not 1'
grep -q "libpkgcatalog >= 1.1.0" "$root/src/meson.build" || fail 'catalog floor missing'
grep -q "libpkgstate >= 2.1.0" "$root/src/meson.build" || fail 'state floor missing'
grep -q 'libpkgresolve 1.0.0' "$root/HISTORY.md" || fail 'history release missing'
