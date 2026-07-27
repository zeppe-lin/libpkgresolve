// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file request.h
 *  \brief Sealed native dependency-resolution requests.
 */
#pragma once

#include <vector>

#include <libpkgcatalog/catalog.h>
#include <libpkgstate/snapshot.h>

#include <libpkgresolve/identity.h>
#include <libpkgresolve/model.h>

namespace pkgresolve {

class resolution_request final {
public:
  [[nodiscard]] static resolution_request seal(
      pkgcatalog::catalog_snapshot catalog,
      pkgstate::snapshot installed,
      architecture_context architectures,
      std::vector<resolution_goal> goals,
      resolution_policy policy = resolution_policy());

  [[nodiscard]] const pkgcatalog::catalog_snapshot& catalog() const noexcept;
  [[nodiscard]] const pkgstate::snapshot& installed() const noexcept;
  [[nodiscard]] const architecture_context& architectures() const noexcept;
  [[nodiscard]] const std::vector<resolution_goal>& goals() const noexcept;
  [[nodiscard]] const resolution_policy& policy() const noexcept;
  [[nodiscard]] const resolution_request_identity& identity() const noexcept;

private:
  resolution_request(pkgcatalog::catalog_snapshot catalog,
                     pkgstate::snapshot installed,
                     architecture_context architectures,
                     std::vector<resolution_goal> goals,
                     resolution_policy policy,
                     resolution_request_identity identity);
  pkgcatalog::catalog_snapshot catalog_;
  pkgstate::snapshot installed_;
  architecture_context architectures_;
  std::vector<resolution_goal> goals_;
  resolution_policy policy_;
  resolution_request_identity identity_;
};

} // namespace pkgresolve
