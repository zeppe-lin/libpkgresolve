// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../support/test.h"

#include <libpkgresolve/libpkgresolve.h>

#include <string>

template<typename Identity>
void qualify_identity()
{
  const std::string zero(64, '0');
  const std::string one(64, '1');
  const auto first = Identity::from_sha256(zero);
  const auto same = Identity::from_sha256(zero);
  const auto later = Identity::from_sha256(one);
  TEST_CHECK(first.hex() == zero);
  TEST_CHECK(first == same);
  TEST_CHECK(first != later);
  TEST_CHECK(first < later);
  TEST_THROWS(pkgresolve::error_code::invalid_identity,
              Identity::from_sha256("bad"));
  TEST_THROWS(pkgresolve::error_code::invalid_identity,
              Identity::from_sha256(std::string(64, 'A')));
}

int main()
{
  qualify_identity<pkgresolve::resolution_request_identity>();
  qualify_identity<pkgresolve::package_selection_identity>();
  qualify_identity<pkgresolve::requirement_edge_identity>();
  qualify_identity<pkgresolve::goal_closure_identity>();
  qualify_identity<pkgresolve::resolution_result_identity>();
  return 0;
}
