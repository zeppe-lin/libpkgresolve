// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgresolve/libpkgresolve.h>

#include <cstddef>

static_assert(sizeof(void*) == 8,
              "libpkgresolve 3 ABI layout contract requires 64-bit pointers");
static_assert(sizeof(pkgsource::source_snapshot) == 712);
static_assert(alignof(pkgsource::source_snapshot) == 8);
static_assert(sizeof(pkgcatalog::catalog_candidate) == 896);
static_assert(sizeof(pkgcatalog::catalog_snapshot) == 112);
static_assert(sizeof(pkgstate::installed_package) == 1544);
static_assert(sizeof(pkgstate::snapshot) == 336);
static_assert(sizeof(pkgresolve::selection_authority) == 1552);
static_assert(sizeof(pkgresolve::selected_package) == 1792);
static_assert(sizeof(pkgresolve::resolution_request) == 576);
static_assert(sizeof(pkgresolve::resolution_result) == 704);

int main()
{
  return 0;
}
