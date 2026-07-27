// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgresolve/request.h>

#include <libpkgresolve/error.h>

#include "identity_support.h"

#include <algorithm>
#include <utility>

namespace pkgresolve {
namespace {
void write_scope(detail::identity_writer& writer,
                 const pkgsource::requirement_scope& scope)
{
  writer.text(pkgsource::to_string(scope.kind()));
  writer.boolean(scope.action().has_value());
  if (scope.action())
    writer.text(pkgsource::to_string(*scope.action()));
}
resolution_request_identity make_identity(
    const pkgcatalog::catalog_snapshot& catalog,
    const pkgstate::snapshot& installed,
    const architecture_context& architectures,
    const std::vector<resolution_goal>& goals,
    const resolution_policy& policy)
{
  detail::identity_writer writer;
  writer.text("libpkgresolve/request/1");
  writer.text(catalog.identity().hex());
  writer.text(installed.identity().string());
  writer.text(architectures.build().name());
  writer.text(architectures.target().name());
  writer.text(to_string(policy.preference()));
  writer.number(goals.size());
  for (const resolution_goal& goal : goals) {
    write_scope(writer, goal.scope());
    writer.text(pkgsource::to_string(goal.subject().kind()));
    writer.text(goal.subject().text());
  }
  return resolution_request_identity::from_sha256(writer.finish());
}
} // namespace

resolution_request::resolution_request(
    pkgcatalog::catalog_snapshot catalog,
    pkgstate::snapshot installed,
    architecture_context architectures,
    std::vector<resolution_goal> goals,
    resolution_policy policy,
    resolution_request_identity identity)
    : catalog_(std::move(catalog)), installed_(std::move(installed)),
      architectures_(std::move(architectures)), goals_(std::move(goals)),
      policy_(std::move(policy)), identity_(std::move(identity)) {}
resolution_request resolution_request::seal(
    pkgcatalog::catalog_snapshot catalog,
    pkgstate::snapshot installed,
    architecture_context architectures,
    std::vector<resolution_goal> goals,
    resolution_policy policy)
{
  if (goals.empty())
    throw error(error_code::invalid_request,
                "resolution request has no goals");
  std::sort(goals.begin(), goals.end());
  for (std::size_t index = 1; index < goals.size(); ++index)
    if (goals[index - 1] == goals[index])
      throw error(error_code::duplicate_goal,
                  "duplicate resolution goal: " + goals[index].subject().text());
  const resolution_request_identity identity =
      make_identity(catalog, installed, architectures, goals, policy);
  return resolution_request(std::move(catalog), std::move(installed),
                            std::move(architectures), std::move(goals),
                            std::move(policy), identity);
}
const pkgcatalog::catalog_snapshot& resolution_request::catalog() const noexcept
{ return catalog_; }
const pkgstate::snapshot& resolution_request::installed() const noexcept
{ return installed_; }
const architecture_context& resolution_request::architectures() const noexcept
{ return architectures_; }
const std::vector<resolution_goal>& resolution_request::goals() const noexcept
{ return goals_; }
const resolution_policy& resolution_request::policy() const noexcept
{ return policy_; }
const resolution_request_identity& resolution_request::identity() const noexcept
{ return identity_; }
} // namespace pkgresolve
