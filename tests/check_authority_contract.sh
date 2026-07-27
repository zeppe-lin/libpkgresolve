#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail() { echo "authority-contract-test: $*" >&2; exit 1; }
for term in 'Pkgfile' 'pkgman.conf' 'build_and_run' 'filesystem traversal' 'package installation'; do
  if grep -R -n --include='*.h' --include='*.cpp' "$term" "$root/include" "$root/src" >/dev/null; then
    fail "forbidden compatibility or orchestration term: $term"
  fi
done
grep -q 'does not plan' "$root/DESIGN.md" || fail 'planning boundary missing'
grep -q 'effective catalog candidates only' "$root/DESIGN.md" || fail 'candidate boundary missing'
grep -q 'build, check, run, and lifecycle' "$root/DESIGN.md" || fail 'typed closure boundary missing'
