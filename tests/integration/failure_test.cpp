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
  const auto pre_remove = pkgsource::requirement_scope::lifecycle(
      pkgsource::lifecycle_action::pre_remove);

  TEST_THROWS(error_code::unknown_package,
      fixture::resolution(
          catalog, fixture::empty_state(),
          {fixture::package_goal(pkgsource::requirement_scope::run(),
                                 "missing")}));
  TEST_THROWS(error_code::unknown_profile,
      fixture::resolution(
          catalog, fixture::empty_state(),
          {fixture::profile_goal(pkgsource::requirement_scope::run(),
                                 "@missing")}));
  TEST_THROWS(error_code::missing_installed_package,
      fixture::resolution(
          catalog, fixture::empty_state(),
          {fixture::package_goal(pre_remove, "app")}));

  const auto binding = fixture::target();
  const auto installed_only = fixture::installed_package(app, binding, 70);
  const auto installed_only_state = fixture::state({installed_only}, binding);
  const auto empty_catalog = fixture::catalog(profiles, {});
  TEST_THROWS(error_code::missing_candidate,
      fixture::resolution(
          empty_catalog, installed_only_state,
          {fixture::package_goal(pkgsource::requirement_scope::build(),
                                 "app")}));

  const auto installed = fixture::installed_package(app, binding, 80,
      pkgsource::architecture_reference("x86_64"),
      pkgsource::architecture_reference("x86_64"));
  const auto state = fixture::state({installed}, binding);
  TEST_THROWS(error_code::architecture_mismatch,
      fixture::resolution(
          catalog, state,
          {fixture::package_goal(pkgsource::requirement_scope::run(), "app")},
          installed_preference::retain_compatible, "x86_64", "aarch64"));

  const auto corrupt_release = fixture::installed_package(
      app, binding, 90, pkgsource::architecture_reference("x86_64"),
      pkgsource::architecture_reference("x86_64"),
      fixture::state_identity<pkgstate::package_release_identity>(120));
  TEST_THROWS(error_code::inconsistent_authority,
      fixture::resolution(
          catalog, fixture::state({corrupt_release}, binding),
          {fixture::package_goal(pkgsource::requirement_scope::run(), "app")}));
  return 0;
}
