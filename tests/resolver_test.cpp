// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "fixture.h"
#include "test.h"

#include <algorithm>

namespace {
pkgresolve::resolution_request request(
    pkgcatalog::catalog_snapshot catalog,
    pkgstate::snapshot state,
    std::vector<pkgresolve::resolution_goal> goals,
    pkgresolve::installed_preference preference =
        pkgresolve::installed_preference::retain_compatible,
    std::string build = "x86_64",
    std::string target = "x86_64")
{
  return pkgresolve::resolution_request::seal(
      std::move(catalog), std::move(state),
      pkgresolve::architecture_context(
          pkgsource::architecture_reference(std::move(build)),
          pkgsource::architecture_reference(std::move(target))),
      std::move(goals), pkgresolve::resolution_policy(preference));
}
}

int main()
{
  using namespace pkgresolve;
  auto profiles = fixture::profiles();
  auto app = fixture::source(profiles, "app", {
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "lib", "requirements.run[0]"),
      fixture::profile_requirement(pkgsource::requirement_scope::build(),
                                   "@toolchain", "requirements.build[0]"),
      fixture::requirement(pkgsource::requirement_scope::check(),
                           "tester", "requirements.check[0]"),
      fixture::requirement(
          pkgsource::requirement_scope::lifecycle(
              pkgsource::lifecycle_action::post_install),
          "cache", "requirements.lifecycle.post-install[0]"),
      fixture::requirement(
          pkgsource::requirement_scope::lifecycle(
              pkgsource::lifecycle_action::pre_remove),
          "cleanup", "requirements.lifecycle.pre-remove[0]"),
  });
  auto lib = fixture::source(profiles, "lib", {
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "runtime", "requirements.run[0]"),
  });
  auto runtime = fixture::source(profiles, "runtime");
  auto compiler = fixture::source(profiles, "compiler", {
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "hostlib", "requirements.run[0]"),
  });
  auto make = fixture::source(profiles, "make");
  auto hostlib = fixture::source(profiles, "hostlib");
  auto tester = fixture::source(profiles, "tester");
  auto cache = fixture::source(profiles, "cache");
  auto cleanup = fixture::source(profiles, "cleanup");
  auto data = fixture::source(profiles, "data");
  auto catalog = fixture::catalog(profiles, {
      app, lib, runtime, compiler, make, hostlib, tester, cache, cleanup, data,
  });

  const auto run_result = resolve(request(
      catalog, fixture::empty_state(),
      {fixture::package_goal(pkgsource::requirement_scope::run(), "app")}));
  TEST_CHECK(run_result.selections().size() == 3);
  TEST_CHECK(run_result.edges().size() == 2);
  TEST_CHECK(run_result.edges_for_scope(
      pkgsource::requirement_scope::run()).size() == 2);
  TEST_CHECK(run_result.find(pkgsource::package_reference("app"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);

  const auto binding = fixture::target();
  const auto installed_lib = fixture::installed_package(lib, binding, 40);
  const auto installed_state = fixture::state({installed_lib}, binding);
  const auto retained = resolve(request(
      catalog, installed_state,
      {fixture::package_goal(pkgsource::requirement_scope::run(), "app")}));
  const auto* retained_lib = retained.find(pkgsource::package_reference("lib"),
      resolution_environment::target,
      selection_authority_kind::installed_package);
  TEST_CHECK(retained_lib != nullptr);
  TEST_CHECK(retained.find(pkgsource::package_reference("runtime"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);
  bool saw_installed_witness = false;
  for (const auto& edge : retained.edges())
    if (edge.witness().kind() == requirement_authority_kind::installed_state)
      saw_installed_witness = true;
  TEST_CHECK(saw_installed_witness);

  const auto preferred = resolve(request(
      catalog, installed_state,
      {fixture::package_goal(pkgsource::requirement_scope::run(), "app")},
      installed_preference::prefer_catalog));
  TEST_CHECK(preferred.find(pkgsource::package_reference("lib"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);
  TEST_CHECK(preferred.find(pkgsource::package_reference("lib"),
      resolution_environment::target,
      selection_authority_kind::installed_package) == nullptr);

  const auto build_result = resolve(request(
      catalog, fixture::empty_state(),
      {fixture::package_goal(pkgsource::requirement_scope::build(), "app")}));
  TEST_CHECK(build_result.edges_for_scope(
      pkgsource::requirement_scope::build()).size() == 2);
  TEST_CHECK(build_result.find(pkgsource::package_reference("compiler"),
      resolution_environment::build,
      selection_authority_kind::catalog_candidate) != nullptr);
  TEST_CHECK(build_result.find(pkgsource::package_reference("hostlib"),
      resolution_environment::build,
      selection_authority_kind::catalog_candidate) != nullptr);
  TEST_CHECK(build_result.edges_for_scope(
      pkgsource::requirement_scope::check()).empty());

  const auto check_result = resolve(request(
      catalog, fixture::empty_state(),
      {fixture::package_goal(pkgsource::requirement_scope::check(), "app")}));
  TEST_CHECK(check_result.edges_for_scope(
      pkgsource::requirement_scope::check()).size() == 1);
  TEST_CHECK(check_result.find(pkgsource::package_reference("tester"),
      resolution_environment::build,
      selection_authority_kind::catalog_candidate) != nullptr);

  const auto post_install = pkgsource::requirement_scope::lifecycle(
      pkgsource::lifecycle_action::post_install);
  const auto lifecycle_result = resolve(request(
      catalog, fixture::empty_state(),
      {fixture::package_goal(post_install, "app")}));
  TEST_CHECK(lifecycle_result.edges_for_scope(post_install).size() == 1);
  TEST_CHECK(lifecycle_result.find(pkgsource::package_reference("cache"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);

  const auto installed_app = fixture::installed_package(app, binding, 70);
  const auto removal_state = fixture::state({installed_app}, binding);
  const auto pre_remove = pkgsource::requirement_scope::lifecycle(
      pkgsource::lifecycle_action::pre_remove);
  const auto removal_result = resolve(request(
      catalog, removal_state,
      {fixture::package_goal(pre_remove, "app")}));
  TEST_CHECK(removal_result.find(pkgsource::package_reference("app"),
      resolution_environment::target,
      selection_authority_kind::installed_package) != nullptr);
  TEST_CHECK(removal_result.find(pkgsource::package_reference("cleanup"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);

  const auto combined_state = fixture::state({installed_app}, binding);
  const auto combined_result = resolve(request(
      catalog, combined_state,
      {
        fixture::package_goal(pkgsource::requirement_scope::run(), "app"),
        fixture::package_goal(pkgsource::requirement_scope::build(), "app"),
      }));
  TEST_CHECK(combined_result.find(pkgsource::package_reference("app"),
      resolution_environment::target,
      selection_authority_kind::installed_package) != nullptr);
  TEST_CHECK(combined_result.find(pkgsource::package_reference("app"),
      resolution_environment::target,
      selection_authority_kind::catalog_candidate) != nullptr);

  const auto profile_result = resolve(request(
      catalog, fixture::empty_state(),
      {fixture::profile_goal(pkgsource::requirement_scope::run(), "@desktop")}));
  TEST_CHECK(profile_result.goals().front().members().size() == 2);
  TEST_CHECK(std::all_of(profile_result.goals().front().members().begin(),
                         profile_result.goals().front().members().end(),
      [](const goal_member& member) { return member.profile().has_value(); }));
  bool profile_reason = false;
  for (const auto& reason : profile_result.reasons())
    if (reason.kind() == selection_reason_kind::profile_goal)
      profile_reason = true;
  TEST_CHECK(profile_reason);

  const auto reordered = resolve(request(
      catalog, fixture::empty_state(),
      {
        fixture::profile_goal(pkgsource::requirement_scope::run(), "@desktop"),
        fixture::package_goal(pkgsource::requirement_scope::build(), "app"),
      }));
  const auto reordered_again = resolve(request(
      catalog, fixture::empty_state(),
      {
        fixture::package_goal(pkgsource::requirement_scope::build(), "app"),
        fixture::profile_goal(pkgsource::requirement_scope::run(), "@desktop"),
      }));
  TEST_CHECK(reordered.identity() == reordered_again.identity());

  TEST_THROWS(error_code::unknown_package,
      resolve(request(catalog, fixture::empty_state(),
          {fixture::package_goal(pkgsource::requirement_scope::run(),
                                 "missing")})));
  TEST_THROWS(error_code::unknown_profile,
      resolve(request(catalog, fixture::empty_state(),
          {fixture::profile_goal(pkgsource::requirement_scope::run(),
                                 "@missing")})));
  TEST_THROWS(error_code::missing_installed_package,
      resolve(request(catalog, fixture::empty_state(),
          {fixture::package_goal(pre_remove, "app")})));

  auto arm_only = fixture::source(profiles, "arm-only", {},
                                  {"x86_64"}, {"aarch64"});
  auto arch_catalog = fixture::catalog(profiles, {arm_only});
  TEST_THROWS(error_code::architecture_mismatch,
      resolve(request(arch_catalog, fixture::empty_state(),
          {fixture::package_goal(pkgsource::requirement_scope::run(),
                                 "arm-only")})));

  auto cross_tool = fixture::source(profiles, "cross-tool", {},
                                    {"x86_64"}, {"x86_64"});
  auto cross_app = fixture::source(profiles, "cross-app", {
      fixture::requirement(pkgsource::requirement_scope::build(),
                           "cross-tool", "requirements.build[0]"),
  }, {"x86_64"}, {"aarch64"});
  auto cross_catalog = fixture::catalog(profiles, {cross_app, cross_tool});
  const auto cross_result = resolve(request(
      cross_catalog, fixture::empty_state(),
      {fixture::package_goal(pkgsource::requirement_scope::build(),
                             "cross-app")},
      installed_preference::retain_compatible, "x86_64", "aarch64"));
  const auto* selected_tool = cross_result.find(
      pkgsource::package_reference("cross-tool"),
      resolution_environment::build,
      selection_authority_kind::catalog_candidate);
  TEST_CHECK(selected_tool != nullptr);
  TEST_CHECK(selected_tool->architectures().target().name() == "x86_64");

  auto cycle_a = fixture::source(profiles, "cycle-a", {
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "cycle-b", "requirements.run[0]"),
  });
  auto cycle_b = fixture::source(profiles, "cycle-b", {
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "cycle-a", "requirements.run[0]"),
  });
  auto cycle_catalog = fixture::catalog(profiles, {cycle_a, cycle_b});
  const auto cycle_result = resolve(request(
      cycle_catalog, fixture::empty_state(),
      {fixture::package_goal(pkgsource::requirement_scope::run(),
                             "cycle-a")}));
  TEST_CHECK(cycle_result.selections().size() == 2);
  TEST_CHECK(cycle_result.edges().size() == 2);
  return 0;
}
