// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../fixtures/resolution.h"
#include "../support/test.h"

#include <algorithm>

int main()
{
  using namespace pkgresolve;
  auto profiles = fixture::profiles();
  auto app = fixture::source(profiles, "app");
  auto data = fixture::source(profiles, "data");
  auto catalog = fixture::catalog(profiles, {app, data});

  const auto result = fixture::resolution(
      catalog, fixture::empty_state(),
      {fixture::profile_goal(pkgsource::requirement_scope::run(), "@desktop")});
  TEST_CHECK(result.goals().size() == 1);
  const auto& goal = result.goals().front();
  TEST_CHECK(goal.members().size() == 2);
  TEST_CHECK(goal.selections().size() == 2);
  TEST_CHECK(goal.edges().empty());
  TEST_CHECK(std::all_of(goal.members().begin(), goal.members().end(),
      [](const goal_member& member) {
        return member.profile().has_value() && !member.expansion().empty();
      }));

  const auto& sealed_profile = profiles.require(pkgsource::profile_reference("@desktop"));
  for (const auto& member : goal.members())
    TEST_CHECK(*member.profile() == sealed_profile.identity());

  std::size_t profile_reasons = 0;
  for (const auto& reason : result.reasons())
    if (reason.kind() == selection_reason_kind::profile_goal) {
      ++profile_reasons;
      TEST_CHECK(reason.profile().has_value());
      TEST_CHECK(reason.profile_identity().has_value());
      TEST_CHECK(*reason.profile_identity() == sealed_profile.identity());
    }
  TEST_CHECK(profile_reasons == 2);
  return 0;
}
