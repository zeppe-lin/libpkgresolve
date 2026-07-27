// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "fixture.h"
#include "test.h"

int main()
{
  using namespace pkgresolve;
  architecture_context context(
      pkgsource::architecture_reference("x86_64"),
      pkgsource::architecture_reference("aarch64"));
  TEST_CHECK(context.build().name() == "x86_64");
  TEST_CHECK(context.target().name() == "aarch64");
  TEST_CHECK(context.selected_target(resolution_environment::build).name()
             == "x86_64");
  TEST_CHECK(context.selected_target(resolution_environment::target).name()
             == "aarch64");

  resolution_policy retain;
  resolution_policy prefer(installed_preference::prefer_catalog);
  TEST_CHECK(retain.preference() == installed_preference::retain_compatible);
  TEST_CHECK(prefer.preference() == installed_preference::prefer_catalog);

  const auto goal = fixture::package_goal(
      pkgsource::requirement_scope::run(), "app", "argv[0]");
  TEST_CHECK(goal.subject().package().name() == "app");
  TEST_CHECK(goal.origin() == "argv[0]");
  TEST_THROWS(error_code::invalid_request,
              resolution_goal(pkgsource::requirement_scope::run(),
                  pkgsource::requirement_subject(
                      pkgsource::package_reference("app")), ""));
  return 0;
}
