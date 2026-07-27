// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file identity.h
 *  \brief Domain-specific SHA-256 resolution identities.
 */
#pragma once

#include <string>

namespace pkgresolve {

#define PKGRESOLVE_DECLARE_IDENTITY(type_name)                                 \
class type_name final {                                                        \
public:                                                                        \
  [[nodiscard]] static type_name from_sha256(std::string hex);                 \
  [[nodiscard]] const std::string& hex() const noexcept;                       \
  friend bool operator==(const type_name& lhs, const type_name& rhs) noexcept; \
  friend bool operator!=(const type_name& lhs, const type_name& rhs) noexcept; \
  friend bool operator<(const type_name& lhs, const type_name& rhs) noexcept;  \
private:                                                                       \
  explicit type_name(std::string hex);                                         \
  std::string hex_;                                                            \
}

PKGRESOLVE_DECLARE_IDENTITY(resolution_request_identity);
PKGRESOLVE_DECLARE_IDENTITY(package_selection_identity);
PKGRESOLVE_DECLARE_IDENTITY(requirement_edge_identity);
PKGRESOLVE_DECLARE_IDENTITY(goal_closure_identity);
PKGRESOLVE_DECLARE_IDENTITY(resolution_result_identity);

#undef PKGRESOLVE_DECLARE_IDENTITY

} // namespace pkgresolve
