// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../fixtures/resolution.h"
#include "../support/test.h"

int main()
{
  auto profiles = fixture::profiles();
  auto catalog = fixture::catalog(profiles, {
      fixture::source(profiles, "app"),
      fixture::source(profiles, "data"),
  });
  auto state = fixture::empty_state();

  const auto first = fixture::request(
      catalog, state,
      {
        fixture::profile_goal(pkgsource::requirement_scope::run(),
                              "@desktop", "one"),
        fixture::package_goal(pkgsource::requirement_scope::build(),
                              "app", "two"),
      });
  const auto second = fixture::request(
      catalog, state,
      {
        fixture::package_goal(pkgsource::requirement_scope::build(),
                              "app", "different-origin"),
        fixture::profile_goal(pkgsource::requirement_scope::run(),
                              "@desktop", "other-origin"),
      });
  TEST_CHECK(first.identity() == second.identity());
  TEST_CHECK(first.goals().size() == 2);
  TEST_CHECK(first.goals()[0] < first.goals()[1]);
  TEST_CHECK(first.catalog().identity() == catalog.identity());
  TEST_CHECK(first.installed().identity() == state.identity());

  TEST_THROWS(pkgresolve::error_code::duplicate_goal,
      fixture::request(
          catalog, state,
          {
            fixture::package_goal(pkgsource::requirement_scope::run(), "app"),
            fixture::package_goal(pkgsource::requirement_scope::run(), "app",
                                  "different-origin"),
          }));
  TEST_THROWS(pkgresolve::error_code::invalid_request,
      fixture::request(catalog, state, {}));
  return 0;
}
