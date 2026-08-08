// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../fixtures/resolution.h"
#include "../support/test.h"

int main()
{
  using namespace pkgresolve;
  auto profiles = fixture::profiles();
  auto arm_only = fixture::source(profiles, "arm-only", {},
                                  {"x86_64"}, {"aarch64"});
  auto arm_catalog = fixture::catalog(profiles, {arm_only});
  TEST_THROWS(error_code::architecture_mismatch,
      fixture::resolution(
          arm_catalog, fixture::empty_state(),
          {fixture::package_goal(pkgsource::requirement_scope::run(),
                                 "arm-only")}));

  auto cross_tool = fixture::source(profiles, "cross-tool", {},
                                    {"x86_64"}, {"x86_64"});
  auto cross_app = fixture::source(profiles, "cross-app", {
      fixture::requirement(pkgsource::requirement_scope::build(),
                           "cross-tool", "requirements.build[0]"),
  }, {"x86_64"}, {"aarch64"});
  auto cross_catalog = fixture::catalog(profiles, {cross_app, cross_tool});
  const auto cross = fixture::resolution(
      cross_catalog, fixture::empty_state(),
      {fixture::package_goal(pkgsource::requirement_scope::build(),
                             "cross-app")},
      installed_preference::retain_compatible, "x86_64", "aarch64");
  const auto* selected_tool = cross.find(
      pkgsource::package_reference("cross-tool"),
      resolution_environment::build,
      selection_authority_kind::catalog_candidate);
  TEST_CHECK(selected_tool != nullptr);
  TEST_CHECK(selected_tool->architectures().build().name() == "x86_64");
  TEST_CHECK(selected_tool->architectures().target().name() == "x86_64");
  const auto* selected_app = cross.find(
      pkgsource::package_reference("cross-app"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate);
  TEST_CHECK(selected_app != nullptr);
  TEST_CHECK(selected_app->architectures().target().name() == "aarch64");
  return 0;
}
