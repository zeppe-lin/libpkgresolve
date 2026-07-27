// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file result.h
 *  \brief Immutable selected packages, witnesses, and typed closures.
 */
#pragma once

#include <optional>
#include <variant>
#include <vector>

#include <libpkgcatalog/catalog.h>
#include <libpkgstate/installed_package.h>

#include <libpkgresolve/request.h>

namespace pkgresolve {

using selection_authority = std::variant<
    pkgcatalog::catalog_candidate,
    pkgstate::installed_package>;

class selected_package final {
public:
  selected_package(resolution_environment environment,
                   architecture_context selected_architectures,
                   selection_authority authority,
                   pkgsource::package_release release,
                   pkgsource::source_snapshot_identity source_snapshot,
                   package_selection_identity identity);
  [[nodiscard]] resolution_environment environment() const noexcept;
  [[nodiscard]] selection_authority_kind authority_kind() const noexcept;
  [[nodiscard]] const architecture_context& architectures() const noexcept;
  [[nodiscard]] const selection_authority& authority() const noexcept;
  [[nodiscard]] const pkgcatalog::catalog_candidate* candidate() const noexcept;
  [[nodiscard]] const pkgstate::installed_package* installed() const noexcept;
  [[nodiscard]] const pkgsource::package_release& release() const noexcept;
  [[nodiscard]] const pkgsource::package_reference& package() const noexcept;
  [[nodiscard]] const pkgsource::source_snapshot_identity&
  source_snapshot() const noexcept;
  [[nodiscard]] const package_selection_identity& identity() const noexcept;
private:
  resolution_environment environment_;
  architecture_context selected_architectures_;
  selection_authority authority_;
  pkgsource::package_release release_;
  pkgsource::source_snapshot_identity source_snapshot_;
  package_selection_identity identity_;
};

class requirement_witness final {
public:
  [[nodiscard]] static requirement_witness catalog(
      pkgsource::source_snapshot_identity source,
      std::vector<pkgsource::requirement_origin> origins);
  [[nodiscard]] static requirement_witness installed(
      pkgstate::installed_package_identity package,
      std::vector<pkgstate::requirement_origin> origins);
  [[nodiscard]] requirement_authority_kind kind() const noexcept;
  [[nodiscard]] const std::optional<pkgsource::source_snapshot_identity>&
  catalog_source() const noexcept;
  [[nodiscard]] const std::optional<pkgstate::installed_package_identity>&
  installed_package() const noexcept;
  [[nodiscard]] const std::vector<pkgsource::requirement_origin>&
  catalog_origins() const noexcept;
  [[nodiscard]] const std::vector<pkgstate::requirement_origin>&
  installed_origins() const noexcept;
private:
  requirement_witness(
      requirement_authority_kind kind,
      std::optional<pkgsource::source_snapshot_identity> catalog_source,
      std::optional<pkgstate::installed_package_identity> installed_package,
      std::vector<pkgsource::requirement_origin> catalog_origins,
      std::vector<pkgstate::requirement_origin> installed_origins);
  requirement_authority_kind kind_;
  std::optional<pkgsource::source_snapshot_identity> catalog_source_;
  std::optional<pkgstate::installed_package_identity> installed_package_;
  std::vector<pkgsource::requirement_origin> catalog_origins_;
  std::vector<pkgstate::requirement_origin> installed_origins_;
};

class requirement_edge final {
public:
  requirement_edge(package_selection_identity issuer,
                   package_selection_identity required,
                   pkgsource::requirement_scope scope,
                   resolution_environment environment,
                   requirement_witness witness,
                   requirement_edge_identity identity);
  [[nodiscard]] const package_selection_identity& issuer() const noexcept;
  [[nodiscard]] const package_selection_identity& required() const noexcept;
  [[nodiscard]] const pkgsource::requirement_scope& scope() const noexcept;
  [[nodiscard]] resolution_environment environment() const noexcept;
  [[nodiscard]] const requirement_witness& witness() const noexcept;
  [[nodiscard]] const requirement_edge_identity& identity() const noexcept;
private:
  package_selection_identity issuer_;
  package_selection_identity required_;
  pkgsource::requirement_scope scope_;
  resolution_environment environment_;
  requirement_witness witness_;
  requirement_edge_identity identity_;
};

class goal_member final {
public:
  goal_member(pkgsource::package_reference package,
              package_selection_identity selection,
              std::optional<pkgsource::profile_identity> profile,
              std::vector<pkgsource::profile_expansion_step> expansion);
  [[nodiscard]] const pkgsource::package_reference& package() const noexcept;
  [[nodiscard]] const package_selection_identity& selection() const noexcept;
  [[nodiscard]] const std::optional<pkgsource::profile_identity>&
  profile() const noexcept;
  [[nodiscard]] const std::vector<pkgsource::profile_expansion_step>&
  expansion() const noexcept;
  friend bool operator==(const goal_member& lhs,
                         const goal_member& rhs) noexcept;
  friend bool operator!=(const goal_member& lhs,
                         const goal_member& rhs) noexcept;
  friend bool operator<(const goal_member& lhs,
                        const goal_member& rhs) noexcept;
private:
  pkgsource::package_reference package_;
  package_selection_identity selection_;
  std::optional<pkgsource::profile_identity> profile_;
  std::vector<pkgsource::profile_expansion_step> expansion_;
};

class resolved_goal final {
public:
  resolved_goal(resolution_goal goal,
                std::vector<goal_member> members,
                std::vector<package_selection_identity> selections,
                std::vector<requirement_edge_identity> edges,
                goal_closure_identity identity);
  [[nodiscard]] const resolution_goal& goal() const noexcept;
  [[nodiscard]] const std::vector<goal_member>& members() const noexcept;
  [[nodiscard]] const std::vector<package_selection_identity>&
  selections() const noexcept;
  [[nodiscard]] const std::vector<requirement_edge_identity>&
  edges() const noexcept;
  [[nodiscard]] const goal_closure_identity& identity() const noexcept;
private:
  resolution_goal goal_;
  std::vector<goal_member> members_;
  std::vector<package_selection_identity> selections_;
  std::vector<requirement_edge_identity> edges_;
  goal_closure_identity identity_;
};

class selection_reason final {
public:
  selection_reason(package_selection_identity selection,
                   selection_reason_kind kind,
                   pkgsource::requirement_scope scope,
                   std::optional<package_selection_identity> issuer,
                   std::optional<pkgsource::profile_reference> profile,
                   std::optional<pkgsource::profile_identity> profile_identity);
  [[nodiscard]] const package_selection_identity& selection() const noexcept;
  [[nodiscard]] selection_reason_kind kind() const noexcept;
  [[nodiscard]] const pkgsource::requirement_scope& scope() const noexcept;
  [[nodiscard]] const std::optional<package_selection_identity>&
  issuer() const noexcept;
  [[nodiscard]] const std::optional<pkgsource::profile_reference>&
  profile() const noexcept;
  [[nodiscard]] const std::optional<pkgsource::profile_identity>&
  profile_identity() const noexcept;
  friend bool operator==(const selection_reason& lhs,
                         const selection_reason& rhs) noexcept;
  friend bool operator!=(const selection_reason& lhs,
                         const selection_reason& rhs) noexcept;
  friend bool operator<(const selection_reason& lhs,
                        const selection_reason& rhs) noexcept;
private:
  package_selection_identity selection_;
  selection_reason_kind kind_;
  pkgsource::requirement_scope scope_;
  std::optional<package_selection_identity> issuer_;
  std::optional<pkgsource::profile_reference> profile_;
  std::optional<pkgsource::profile_identity> profile_identity_;
};

class resolution_result final {
public:
  resolution_result(resolution_request request,
                    std::vector<selected_package> selections,
                    std::vector<requirement_edge> edges,
                    std::vector<resolved_goal> goals,
                    std::vector<selection_reason> reasons,
                    resolution_result_identity identity);
  [[nodiscard]] const resolution_request& request() const noexcept;
  [[nodiscard]] const std::vector<selected_package>& selections() const noexcept;
  [[nodiscard]] const std::vector<requirement_edge>& edges() const noexcept;
  [[nodiscard]] const std::vector<resolved_goal>& goals() const noexcept;
  [[nodiscard]] const std::vector<selection_reason>& reasons() const noexcept;
  [[nodiscard]] const selected_package* find(
      const pkgsource::package_reference& package,
      resolution_environment environment,
      selection_authority_kind authority) const noexcept;
  [[nodiscard]] std::vector<selection_reason> reasons_for(
      const package_selection_identity& selection) const;
  [[nodiscard]] std::vector<requirement_edge> edges_for_scope(
      const pkgsource::requirement_scope& scope) const;
  [[nodiscard]] const resolution_result_identity& identity() const noexcept;
private:
  resolution_request request_;
  std::vector<selected_package> selections_;
  std::vector<requirement_edge> edges_;
  std::vector<resolved_goal> goals_;
  std::vector<selection_reason> reasons_;
  resolution_result_identity identity_;
};

} // namespace pkgresolve
