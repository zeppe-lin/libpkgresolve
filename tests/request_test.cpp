// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "fixture.h"
#include "test.h"

int main()
{
  auto profile_catalog = fixture::profiles();
  auto snapshot = fixture::catalog(profile_catalog, {
      fixture::source(profile_catalog, "app"),
      fixture::source(profile_catalog, "data"),
  });
  auto state = fixture::empty_state();
  auto first = pkgresolve::resolution_request::seal(
      snapshot, state,
      pkgresolve::architecture_context(
          pkgsource::architecture_reference("x86_64"),
          pkgsource::architecture_reference("x86_64")),
      {
        fixture::profile_goal(pkgsource::requirement_scope::run(),
                              "@desktop", "one"),
        fixture::package_goal(pkgsource::requirement_scope::build(),
                              "app", "two"),
      });
  auto second = pkgresolve::resolution_request::seal(
      snapshot, state,
      pkgresolve::architecture_context(
          pkgsource::architecture_reference("x86_64"),
          pkgsource::architecture_reference("x86_64")),
      {
        fixture::package_goal(pkgsource::requirement_scope::build(),
                              "app", "different-origin"),
        fixture::profile_goal(pkgsource::requirement_scope::run(),
                              "@desktop", "other-origin"),
      });
  TEST_CHECK(first.identity() == second.identity());
  TEST_CHECK(first.goals().size() == 2);
  TEST_THROWS(pkgresolve::error_code::duplicate_goal,
      pkgresolve::resolution_request::seal(
          snapshot, state,
          pkgresolve::architecture_context(
              pkgsource::architecture_reference("x86_64"),
              pkgsource::architecture_reference("x86_64")),
          {
            fixture::package_goal(pkgsource::requirement_scope::run(), "app"),
            fixture::package_goal(pkgsource::requirement_scope::run(), "app", "two"),
          }));
  TEST_THROWS(pkgresolve::error_code::invalid_request,
      pkgresolve::resolution_request::seal(
          snapshot, state,
          pkgresolve::architecture_context(
              pkgsource::architecture_reference("x86_64"),
              pkgsource::architecture_reference("x86_64")), {}));
  return 0;
}
