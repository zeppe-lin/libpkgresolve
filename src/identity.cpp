// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgresolve/identity.h>

#include "identity_support.h"

#include <utility>

namespace pkgresolve {
#define PKGRESOLVE_DEFINE_IDENTITY(type_name)                                  \
type_name::type_name(std::string hex) : hex_(std::move(hex)) {}                \
type_name type_name::from_sha256(std::string hex)                              \
{                                                                              \
  detail::require_sha256_hex(hex);                                             \
  return type_name(std::move(hex));                                            \
}                                                                              \
const std::string& type_name::hex() const noexcept { return hex_; }             \
bool operator==(const type_name& lhs, const type_name& rhs) noexcept           \
{ return lhs.hex_ == rhs.hex_; }                                               \
bool operator!=(const type_name& lhs, const type_name& rhs) noexcept           \
{ return !(lhs == rhs); }                                                      \
bool operator<(const type_name& lhs, const type_name& rhs) noexcept            \
{ return lhs.hex_ < rhs.hex_; }

PKGRESOLVE_DEFINE_IDENTITY(resolution_request_identity)
PKGRESOLVE_DEFINE_IDENTITY(package_selection_identity)
PKGRESOLVE_DEFINE_IDENTITY(requirement_edge_identity)
PKGRESOLVE_DEFINE_IDENTITY(goal_closure_identity)
PKGRESOLVE_DEFINE_IDENTITY(resolution_result_identity)
#undef PKGRESOLVE_DEFINE_IDENTITY
} // namespace pkgresolve
