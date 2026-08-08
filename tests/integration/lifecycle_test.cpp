// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../fixtures/resolution.h"
#include "../support/test.h"

int main()
{
  using namespace pkgresolve;
  auto profiles = fixture::profiles();
  const auto post_install = pkgsource::requirement_scope::lifecycle(
      pkgsource::lifecycle_action::post_install);
  const auto pre_remove = pkgsource::requirement_scope::lifecycle(
      pkgsource::lifecycle_action::pre_remove);
  auto app = fixture::source(profiles, "app", {
      fixture::requirement(post_install, "cache",
                           "requirements.lifecycle.post-install[0]"),
      fixture::requirement(pre_remove, "cleanup",
                           "requirements.lifecycle.pre-remove[0]"),
  });
  auto cache = fixture::source(profiles, "cache");
  auto cleanup = fixture::source(profiles, "cleanup");
  auto catalog = fixture::catalog(profiles, {app, cache, cleanup});

  const auto install = fixture::resolution(
      catalog, fixture::empty_state(),
      {fixture::package_goal(post_install, "app")});
  TEST_CHECK(install.edges_for_scope(post_install).size() == 1);
  TEST_CHECK(install.find(pkgsource::package_reference("app"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);
  TEST_CHECK(install.find(pkgsource::package_reference("cache"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);

  const auto binding = fixture::target();
  const auto installed_app = fixture::installed_package(app, binding, 70);
  const auto removal_state = fixture::state({installed_app}, binding);
  auto removal_catalog = fixture::catalog(profiles, {cleanup});
  const auto removal = fixture::resolution(
      removal_catalog, removal_state,
      {fixture::package_goal(pre_remove, "app")});
  TEST_CHECK(removal.edges_for_scope(pre_remove).size() == 1);
  TEST_CHECK(removal.find(pkgsource::package_reference("app"),
      resolution_environment::target,
      selection_authority_kind::installed_package) != nullptr);
  TEST_CHECK(removal.find(pkgsource::package_reference("cleanup"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);

  bool saw_installed_witness = false;
  for (const auto& edge : removal.edges())
    if (edge.scope() == pre_remove) {
      TEST_CHECK(edge.witness().kind()
                 == requirement_authority_kind::installed_state);
      saw_installed_witness = true;
    }
  TEST_CHECK(saw_installed_witness);
  return 0;
}
