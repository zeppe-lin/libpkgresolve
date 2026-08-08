// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../fixtures/resolution.h"
#include "../support/test.h"

int main()
{
  using namespace pkgresolve;
  auto profiles = fixture::profiles();
  auto app = fixture::source(profiles, "app", {
      fixture::profile_requirement(pkgsource::requirement_scope::build(),
                                   "@toolchain", "requirements.build[0]"),
      fixture::requirement(pkgsource::requirement_scope::check(),
                           "tester", "requirements.check[0]"),
  });
  auto compiler = fixture::source(profiles, "compiler", {
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "hostlib", "requirements.run[0]"),
  });
  auto make = fixture::source(profiles, "make");
  auto hostlib = fixture::source(profiles, "hostlib");
  auto tester = fixture::source(profiles, "tester");
  auto catalog = fixture::catalog(
      profiles, {app, compiler, make, hostlib, tester});

  const auto build = fixture::resolution(
      catalog, fixture::empty_state(),
      {fixture::package_goal(pkgsource::requirement_scope::build(), "app")});
  TEST_CHECK(build.edges_for_scope(pkgsource::requirement_scope::build()).size()
             == 2);
  TEST_CHECK(build.find(pkgsource::package_reference("compiler"),
      resolution_environment::build,
      selection_authority_kind::catalog_candidate) != nullptr);
  TEST_CHECK(build.find(pkgsource::package_reference("make"),
      resolution_environment::build,
      selection_authority_kind::catalog_candidate) != nullptr);
  TEST_CHECK(build.find(pkgsource::package_reference("hostlib"),
      resolution_environment::build,
      selection_authority_kind::catalog_candidate) != nullptr);
  TEST_CHECK(build.edges_for_scope(pkgsource::requirement_scope::check()).empty());

  const auto check = fixture::resolution(
      catalog, fixture::empty_state(),
      {fixture::package_goal(pkgsource::requirement_scope::check(), "app")});
  TEST_CHECK(check.edges_for_scope(pkgsource::requirement_scope::check()).size()
             == 1);
  TEST_CHECK(check.find(pkgsource::package_reference("tester"),
      resolution_environment::build,
      selection_authority_kind::catalog_candidate) != nullptr);
  TEST_CHECK(check.find(pkgsource::package_reference("app"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);
  return 0;
}
