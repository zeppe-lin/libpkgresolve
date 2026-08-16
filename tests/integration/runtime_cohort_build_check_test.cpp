// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../fixtures/resolution.h"
#include "../support/test.h"

#include <algorithm>
#include <string_view>

namespace {

const pkgresolve::selected_package& require_selection(
    const pkgresolve::resolution_result& result,
    std::string_view package,
    pkgresolve::resolution_environment environment)
{
  const auto* selected = result.find(
      pkgsource::package_reference(std::string(package)), environment,
      pkgresolve::selection_authority_kind::catalog_candidate);
  TEST_CHECK(selected != nullptr);
  return *selected;
}

bool has_edge(const pkgresolve::resolution_result& result,
              const pkgresolve::selected_package& issuer,
              const pkgresolve::selected_package& required,
              pkgsource::requirement_scope_kind scope)
{
  return std::any_of(
      result.edges().begin(), result.edges().end(), [&](const auto& edge) {
        return edge.issuer() == issuer.identity() &&
               edge.required() == required.identity() &&
               edge.scope().kind() == scope;
      });
}

std::size_t selection_count(const pkgresolve::resolution_result& result,
                            std::string_view package,
                            pkgresolve::resolution_environment environment)
{
  return static_cast<std::size_t>(std::count_if(
      result.selections().begin(), result.selections().end(),
      [&](const auto& selection) {
        return selection.package().name() == package &&
               selection.environment() == environment;
      }));
}

} // namespace

int main()
{
  using namespace pkgresolve;

  auto profiles = fixture::profiles();
  auto headers = fixture::source(profiles, "cohort-headers");
  auto libc_bootstrap = fixture::source(profiles, "cohort-libc-bootstrap", {
      fixture::requirement(pkgsource::requirement_scope::build(),
                           "cohort-headers", "requirements.build[0]"),
  });
  auto libc = fixture::source(profiles, "cohort-libc", {
      fixture::requirement(pkgsource::requirement_scope::build(),
                           "cohort-headers", "requirements.build[0]"),
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "cohort-libgcc", "requirements.run[0]"),
  });
  auto libgcc = fixture::source(profiles, "cohort-libgcc", {
      fixture::requirement(pkgsource::requirement_scope::build(),
                           "cohort-libc-bootstrap", "requirements.build[0]"),
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "cohort-libc", "requirements.run[0]"),
  });
  auto filesystem = fixture::source(profiles, "cohort-filesystem");
  auto checker = fixture::source(profiles, "cohort-checker", {
      fixture::requirement(pkgsource::requirement_scope::build(),
                           "cohort-headers", "requirements.build[0]"),
  });
  auto probe = fixture::source(profiles, "cohort-probe", {
      fixture::requirement(pkgsource::requirement_scope::build(),
                           "cohort-filesystem", "requirements.build[0]"),
      fixture::requirement(pkgsource::requirement_scope::build(),
                           "cohort-libc", "requirements.build[1]"),
      fixture::requirement(pkgsource::requirement_scope::build(),
                           "cohort-libgcc", "requirements.build[2]"),
      fixture::requirement(pkgsource::requirement_scope::check(),
                           "cohort-filesystem", "requirements.check[0]"),
      fixture::requirement(pkgsource::requirement_scope::check(),
                           "cohort-libc", "requirements.check[1]"),
      fixture::requirement(pkgsource::requirement_scope::check(),
                           "cohort-libgcc", "requirements.check[2]"),
      fixture::requirement(pkgsource::requirement_scope::check(),
                           "cohort-checker", "requirements.check[3]"),
  });

  auto catalog = fixture::catalog(
      profiles,
      {headers, libc_bootstrap, libc, libgcc, filesystem, checker, probe});
  const auto result = fixture::resolution(
      catalog, fixture::empty_state(),
      {fixture::package_goal(pkgsource::requirement_scope::check(),
                             "cohort-probe")});

  TEST_CHECK(result.selections().size() == 7);
  TEST_CHECK(result.edges_for_scope(pkgsource::requirement_scope::build()).size()
             == 7);
  TEST_CHECK(result.edges_for_scope(pkgsource::requirement_scope::check()).size()
             == 4);
  TEST_CHECK(result.edges_for_scope(pkgsource::requirement_scope::run()).size()
             == 2);

  const auto& target_probe = require_selection(
      result, "cohort-probe", resolution_environment::target);
  const auto& build_headers = require_selection(
      result, "cohort-headers", resolution_environment::build);
  const auto& build_bootstrap = require_selection(
      result, "cohort-libc-bootstrap", resolution_environment::build);
  const auto& build_libc = require_selection(
      result, "cohort-libc", resolution_environment::build);
  const auto& build_libgcc = require_selection(
      result, "cohort-libgcc", resolution_environment::build);
  const auto& build_filesystem = require_selection(
      result, "cohort-filesystem", resolution_environment::build);
  const auto& build_checker = require_selection(
      result, "cohort-checker", resolution_environment::build);

  TEST_CHECK(selection_count(result, "cohort-libc", resolution_environment::build)
             == 1);
  TEST_CHECK(selection_count(result, "cohort-libgcc", resolution_environment::build)
             == 1);
  TEST_CHECK(selection_count(result, "cohort-libc", resolution_environment::target)
             == 0);
  TEST_CHECK(selection_count(result, "cohort-libgcc", resolution_environment::target)
             == 0);

  TEST_CHECK(has_edge(result, target_probe, build_filesystem,
                      pkgsource::requirement_scope_kind::build));
  TEST_CHECK(has_edge(result, target_probe, build_libc,
                      pkgsource::requirement_scope_kind::build));
  TEST_CHECK(has_edge(result, target_probe, build_libgcc,
                      pkgsource::requirement_scope_kind::build));
  TEST_CHECK(has_edge(result, target_probe, build_filesystem,
                      pkgsource::requirement_scope_kind::check));
  TEST_CHECK(has_edge(result, target_probe, build_libc,
                      pkgsource::requirement_scope_kind::check));
  TEST_CHECK(has_edge(result, target_probe, build_libgcc,
                      pkgsource::requirement_scope_kind::check));
  TEST_CHECK(has_edge(result, target_probe, build_checker,
                      pkgsource::requirement_scope_kind::check));

  TEST_CHECK(has_edge(result, build_libc, build_headers,
                      pkgsource::requirement_scope_kind::build));
  TEST_CHECK(has_edge(result, build_libgcc, build_bootstrap,
                      pkgsource::requirement_scope_kind::build));
  TEST_CHECK(has_edge(result, build_bootstrap, build_headers,
                      pkgsource::requirement_scope_kind::build));
  TEST_CHECK(has_edge(result, build_checker, build_headers,
                      pkgsource::requirement_scope_kind::build));
  TEST_CHECK(has_edge(result, build_libc, build_libgcc,
                      pkgsource::requirement_scope_kind::run));
  TEST_CHECK(has_edge(result, build_libgcc, build_libc,
                      pkgsource::requirement_scope_kind::run));

  // The reciprocal runtime closure must not be rewritten as extra direct
  // build/check authority from the probe. Those direct edges are exactly the
  // declarations above; the cycle remains its own run-scope evidence.
  TEST_CHECK(std::none_of(
      result.edges().begin(), result.edges().end(), [&](const auto& edge) {
        return edge.issuer() == target_probe.identity() &&
               edge.required() == build_headers.identity();
      }));

  return 0;
}
