// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file resolver.h
 *  \brief Deterministic native dependency closure resolution.
 */
#pragma once

#include <libpkgresolve/result.h>

namespace pkgresolve {

/*! \brief Resolve exact package selections and typed closure witnesses.
 *
 * The function is pure with respect to package state: it reads only the sealed
 * catalog and installed snapshot retained by the request. It neither plans nor
 * mutates any target.
 */
[[nodiscard]] resolution_result resolve(resolution_request request);

} // namespace pkgresolve
