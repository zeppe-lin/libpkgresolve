// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgresolve/result.h>

#include <libpkgresolve/error.h>

#include <algorithm>
#include <tuple>
#include <utility>

namespace pkgresolve {
namespace {
bool supported(resolution_environment value) noexcept
{
  switch (value) {
  case resolution_environment::build:
  case resolution_environment::target:
    return true;
  }
  return false;
}

bool supported(selection_reason_kind value) noexcept
{
  switch (value) {
  case selection_reason_kind::direct_goal:
  case selection_reason_kind::profile_goal:
  case selection_reason_kind::runtime_requirement:
  case selection_reason_kind::build_requirement:
  case selection_reason_kind::check_requirement:
  case selection_reason_kind::lifecycle_requirement:
    return true;
  }
  return false;
}
} // namespace

selected_package::selected_package(
    resolution_environment environment,
    architecture_context selected_architectures,
    selection_authority authority,
    pkgsource::package_release release,
    pkgsource::source_snapshot_identity source_snapshot,
    package_selection_identity identity)
    : environment_(environment),
      selected_architectures_(std::move(selected_architectures)),
      authority_(std::move(authority)), release_(std::move(release)),
      source_snapshot_(std::move(source_snapshot)), identity_(std::move(identity))
{
  if (!supported(environment_))
    throw error(error_code::inconsistent_authority,
                "selected package has unsupported environment");
}
resolution_environment selected_package::environment() const noexcept
{ return environment_; }
selection_authority_kind selected_package::authority_kind() const noexcept
{
  return std::holds_alternative<pkgcatalog::catalog_candidate>(authority_)
      ? selection_authority_kind::catalog_candidate
      : selection_authority_kind::installed_package;
}
const architecture_context& selected_package::architectures() const noexcept
{ return selected_architectures_; }
const selection_authority& selected_package::authority() const noexcept
{ return authority_; }
const pkgcatalog::catalog_candidate* selected_package::candidate() const noexcept
{ return std::get_if<pkgcatalog::catalog_candidate>(&authority_); }
const pkgstate::installed_package* selected_package::installed() const noexcept
{ return std::get_if<pkgstate::installed_package>(&authority_); }
const pkgsource::package_release& selected_package::release() const noexcept
{ return release_; }
const pkgsource::package_reference& selected_package::package() const noexcept
{ return release_.package(); }
const pkgsource::source_snapshot_identity&
selected_package::source_snapshot() const noexcept { return source_snapshot_; }
const package_selection_identity& selected_package::identity() const noexcept
{ return identity_; }

requirement_witness::requirement_witness(
    requirement_authority_kind kind,
    std::optional<pkgsource::source_snapshot_identity> catalog_source,
    std::optional<pkgstate::installed_package_identity> installed_package,
    std::vector<pkgsource::requirement_origin> catalog_origins,
    std::vector<pkgstate::requirement_origin> installed_origins)
    : kind_(kind), catalog_source_(std::move(catalog_source)),
      installed_package_(std::move(installed_package)),
      catalog_origins_(std::move(catalog_origins)),
      installed_origins_(std::move(installed_origins)) {}
requirement_witness requirement_witness::catalog(
    pkgsource::source_snapshot_identity source,
    std::vector<pkgsource::requirement_origin> origins)
{
  if (origins.empty())
    throw error(error_code::inconsistent_authority,
                "catalog requirement witness has no origins");
  std::sort(origins.begin(), origins.end());
  origins.erase(std::unique(origins.begin(), origins.end()), origins.end());
  return requirement_witness(requirement_authority_kind::catalog_source,
                             std::move(source), std::nullopt,
                             std::move(origins), {});
}
requirement_witness requirement_witness::installed(
    pkgstate::installed_package_identity package,
    std::vector<pkgstate::requirement_origin> origins)
{
  if (origins.empty())
    throw error(error_code::inconsistent_authority,
                "installed requirement witness has no origins");
  std::sort(origins.begin(), origins.end());
  origins.erase(std::unique(origins.begin(), origins.end()), origins.end());
  return requirement_witness(requirement_authority_kind::installed_state,
                             std::nullopt, std::move(package), {},
                             std::move(origins));
}
requirement_authority_kind requirement_witness::kind() const noexcept
{ return kind_; }
const std::optional<pkgsource::source_snapshot_identity>&
requirement_witness::catalog_source() const noexcept { return catalog_source_; }
const std::optional<pkgstate::installed_package_identity>&
requirement_witness::installed_package() const noexcept { return installed_package_; }
const std::vector<pkgsource::requirement_origin>&
requirement_witness::catalog_origins() const noexcept { return catalog_origins_; }
const std::vector<pkgstate::requirement_origin>&
requirement_witness::installed_origins() const noexcept { return installed_origins_; }

requirement_edge::requirement_edge(
    package_selection_identity issuer,
    package_selection_identity required,
    pkgsource::requirement_scope scope,
    resolution_environment environment,
    requirement_witness witness,
    requirement_edge_identity identity)
    : issuer_(std::move(issuer)), required_(std::move(required)),
      scope_(std::move(scope)), environment_(environment),
      witness_(std::move(witness)), identity_(std::move(identity))
{
  if (!supported(environment_))
    throw error(error_code::inconsistent_authority,
                "requirement edge has unsupported environment");
}
const package_selection_identity& requirement_edge::issuer() const noexcept
{ return issuer_; }
const package_selection_identity& requirement_edge::required() const noexcept
{ return required_; }
const pkgsource::requirement_scope& requirement_edge::scope() const noexcept
{ return scope_; }
resolution_environment requirement_edge::environment() const noexcept
{ return environment_; }
const requirement_witness& requirement_edge::witness() const noexcept
{ return witness_; }
const requirement_edge_identity& requirement_edge::identity() const noexcept
{ return identity_; }

goal_member::goal_member(
    pkgsource::package_reference package,
    package_selection_identity selection,
    std::optional<pkgsource::profile_identity> profile,
    std::vector<pkgsource::profile_expansion_step> expansion)
    : package_(std::move(package)), selection_(std::move(selection)),
      profile_(std::move(profile)), expansion_(std::move(expansion)) {}
const pkgsource::package_reference& goal_member::package() const noexcept
{ return package_; }
const package_selection_identity& goal_member::selection() const noexcept
{ return selection_; }
const std::optional<pkgsource::profile_identity>&
goal_member::profile() const noexcept { return profile_; }
const std::vector<pkgsource::profile_expansion_step>&
goal_member::expansion() const noexcept { return expansion_; }
bool operator==(const goal_member& lhs, const goal_member& rhs) noexcept
{ return std::tie(lhs.package_, lhs.selection_, lhs.profile_, lhs.expansion_)
      == std::tie(rhs.package_, rhs.selection_, rhs.profile_, rhs.expansion_); }
bool operator!=(const goal_member& lhs, const goal_member& rhs) noexcept
{ return !(lhs == rhs); }
bool operator<(const goal_member& lhs, const goal_member& rhs) noexcept
{ return std::tie(lhs.package_, lhs.selection_, lhs.profile_, lhs.expansion_)
       < std::tie(rhs.package_, rhs.selection_, rhs.profile_, rhs.expansion_); }

resolved_goal::resolved_goal(
    resolution_goal goal,
    std::vector<goal_member> members,
    std::vector<package_selection_identity> selections,
    std::vector<requirement_edge_identity> edges,
    goal_closure_identity identity)
    : goal_(std::move(goal)), members_(std::move(members)),
      selections_(std::move(selections)), edges_(std::move(edges)),
      identity_(std::move(identity)) {}
const resolution_goal& resolved_goal::goal() const noexcept { return goal_; }
const std::vector<goal_member>& resolved_goal::members() const noexcept
{ return members_; }
const std::vector<package_selection_identity>&
resolved_goal::selections() const noexcept { return selections_; }
const std::vector<requirement_edge_identity>& resolved_goal::edges() const noexcept
{ return edges_; }
const goal_closure_identity& resolved_goal::identity() const noexcept
{ return identity_; }

selection_reason::selection_reason(
    package_selection_identity selection,
    selection_reason_kind kind,
    pkgsource::requirement_scope scope,
    std::optional<package_selection_identity> issuer,
    std::optional<pkgsource::profile_reference> profile,
    std::optional<pkgsource::profile_identity> profile_identity)
    : selection_(std::move(selection)), kind_(kind), scope_(std::move(scope)),
      issuer_(std::move(issuer)), profile_(std::move(profile)),
      profile_identity_(std::move(profile_identity))
{
  if (!supported(kind_))
    throw error(error_code::inconsistent_authority,
                "selection reason has unsupported kind");
}
const package_selection_identity& selection_reason::selection() const noexcept
{ return selection_; }
selection_reason_kind selection_reason::kind() const noexcept { return kind_; }
const pkgsource::requirement_scope& selection_reason::scope() const noexcept
{ return scope_; }
const std::optional<package_selection_identity>&
selection_reason::issuer() const noexcept { return issuer_; }
const std::optional<pkgsource::profile_reference>&
selection_reason::profile() const noexcept { return profile_; }
const std::optional<pkgsource::profile_identity>&
selection_reason::profile_identity() const noexcept { return profile_identity_; }
bool operator==(const selection_reason& lhs,
                const selection_reason& rhs) noexcept
{
  return std::tie(lhs.selection_, lhs.kind_, lhs.scope_, lhs.issuer_,
                  lhs.profile_, lhs.profile_identity_)
      == std::tie(rhs.selection_, rhs.kind_, rhs.scope_, rhs.issuer_,
                  rhs.profile_, rhs.profile_identity_);
}
bool operator!=(const selection_reason& lhs,
                const selection_reason& rhs) noexcept { return !(lhs == rhs); }
bool operator<(const selection_reason& lhs,
               const selection_reason& rhs) noexcept
{
  return std::tie(lhs.selection_, lhs.kind_, lhs.scope_, lhs.issuer_,
                  lhs.profile_, lhs.profile_identity_)
       < std::tie(rhs.selection_, rhs.kind_, rhs.scope_, rhs.issuer_,
                  rhs.profile_, rhs.profile_identity_);
}

resolution_result::resolution_result(
    resolution_request request,
    std::vector<selected_package> selections,
    std::vector<requirement_edge> edges,
    std::vector<resolved_goal> goals,
    std::vector<selection_reason> reasons,
    resolution_result_identity identity)
    : request_(std::move(request)), selections_(std::move(selections)),
      edges_(std::move(edges)), goals_(std::move(goals)),
      reasons_(std::move(reasons)), identity_(std::move(identity)) {}
const resolution_request& resolution_result::request() const noexcept
{ return request_; }
const std::vector<selected_package>& resolution_result::selections() const noexcept
{ return selections_; }
const std::vector<requirement_edge>& resolution_result::edges() const noexcept
{ return edges_; }
const std::vector<resolved_goal>& resolution_result::goals() const noexcept
{ return goals_; }
const std::vector<selection_reason>& resolution_result::reasons() const noexcept
{ return reasons_; }
const selected_package* resolution_result::find(
    const pkgsource::package_reference& package,
    resolution_environment environment,
    selection_authority_kind authority) const noexcept
{
  for (const selected_package& selection : selections_)
    if (selection.package() == package &&
        selection.environment() == environment &&
        selection.authority_kind() == authority)
      return &selection;
  return nullptr;
}
std::vector<selection_reason> resolution_result::reasons_for(
    const package_selection_identity& selection) const
{
  std::vector<selection_reason> result;
  for (const selection_reason& reason : reasons_)
    if (reason.selection() == selection)
      result.push_back(reason);
  return result;
}
std::vector<requirement_edge> resolution_result::edges_for_scope(
    const pkgsource::requirement_scope& scope) const
{
  std::vector<requirement_edge> result;
  for (const requirement_edge& edge : edges_)
    if (edge.scope() == scope)
      result.push_back(edge);
  return result;
}
const resolution_result_identity& resolution_result::identity() const noexcept
{ return identity_; }
} // namespace pkgresolve
