// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "source_state.h"

#include <string>
#include <utility>
#include <vector>

#include <libpkgresolve/libpkgresolve.h>

namespace fixture {

inline pkgresolve::resolution_goal package_goal(
    pkgsource::requirement_scope scope, std::string package,
    std::string origin = "test")
{
  return pkgresolve::resolution_goal(
      std::move(scope),
      pkgsource::requirement_subject(
          pkgsource::package_reference(std::move(package))),
      std::move(origin));
}

inline pkgresolve::resolution_goal profile_goal(
    pkgsource::requirement_scope scope, std::string profile,
    std::string origin = "test")
{
  return pkgresolve::resolution_goal(
      std::move(scope),
      pkgsource::requirement_subject(
          pkgsource::profile_reference(std::move(profile))),
      std::move(origin));
}

inline pkgresolve::resolution_request request(
    pkgcatalog::catalog_snapshot catalog,
    pkgstate::snapshot state,
    std::vector<pkgresolve::resolution_goal> goals,
    pkgresolve::installed_preference preference =
        pkgresolve::installed_preference::retain_compatible,
    std::string build = "x86_64",
    std::string target = "x86_64")
{
  return pkgresolve::resolution_request::seal(
      std::move(catalog), std::move(state),
      pkgresolve::architecture_context(
          pkgsource::architecture_reference(std::move(build)),
          pkgsource::architecture_reference(std::move(target))),
      std::move(goals), pkgresolve::resolution_policy(preference));
}

inline pkgresolve::resolution_result resolution(
    pkgcatalog::catalog_snapshot catalog,
    pkgstate::snapshot state,
    std::vector<pkgresolve::resolution_goal> goals,
    pkgresolve::installed_preference preference =
        pkgresolve::installed_preference::retain_compatible,
    std::string build = "x86_64",
    std::string target = "x86_64")
{
  return pkgresolve::resolve(request(
      std::move(catalog), std::move(state), std::move(goals),
      preference, std::move(build), std::move(target)));
}

} // namespace fixture
