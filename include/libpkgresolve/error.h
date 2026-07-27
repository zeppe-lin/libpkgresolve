// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file error.h
 *  \brief Typed native resolution failures.
 */
#pragma once

#include <stdexcept>
#include <string>

namespace pkgresolve {

enum class error_code {
  invalid_request,
  invalid_identity,
  duplicate_goal,
  unknown_package,
  unknown_profile,
  missing_candidate,
  missing_installed_package,
  architecture_mismatch,
  inconsistent_authority,
  identity_failed,
};

class error : public std::runtime_error {
public:
  error(error_code code, std::string message);
  [[nodiscard]] error_code code() const noexcept;
private:
  error_code code_;
};

} // namespace pkgresolve
