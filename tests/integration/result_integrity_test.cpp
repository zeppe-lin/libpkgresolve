// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../fixtures/resolution.h"
#include "../support/result_query.h"
#include "../support/test.h"

#include <algorithm>
#include <set>
#include <string>

int main()
{
  using namespace pkgresolve;
  auto profiles = fixture::profiles();
  auto app = fixture::source(profiles, "app", {
      fixture::requirement(pkgsource::requirement_scope::run(),
                           "runtime", "requirements.run[0]"),
      fixture::requirement(pkgsource::requirement_scope::build(),
                           "compiler", "requirements.build[0]"),
  });
  auto runtime = fixture::source(profiles, "runtime");
  auto compiler = fixture::source(profiles, "compiler");
  auto data = fixture::source(profiles, "data");
  auto catalog = fixture::catalog(profiles, {app, runtime, compiler, data});

  const auto first = fixture::resolution(
      catalog, fixture::empty_state(),
      {
        fixture::profile_goal(pkgsource::requirement_scope::run(), "@desktop"),
        fixture::package_goal(pkgsource::requirement_scope::build(), "app"),
      });
  const auto reordered = fixture::resolution(
      catalog, fixture::empty_state(),
      {
        fixture::package_goal(pkgsource::requirement_scope::build(), "app",
                              "other-origin"),
        fixture::profile_goal(pkgsource::requirement_scope::run(), "@desktop",
                              "different-origin"),
      });
  TEST_CHECK(first.identity() == reordered.identity());

  TEST_CHECK(test_support::strictly_identity_sorted(
      first.selections(), [](const selected_package& value) {
        return value.identity();
      }));
  TEST_CHECK(test_support::strictly_identity_sorted(
      first.edges(), [](const requirement_edge& value) {
        return value.identity();
      }));
  TEST_CHECK(std::is_sorted(first.reasons().begin(), first.reasons().end()));
  TEST_CHECK(std::adjacent_find(first.reasons().begin(), first.reasons().end())
             == first.reasons().end());

  const auto selections = test_support::selection_id_set(first);
  const auto edges = test_support::edge_id_set(first);
  TEST_CHECK(selections.size() == first.selections().size());
  TEST_CHECK(edges.size() == first.edges().size());

  for (const auto& edge : first.edges()) {
    TEST_CHECK(selections.count(edge.issuer().hex()) == 1);
    TEST_CHECK(selections.count(edge.required().hex()) == 1);
  }
  for (const auto& reason : first.reasons()) {
    TEST_CHECK(selections.count(reason.selection().hex()) == 1);
    if (reason.issuer())
      TEST_CHECK(selections.count(reason.issuer()->hex()) == 1);
  }
  for (const auto& goal : first.goals()) {
    for (const auto& member : goal.members()) {
      TEST_CHECK(selections.count(member.selection().hex()) == 1);
      TEST_CHECK(std::find(goal.selections().begin(), goal.selections().end(),
                           member.selection()) != goal.selections().end());
    }
    TEST_CHECK(std::is_sorted(goal.selections().begin(), goal.selections().end()));
    TEST_CHECK(std::adjacent_find(goal.selections().begin(), goal.selections().end())
               == goal.selections().end());
    TEST_CHECK(std::is_sorted(goal.edges().begin(), goal.edges().end()));
    TEST_CHECK(std::adjacent_find(goal.edges().begin(), goal.edges().end())
               == goal.edges().end());
    for (const auto& selection : goal.selections())
      TEST_CHECK(selections.count(selection.hex()) == 1);
    for (const auto& edge : goal.edges())
      TEST_CHECK(edges.count(edge.hex()) == 1);
  }

  TEST_CHECK(first.goals().size() == first.request().goals().size());
  for (std::size_t index = 0; index < first.goals().size(); ++index)
    TEST_CHECK(first.goals()[index].goal() == first.request().goals()[index]);
  return 0;
}
