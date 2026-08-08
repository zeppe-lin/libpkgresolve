// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../fixtures/resolution.h"
#include "../support/test.h"

int main()
{
  auto profiles = fixture::profiles();
  auto a = fixture::source(profiles, "cycle-a", {
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "cycle-b", "requirements.run[0]"),
  });
  auto b = fixture::source(profiles, "cycle-b", {
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "cycle-a", "requirements.run[0]"),
  });
  auto catalog = fixture::catalog(profiles, {a, b});
  const auto result = fixture::resolution(
      catalog, fixture::empty_state(),
      {fixture::package_goal(pkgsource::requirement_scope::run(), "cycle-a")});
  TEST_CHECK(result.selections().size() == 2);
  TEST_CHECK(result.edges().size() == 2);
  TEST_CHECK(result.goals().front().selections().size() == 2);
  TEST_CHECK(result.goals().front().edges().size() == 2);
  return 0;
}
