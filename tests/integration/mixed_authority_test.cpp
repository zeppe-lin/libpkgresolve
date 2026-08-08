// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../fixtures/resolution.h"
#include "../support/test.h"

int main()
{
  using namespace pkgresolve;
  auto profiles = fixture::profiles();
  auto app = fixture::source(profiles, "app");
  auto catalog = fixture::catalog(profiles, {app});
  const auto binding = fixture::target();
  const auto installed_app = fixture::installed_package(app, binding, 60);
  const auto state = fixture::state({installed_app}, binding);

  const auto result = fixture::resolution(
      catalog, state,
      {
        fixture::package_goal(pkgsource::requirement_scope::run(), "app"),
        fixture::package_goal(pkgsource::requirement_scope::build(), "app"),
      });

  TEST_CHECK(result.find(pkgsource::package_reference("app"),
      resolution_environment::target,
      selection_authority_kind::installed_package) != nullptr);
  TEST_CHECK(result.find(pkgsource::package_reference("app"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);
  TEST_CHECK(result.selections().size() == 2);
  TEST_CHECK(result.goals().size() == 2);
  return 0;
}
