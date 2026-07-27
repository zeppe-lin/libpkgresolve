// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file model.h
 *  \brief Native resolver request values and policy.
 */
#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <libpkgsource/model.h>

namespace pkgresolve {

enum class resolution_environment { build, target };
enum class installed_preference { retain_compatible, prefer_catalog };
enum class selection_authority_kind { catalog_candidate, installed_package };
enum class requirement_authority_kind { catalog_source, installed_state };
enum class selection_reason_kind {
  direct_goal,
  profile_goal,
  runtime_requirement,
  build_requirement,
  check_requirement,
  lifecycle_requirement,
};

[[nodiscard]] std::string_view to_string(resolution_environment value) noexcept;
[[nodiscard]] std::string_view to_string(installed_preference value) noexcept;
[[nodiscard]] std::string_view to_string(selection_authority_kind value) noexcept;
[[nodiscard]] std::string_view to_string(requirement_authority_kind value) noexcept;
[[nodiscard]] std::string_view to_string(selection_reason_kind value) noexcept;

class architecture_context final {
public:
  architecture_context(pkgsource::architecture_reference build,
                       pkgsource::architecture_reference target);
  [[nodiscard]] const pkgsource::architecture_reference& build() const noexcept;
  [[nodiscard]] const pkgsource::architecture_reference& target() const noexcept;
  [[nodiscard]] pkgsource::architecture_reference
  selected_target(resolution_environment environment) const;
  friend bool operator==(const architecture_context& lhs,
                         const architecture_context& rhs) noexcept;
  friend bool operator!=(const architecture_context& lhs,
                         const architecture_context& rhs) noexcept;
  friend bool operator<(const architecture_context& lhs,
                        const architecture_context& rhs) noexcept;
private:
  pkgsource::architecture_reference build_;
  pkgsource::architecture_reference target_;
};

class resolution_policy final {
public:
  explicit resolution_policy(
      installed_preference preference = installed_preference::retain_compatible);
  [[nodiscard]] installed_preference preference() const noexcept;
  friend bool operator==(const resolution_policy& lhs,
                         const resolution_policy& rhs) noexcept;
  friend bool operator!=(const resolution_policy& lhs,
                         const resolution_policy& rhs) noexcept;
  friend bool operator<(const resolution_policy& lhs,
                        const resolution_policy& rhs) noexcept;
private:
  installed_preference preference_;
};

/*! \brief One explicit package or profile closure request.
 *
 * origin is diagnostic provenance and is excluded from request identity.
 */
class resolution_goal final {
public:
  resolution_goal(pkgsource::requirement_scope scope,
                  pkgsource::requirement_subject subject,
                  std::string origin);
  [[nodiscard]] const pkgsource::requirement_scope& scope() const noexcept;
  [[nodiscard]] const pkgsource::requirement_subject& subject() const noexcept;
  [[nodiscard]] const std::string& origin() const noexcept;
  friend bool operator==(const resolution_goal& lhs,
                         const resolution_goal& rhs) noexcept;
  friend bool operator!=(const resolution_goal& lhs,
                         const resolution_goal& rhs) noexcept;
  friend bool operator<(const resolution_goal& lhs,
                        const resolution_goal& rhs) noexcept;
private:
  pkgsource::requirement_scope scope_;
  pkgsource::requirement_subject subject_;
  std::string origin_;
};

} // namespace pkgresolve
