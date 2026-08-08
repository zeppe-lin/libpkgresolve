// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include <libpkgresolve/libpkgresolve.h>

namespace test_support {

inline const pkgresolve::selected_package* selection_by_identity(
    const pkgresolve::resolution_result& result,
    const pkgresolve::package_selection_identity& identity)
{
  for (const auto& selection : result.selections())
    if (selection.identity() == identity)
      return &selection;
  return nullptr;
}

inline const pkgresolve::requirement_edge* edge_by_identity(
    const pkgresolve::resolution_result& result,
    const pkgresolve::requirement_edge_identity& identity)
{
  for (const auto& edge : result.edges())
    if (edge.identity() == identity)
      return &edge;
  return nullptr;
}

inline std::set<std::string> selection_id_set(
    const pkgresolve::resolution_result& result)
{
  std::set<std::string> ids;
  for (const auto& selection : result.selections())
    ids.insert(selection.identity().hex());
  return ids;
}

inline std::set<std::string> edge_id_set(
    const pkgresolve::resolution_result& result)
{
  std::set<std::string> ids;
  for (const auto& edge : result.edges())
    ids.insert(edge.identity().hex());
  return ids;
}

template<typename Values, typename IdentityOf>
bool strictly_identity_sorted(const Values& values, IdentityOf identity_of)
{
  return std::adjacent_find(
      values.begin(), values.end(),
      [&](const auto& lhs, const auto& rhs) {
        return !(identity_of(lhs) < identity_of(rhs));
      }) == values.end();
}

} // namespace test_support
