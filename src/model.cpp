// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgresolve/model.h>

#include <libpkgresolve/error.h>

#include <tuple>
#include <utility>

namespace pkgresolve {
std::string_view to_string(resolution_environment value) noexcept
{
  switch (value) {
  case resolution_environment::build: return "build";
  case resolution_environment::target: return "target";
  }
  return "unknown";
}
std::string_view to_string(installed_preference value) noexcept
{
  switch (value) {
  case installed_preference::retain_compatible: return "retain-compatible";
  case installed_preference::prefer_catalog: return "prefer-catalog";
  }
  return "unknown";
}
std::string_view to_string(selection_authority_kind value) noexcept
{
  switch (value) {
  case selection_authority_kind::catalog_candidate: return "catalog-candidate";
  case selection_authority_kind::installed_package: return "installed-package";
  }
  return "unknown";
}
std::string_view to_string(requirement_authority_kind value) noexcept
{
  switch (value) {
  case requirement_authority_kind::catalog_source: return "catalog-source";
  case requirement_authority_kind::installed_state: return "installed-state";
  }
  return "unknown";
}
std::string_view to_string(selection_reason_kind value) noexcept
{
  switch (value) {
  case selection_reason_kind::direct_goal: return "direct-goal";
  case selection_reason_kind::profile_goal: return "profile-goal";
  case selection_reason_kind::runtime_requirement: return "runtime-requirement";
  case selection_reason_kind::build_requirement: return "build-requirement";
  case selection_reason_kind::check_requirement: return "check-requirement";
  case selection_reason_kind::lifecycle_requirement: return "lifecycle-requirement";
  }
  return "unknown";
}

architecture_context::architecture_context(
    pkgsource::architecture_reference build,
    pkgsource::architecture_reference target)
    : build_(std::move(build)), target_(std::move(target)) {}
const pkgsource::architecture_reference& architecture_context::build() const noexcept
{ return build_; }
const pkgsource::architecture_reference& architecture_context::target() const noexcept
{ return target_; }
pkgsource::architecture_reference architecture_context::selected_target(
    resolution_environment environment) const
{
  switch (environment) {
  case resolution_environment::build: return build_;
  case resolution_environment::target: return target_;
  }
  throw error(error_code::invalid_request,
              "unsupported resolution environment");
}
bool operator==(const architecture_context& lhs,
                const architecture_context& rhs) noexcept
{ return std::tie(lhs.build_, lhs.target_) == std::tie(rhs.build_, rhs.target_); }
bool operator!=(const architecture_context& lhs,
                const architecture_context& rhs) noexcept { return !(lhs == rhs); }
bool operator<(const architecture_context& lhs,
               const architecture_context& rhs) noexcept
{ return std::tie(lhs.build_, lhs.target_) < std::tie(rhs.build_, rhs.target_); }

resolution_policy::resolution_policy(installed_preference preference)
    : preference_(preference)
{
  switch (preference_) {
  case installed_preference::retain_compatible:
  case installed_preference::prefer_catalog:
    return;
  }
  throw error(error_code::invalid_request,
              "unsupported installed preference");
}
installed_preference resolution_policy::preference() const noexcept
{ return preference_; }
bool operator==(const resolution_policy& lhs,
                const resolution_policy& rhs) noexcept
{ return lhs.preference_ == rhs.preference_; }
bool operator!=(const resolution_policy& lhs,
                const resolution_policy& rhs) noexcept { return !(lhs == rhs); }
bool operator<(const resolution_policy& lhs,
               const resolution_policy& rhs) noexcept
{ return lhs.preference_ < rhs.preference_; }

resolution_goal::resolution_goal(pkgsource::requirement_scope scope,
                                 pkgsource::requirement_subject subject,
                                 std::string origin)
    : scope_(std::move(scope)), subject_(std::move(subject)),
      origin_(std::move(origin))
{
  if (origin_.empty())
    throw error(error_code::invalid_request,
                "resolution goal has empty diagnostic origin");
}
const pkgsource::requirement_scope& resolution_goal::scope() const noexcept
{ return scope_; }
const pkgsource::requirement_subject& resolution_goal::subject() const noexcept
{ return subject_; }
const std::string& resolution_goal::origin() const noexcept { return origin_; }
bool operator==(const resolution_goal& lhs,
                const resolution_goal& rhs) noexcept
{ return lhs.scope_ == rhs.scope_ && lhs.subject_ == rhs.subject_; }
bool operator!=(const resolution_goal& lhs,
                const resolution_goal& rhs) noexcept { return !(lhs == rhs); }
bool operator<(const resolution_goal& lhs,
               const resolution_goal& rhs) noexcept
{ return std::tie(lhs.scope_, lhs.subject_) < std::tie(rhs.scope_, rhs.subject_); }
} // namespace pkgresolve
