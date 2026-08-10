#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=${1:?source root required}
fail() { echo "abi-contract: $*" >&2; exit 1; }
manifest=$root/abi/libpkgresolve.exports
[ -s "$manifest" ] || fail 'reviewed ELF ABI manifest is absent'
[ "$(sed -n '/^_Z[A-Za-z0-9_]*$/p' "$manifest" | wc -l)" -eq 134 ] ||
  fail 'reviewed ELF ABI manifest must contain exactly 134 symbols'
[ "$(LC_ALL=C sort -u "$manifest" | wc -l)" -eq 134 ] ||
  fail 'reviewed ELF ABI manifest contains duplicate symbols'
! grep -F '_ZN10pkgresolve6detail' "$manifest" >/dev/null ||
  fail 'private detail namespace entered public ABI manifest'
! grep -E '^_ZNSt|^_ZN9__gnu_cxx' "$manifest" >/dev/null ||
  fail 'standard-library implementation symbol entered public ABI manifest'
for identity in \
  resolution_request_identity package_selection_identity \
  requirement_edge_identity goal_closure_identity resolution_result_identity
do
  ! c++filt < "$manifest" | grep -F "pkgresolve::$identity::$identity(std::__cxx11::basic_string" >/dev/null ||
    fail "private $identity constructor entered public ABI manifest"
done
! c++filt < "$manifest" | grep -F 'pkgresolve::resolution_request::resolution_request(' >/dev/null ||
  fail 'private resolution-request constructor entered public ABI manifest'
! c++filt < "$manifest" | grep -F 'pkgresolve::requirement_witness::requirement_witness(' >/dev/null ||
  fail 'private requirement-witness constructor entered public ABI manifest'
grep -F '_ZN10pkgresolve7resolveENS_18resolution_requestE' "$manifest" >/dev/null ||
  fail 'resolve() is absent from reviewed ABI'
grep -F '_ZTIN10pkgresolve5errorE' "$manifest" >/dev/null ||
  fail 'public error RTTI is absent from reviewed ABI'
grep -F "soversion: '3'" "$root/src/meson.build" >/dev/null ||
  fail 'SONAME generation is not 3'
grep -F -- '--version-script=' "$root/src/meson.build" >/dev/null ||
  fail 'reviewed ELF export manifest is not linked'
grep -F '../abi/libpkgresolve.exports' "$root/src/meson.build" >/dev/null ||
  fail 'Meson does not consume reviewed ABI manifest'
grep -F '134-symbol' "$root/MAINTAINING.md" >/dev/null ||
  fail 'reviewed ABI inventory is undocumented'
