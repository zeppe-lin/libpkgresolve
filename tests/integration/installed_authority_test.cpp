// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../fixtures/resolution.h"
#include "../support/result_query.h"
#include "../support/test.h"

int main()
{
  using namespace pkgresolve;
  auto profiles = fixture::profiles();
  auto app = fixture::source(profiles, "app", {
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "lib", "requirements.run[0]"),
  });
  auto lib = fixture::source(profiles, "lib", {
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "runtime", "requirements.run[0]"),
  });
  auto runtime = fixture::source(profiles, "runtime");
  auto catalog = fixture::catalog(profiles, {app, lib, runtime});
  const auto binding = fixture::target();
  const auto installed_lib = fixture::installed_package(lib, binding, 40);
  const auto installed_state = fixture::state({installed_lib}, binding);

  const auto retained = fixture::resolution(
      catalog, installed_state,
      {fixture::package_goal(pkgsource::requirement_scope::run(), "app")});
  const auto* selected_lib = retained.find(
      pkgsource::package_reference("lib"), resolution_environment::target,
      selection_authority_kind::installed_package);
  TEST_CHECK(selected_lib != nullptr);
  TEST_CHECK(selected_lib->installed() != nullptr);
  TEST_CHECK(selected_lib->candidate() == nullptr);
  TEST_CHECK(retained.find(pkgsource::package_reference("runtime"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);

  bool saw_installed_witness = false;
  for (const auto& edge : retained.edges()) {
    const auto* issuer = test_support::selection_by_identity(retained,
                                                              edge.issuer());
    TEST_CHECK(issuer != nullptr);
    if (edge.witness().kind() != requirement_authority_kind::installed_state)
      continue;
    saw_installed_witness = true;
    TEST_CHECK(issuer->installed() != nullptr);
    TEST_CHECK(edge.witness().installed_package().has_value());
    TEST_CHECK(*edge.witness().installed_package()
               == issuer->installed()->identity());
    TEST_CHECK(!edge.witness().installed_origins().empty());
  }
  TEST_CHECK(saw_installed_witness);

  const auto preferred = fixture::resolution(
      catalog, installed_state,
      {fixture::package_goal(pkgsource::requirement_scope::run(), "app")},
      installed_preference::prefer_catalog);
  TEST_CHECK(preferred.find(pkgsource::package_reference("lib"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);
  TEST_CHECK(preferred.find(pkgsource::package_reference("lib"),
      resolution_environment::target,
      selection_authority_kind::installed_package) == nullptr);
  return 0;
}
