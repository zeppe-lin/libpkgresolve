// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "test.h"

#include <libpkgresolve/libpkgresolve.h>

int main()
{
  const std::string zero(64, '0');
  const auto request = pkgresolve::resolution_request_identity::from_sha256(zero);
  const auto result = pkgresolve::resolution_result_identity::from_sha256(zero);
  TEST_CHECK(request.hex() == zero);
  TEST_CHECK(result.hex() == zero);
  TEST_CHECK(pkgresolve::resolution_request_identity::from_sha256(zero) == request);
  TEST_THROWS(pkgresolve::error_code::invalid_identity,
              pkgresolve::resolution_request_identity::from_sha256("bad"));
  TEST_THROWS(pkgresolve::error_code::invalid_identity,
              pkgresolve::resolution_request_identity::from_sha256(
                  std::string(64, 'A')));
  return 0;
}
