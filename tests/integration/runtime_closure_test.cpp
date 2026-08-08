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

  const auto result = fixture::resolution(
      catalog, fixture::empty_state(),
      {fixture::package_goal(pkgsource::requirement_scope::run(), "app")});

  TEST_CHECK(result.selections().size() == 3);
  TEST_CHECK(result.edges().size() == 2);
  TEST_CHECK(result.goals().size() == 1);
  TEST_CHECK(result.goals().front().members().size() == 1);
  TEST_CHECK(result.goals().front().selections().size() == 3);
  TEST_CHECK(result.goals().front().edges().size() == 2);
  TEST_CHECK(result.edges_for_scope(pkgsource::requirement_scope::run()).size()
             == 2);

  const auto* selected_app = result.find(
      pkgsource::package_reference("app"), resolution_environment::target,
      selection_authority_kind::catalog_candidate);
  const auto* selected_lib = result.find(
      pkgsource::package_reference("lib"), resolution_environment::target,
      selection_authority_kind::catalog_candidate);
  TEST_CHECK(selected_app != nullptr);
  TEST_CHECK(selected_lib != nullptr);
  TEST_CHECK(result.reasons_for(selected_app->identity()).size() == 1);
  TEST_CHECK(result.reasons_for(selected_lib->identity()).size() == 1);

  bool saw_app_lib = false;
  bool saw_lib_runtime = false;
  for (const auto& edge : result.edges()) {
    TEST_CHECK(edge.witness().kind()
               == requirement_authority_kind::catalog_source);
    const auto* issuer = test_support::selection_by_identity(result,
                                                              edge.issuer());
    TEST_CHECK(issuer != nullptr);
    TEST_CHECK(edge.witness().catalog_source().has_value());
    TEST_CHECK(*edge.witness().catalog_source() == issuer->source_snapshot());
    TEST_CHECK(!edge.witness().catalog_origins().empty());
    if (issuer->package().name() == "app")
      saw_app_lib = true;
    if (issuer->package().name() == "lib")
      saw_lib_runtime = true;
  }
  TEST_CHECK(saw_app_lib);
  TEST_CHECK(saw_lib_runtime);
  return 0;
}
