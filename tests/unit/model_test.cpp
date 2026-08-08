// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../support/test.h"

#include <libpkgresolve/libpkgresolve.h>

int main()
{
  using namespace pkgresolve;

  TEST_CHECK(to_string(resolution_environment::build) == "build");
  TEST_CHECK(to_string(resolution_environment::target) == "target");
  TEST_CHECK(to_string(installed_preference::retain_compatible)
             == "retain-compatible");
  TEST_CHECK(to_string(installed_preference::prefer_catalog)
             == "prefer-catalog");
  TEST_CHECK(to_string(selection_authority_kind::catalog_candidate)
             == "catalog-candidate");
  TEST_CHECK(to_string(selection_authority_kind::installed_package)
             == "installed-package");
  TEST_CHECK(to_string(requirement_authority_kind::catalog_source)
             == "catalog-source");
  TEST_CHECK(to_string(requirement_authority_kind::installed_state)
             == "installed-state");
  TEST_CHECK(to_string(selection_reason_kind::direct_goal) == "direct-goal");
  TEST_CHECK(to_string(selection_reason_kind::profile_goal) == "profile-goal");
  TEST_CHECK(to_string(selection_reason_kind::runtime_requirement)
             == "runtime-requirement");
  TEST_CHECK(to_string(selection_reason_kind::build_requirement)
             == "build-requirement");
  TEST_CHECK(to_string(selection_reason_kind::check_requirement)
             == "check-requirement");
  TEST_CHECK(to_string(selection_reason_kind::lifecycle_requirement)
             == "lifecycle-requirement");

  architecture_context context(
      pkgsource::architecture_reference("x86_64"),
      pkgsource::architecture_reference("aarch64"));
  architecture_context same(
      pkgsource::architecture_reference("x86_64"),
      pkgsource::architecture_reference("aarch64"));
  architecture_context later(
      pkgsource::architecture_reference("x86_64"),
      pkgsource::architecture_reference("riscv64"));
  TEST_CHECK(context == same);
  TEST_CHECK(context != later);
  TEST_CHECK(context < later);
  TEST_CHECK(context.selected_target(resolution_environment::build).name()
             == "x86_64");
  TEST_CHECK(context.selected_target(resolution_environment::target).name()
             == "aarch64");

  resolution_policy retain;
  resolution_policy prefer(installed_preference::prefer_catalog);
  TEST_CHECK(retain.preference() == installed_preference::retain_compatible);
  TEST_CHECK(retain != prefer);
  TEST_CHECK(retain < prefer);

  const resolution_goal first(
      pkgsource::requirement_scope::run(),
      pkgsource::requirement_subject(pkgsource::package_reference("app")),
      "argv[0]");
  const resolution_goal same_semantics(
      pkgsource::requirement_scope::run(),
      pkgsource::requirement_subject(pkgsource::package_reference("app")),
      "different-origin");
  TEST_CHECK(first == same_semantics);
  TEST_CHECK(first.origin() == "argv[0]");
  TEST_THROWS(error_code::invalid_request,
      resolution_goal(pkgsource::requirement_scope::run(),
          pkgsource::requirement_subject(pkgsource::package_reference("app")),
          ""));
  return 0;
}
