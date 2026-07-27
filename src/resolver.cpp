// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgresolve/resolver.h>

#include <libpkgresolve/error.h>

#include <libpkgsource/error.h>

#include "identity_support.h"

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <utility>

namespace pkgresolve {
namespace {

enum class selection_requirement { flexible, candidate_required, installed_required };

struct selection_key {
  std::string package;
  resolution_environment environment;
  selection_authority_kind authority;
  friend bool operator<(const selection_key& lhs,
                        const selection_key& rhs) noexcept
  {
    return std::tie(lhs.package, lhs.environment, lhs.authority)
         < std::tie(rhs.package, rhs.environment, rhs.authority);
  }
};

struct expansion_key {
  std::string selection;
  pkgsource::requirement_scope scope;
  friend bool operator<(const expansion_key& lhs,
                        const expansion_key& rhs) noexcept
  {
    return std::tie(lhs.selection, lhs.scope)
         < std::tie(rhs.selection, rhs.scope);
  }
};

void write_scope(detail::identity_writer& writer,
                 const pkgsource::requirement_scope& scope)
{
  writer.text(pkgsource::to_string(scope.kind()));
  writer.boolean(scope.action().has_value());
  if (scope.action())
    writer.text(pkgsource::to_string(*scope.action()));
}

void write_source_provenance(detail::identity_writer& writer,
                             const pkgsource::declaration_provenance& value)
{
  writer.text(value.document());
  writer.text(value.path());
  writer.number(value.line());
  writer.number(value.column());
}
void write_state_provenance(detail::identity_writer& writer,
                            const pkgstate::declaration_provenance& value)
{
  writer.text(value.document());
  writer.text(value.path());
  writer.number(value.line());
  writer.number(value.column());
}
void write_source_steps(
    detail::identity_writer& writer,
    const std::vector<pkgsource::profile_expansion_step>& steps)
{
  writer.number(steps.size());
  for (const auto& step : steps) {
    writer.text(step.profile().name());
    writer.text(pkgsource::to_string(step.member().kind()));
    writer.text(step.member().text());
    write_source_provenance(writer, step.provenance());
  }
}
void write_state_steps(
    detail::identity_writer& writer,
    const std::vector<pkgstate::profile_expansion_step>& steps)
{
  writer.number(steps.size());
  for (const auto& step : steps) {
    writer.text(step.profile().name());
    writer.number(static_cast<std::uint64_t>(step.member_kind()));
    writer.text(step.member());
    write_state_provenance(writer, step.provenance());
  }
}


std::string state_digest_hex(const pkgstate::digest_bytes& bytes)
{
  static constexpr char digits[] = "0123456789abcdef";
  std::string result;
  result.reserve(bytes.size() * 2);
  for (const std::uint8_t byte : bytes) {
    result.push_back(digits[(byte >> 4) & 0x0f]);
    result.push_back(digits[byte & 0x0f]);
  }
  return result;
}

pkgstate::lifecycle_action state_action(pkgsource::lifecycle_action action)
{
  switch (action) {
  case pkgsource::lifecycle_action::pre_install:
    return pkgstate::lifecycle_action::pre_install;
  case pkgsource::lifecycle_action::post_install:
    return pkgstate::lifecycle_action::post_install;
  case pkgsource::lifecycle_action::pre_remove:
    return pkgstate::lifecycle_action::pre_remove;
  case pkgsource::lifecycle_action::post_remove:
    return pkgstate::lifecycle_action::post_remove;
  }
  return pkgstate::lifecycle_action::pre_install;
}

bool allows(const std::vector<pkgsource::architecture_reference>& declared,
            const pkgsource::architecture_reference& selected)
{
  return declared.empty() ||
      std::find(declared.begin(), declared.end(), selected) != declared.end();
}

architecture_context selected_architectures(
    const resolution_request& request, resolution_environment environment)
{
  return architecture_context(
      request.architectures().build(),
      request.architectures().selected_target(environment));
}

bool candidate_compatible(const resolution_request& request,
                          const pkgcatalog::catalog_candidate& candidate,
                          resolution_environment environment)
{
  const architecture_context selected =
      selected_architectures(request, environment);
  const auto& requirements = candidate.source().recipe().architectures();
  return allows(requirements.build(), selected.build()) &&
         allows(requirements.target(), selected.target());
}

bool installed_compatible(const resolution_request& request,
                          const pkgstate::installed_package& installed,
                          resolution_environment environment)
{
  const architecture_context selected =
      selected_architectures(request, environment);
  const auto& binding = installed.control().source().architectures();
  return binding.selected_build().name() == selected.build().name() &&
         binding.selected_target().name() == selected.target().name();
}

const pkgcatalog::catalog_candidate* find_candidate(
    const pkgcatalog::catalog_snapshot& catalog,
    const pkgsource::package_reference& package)
{
  for (const auto& candidate : catalog.candidates())
    if (candidate.status() == pkgcatalog::candidate_status::effective &&
        candidate.package() == package)
      return &candidate;
  return nullptr;
}

pkgsource::package_release convert_release(const pkgstate::package_release& value)
{
  pkgsource::package_release converted(
      pkgsource::package_reference(value.name()), value.version(), value.release());
  if (converted.identity().hex() != state_digest_hex(value.identity().bytes()))
    throw error(error_code::inconsistent_authority,
                "installed package release identity does not match source domain");
  return converted;
}

package_selection_identity selection_identity(
    const resolution_request& request,
    resolution_environment environment,
    selection_authority_kind kind,
    const pkgsource::package_release& release,
    const pkgsource::source_snapshot_identity& source_snapshot,
    const architecture_context& architectures,
    const std::string& authority_identity)
{
  detail::identity_writer writer;
  writer.text("libpkgresolve/package-selection/1");
  writer.text(request.identity().hex());
  writer.text(to_string(environment));
  writer.text(to_string(kind));
  writer.text(release.identity().hex());
  writer.text(source_snapshot.hex());
  writer.text(architectures.build().name());
  writer.text(architectures.target().name());
  writer.text(authority_identity);
  return package_selection_identity::from_sha256(writer.finish());
}

requirement_edge_identity edge_identity(
    const resolution_request& request,
    const package_selection_identity& issuer,
    const package_selection_identity& required,
    const pkgsource::requirement_scope& scope,
    resolution_environment environment,
    const requirement_witness& witness)
{
  detail::identity_writer writer;
  writer.text("libpkgresolve/requirement-edge/1");
  writer.text(request.identity().hex());
  writer.text(issuer.hex());
  writer.text(required.hex());
  write_scope(writer, scope);
  writer.text(to_string(environment));
  writer.text(to_string(witness.kind()));
  if (witness.kind() == requirement_authority_kind::catalog_source) {
    writer.text(witness.catalog_source()->hex());
    writer.number(witness.catalog_origins().size());
    for (const auto& origin : witness.catalog_origins()) {
      write_source_provenance(writer, origin.declaration());
      write_source_steps(writer, origin.expansion());
    }
  } else {
    writer.text(witness.installed_package()->string());
    writer.number(witness.installed_origins().size());
    for (const auto& origin : witness.installed_origins()) {
      write_state_provenance(writer, origin.declaration());
      write_state_steps(writer, origin.expansion());
    }
  }
  return requirement_edge_identity::from_sha256(writer.finish());
}

selection_reason_kind reason_for_scope(
    const pkgsource::requirement_scope& scope)
{
  switch (scope.kind()) {
  case pkgsource::requirement_scope_kind::run:
    return selection_reason_kind::runtime_requirement;
  case pkgsource::requirement_scope_kind::build:
    return selection_reason_kind::build_requirement;
  case pkgsource::requirement_scope_kind::check:
    return selection_reason_kind::check_requirement;
  case pkgsource::requirement_scope_kind::lifecycle:
    return selection_reason_kind::lifecycle_requirement;
  }
  return selection_reason_kind::runtime_requirement;
}

resolution_environment dependency_environment(
    resolution_environment issuer,
    const pkgsource::requirement_scope& scope)
{
  if (scope.kind() == pkgsource::requirement_scope_kind::build ||
      scope.kind() == pkgsource::requirement_scope_kind::check)
    return resolution_environment::build;
  return issuer;
}

selection_requirement root_requirement(const resolution_goal& goal)
{
  switch (goal.scope().kind()) {
  case pkgsource::requirement_scope_kind::run:
    return selection_requirement::flexible;
  case pkgsource::requirement_scope_kind::build:
  case pkgsource::requirement_scope_kind::check:
    return selection_requirement::candidate_required;
  case pkgsource::requirement_scope_kind::lifecycle:
    switch (*goal.scope().action()) {
    case pkgsource::lifecycle_action::pre_install:
    case pkgsource::lifecycle_action::post_install:
      return selection_requirement::candidate_required;
    case pkgsource::lifecycle_action::pre_remove:
    case pkgsource::lifecycle_action::post_remove:
      return selection_requirement::installed_required;
    }
  }
  return selection_requirement::flexible;
}

class engine final {
public:
  explicit engine(resolution_request request) : request_(std::move(request)) {}

  resolution_result run()
  {
    std::vector<std::pair<resolution_goal, std::vector<goal_member>>> roots;
    for (const resolution_goal& goal : request_.goals())
      roots.emplace_back(goal, resolve_goal(goal));

    std::vector<resolved_goal> resolved_goals;
    resolved_goals.reserve(roots.size());
    for (auto& root : roots)
      resolved_goals.push_back(make_goal(root.first, std::move(root.second)));

    std::vector<selected_package> selections;
    selections.reserve(selections_.size());
    for (auto& entry : selections_)
      selections.push_back(std::move(entry.second));
    std::sort(selections.begin(), selections.end(),
              [](const selected_package& lhs, const selected_package& rhs) {
                return lhs.identity() < rhs.identity();
              });

    std::sort(edges_.begin(), edges_.end(),
              [](const requirement_edge& lhs, const requirement_edge& rhs) {
                return lhs.identity() < rhs.identity();
              });
    edges_.erase(std::unique(edges_.begin(), edges_.end(),
        [](const requirement_edge& lhs, const requirement_edge& rhs) {
          return lhs.identity() == rhs.identity();
        }), edges_.end());

    std::sort(reasons_.begin(), reasons_.end());
    reasons_.erase(std::unique(reasons_.begin(), reasons_.end()), reasons_.end());

    detail::identity_writer writer;
    writer.text("libpkgresolve/result/1");
    writer.text(request_.identity().hex());
    writer.number(selections.size());
    for (const auto& selection : selections)
      writer.text(selection.identity().hex());
    writer.number(edges_.size());
    for (const auto& edge : edges_)
      writer.text(edge.identity().hex());
    writer.number(resolved_goals.size());
    for (const auto& goal : resolved_goals)
      writer.text(goal.identity().hex());
    writer.number(reasons_.size());
    for (const auto& reason : reasons_) {
      writer.text(reason.selection().hex());
      writer.text(to_string(reason.kind()));
      write_scope(writer, reason.scope());
      writer.boolean(reason.issuer().has_value());
      if (reason.issuer()) writer.text(reason.issuer()->hex());
      writer.boolean(reason.profile().has_value());
      if (reason.profile()) writer.text(reason.profile()->name());
      writer.boolean(reason.profile_identity().has_value());
      if (reason.profile_identity()) writer.text(reason.profile_identity()->hex());
    }
    const resolution_result_identity identity =
        resolution_result_identity::from_sha256(writer.finish());
    return resolution_result(std::move(request_), std::move(selections),
                             std::move(edges_), std::move(resolved_goals),
                             std::move(reasons_), identity);
  }

private:
  selected_package& select(const pkgsource::package_reference& package,
                           resolution_environment environment,
                           selection_requirement requirement)
  {
    const pkgcatalog::catalog_candidate* candidate =
        find_candidate(request_.catalog(), package);
    const pkgstate::installed_package* installed =
        request_.installed().find_package(package.name());
    const bool candidate_ok = candidate &&
        candidate_compatible(request_, *candidate, environment);
    const bool installed_ok = installed &&
        installed_compatible(request_, *installed, environment);

    selection_authority_kind kind;
    if (requirement == selection_requirement::candidate_required) {
      if (!candidate)
        throw error(error_code::missing_candidate,
                    "catalog has no effective candidate for " + package.name());
      if (!candidate_ok)
        throw error(error_code::architecture_mismatch,
                    "catalog candidate is architecture-incompatible: " +
                    package.name());
      kind = selection_authority_kind::catalog_candidate;
    } else if (requirement == selection_requirement::installed_required) {
      if (!installed)
        throw error(error_code::missing_installed_package,
                    "package is not installed: " + package.name());
      if (!installed_ok)
        throw error(error_code::architecture_mismatch,
                    "installed package is architecture-incompatible: " +
                    package.name());
      kind = selection_authority_kind::installed_package;
    } else if (request_.policy().preference() ==
               installed_preference::retain_compatible) {
      if (installed_ok)
        kind = selection_authority_kind::installed_package;
      else if (candidate_ok)
        kind = selection_authority_kind::catalog_candidate;
      else
        fail_unavailable(package, candidate, installed);
    } else {
      if (candidate_ok)
        kind = selection_authority_kind::catalog_candidate;
      else if (installed_ok)
        kind = selection_authority_kind::installed_package;
      else
        fail_unavailable(package, candidate, installed);
    }

    selection_key key{package.name(), environment, kind};
    const auto found = selections_.find(key);
    if (found != selections_.end())
      return found->second;

    const architecture_context selected =
        selected_architectures(request_, environment);
    if (kind == selection_authority_kind::catalog_candidate) {
      pkgsource::package_release release = candidate->release();
      const auto source = candidate->source().identity();
      const auto identity = selection_identity(
          request_, environment, kind, release, source, selected,
          candidate->identity().hex());
      auto inserted = selections_.emplace(
          std::move(key), selected_package(
              environment, selected, *candidate, std::move(release), source,
              identity));
      return inserted.first->second;
    }

    pkgsource::package_release release = convert_release(installed->release());
    const auto source = pkgsource::source_snapshot_identity::from_sha256(
        state_digest_hex(installed->control().source().snapshot().bytes()));
    const auto identity = selection_identity(
        request_, environment, kind, release, source, selected,
        installed->identity().string());
    auto inserted = selections_.emplace(
        std::move(key), selected_package(
            environment, selected, *installed, std::move(release), source,
            identity));
    return inserted.first->second;
  }

  [[noreturn]] void fail_unavailable(
      const pkgsource::package_reference& package,
      const pkgcatalog::catalog_candidate* candidate,
      const pkgstate::installed_package* installed) const
  {
    if (!candidate && !installed)
      throw error(error_code::unknown_package,
                  "package is absent from catalog and installed state: " +
                  package.name());
    throw error(error_code::architecture_mismatch,
                "no compatible authority for package: " + package.name());
  }

  std::vector<goal_member> resolve_goal(const resolution_goal& goal)
  {
    std::vector<goal_member> members;
    if (goal.subject().kind() == pkgsource::requirement_subject_kind::package) {
      selected_package& selection = select(
          goal.subject().package(), resolution_environment::target,
          root_requirement(goal));
      members.emplace_back(goal.subject().package(), selection.identity(),
                           std::nullopt,
                           std::vector<pkgsource::profile_expansion_step>{});
      reasons_.emplace_back(selection.identity(),
                            selection_reason_kind::direct_goal,
                            goal.scope(), std::nullopt, std::nullopt,
                            std::nullopt);
      expand(selection, goal.scope());
    } else {
      const pkgsource::sealed_profile* profile = nullptr;
      try {
        profile = &request_.catalog().profiles().require(
            goal.subject().profile());
      } catch (const pkgsource::error&) {
        throw error(error_code::unknown_profile,
                    "unknown resolution profile: " +
                    goal.subject().profile().name());
      }
      for (const auto& path : profile->expansion()) {
        selected_package& selection = select(
            path.package(), resolution_environment::target,
            root_requirement(goal));
        members.emplace_back(path.package(), selection.identity(),
                             profile->identity(), path.steps());
        reasons_.emplace_back(selection.identity(),
                              selection_reason_kind::profile_goal,
                              goal.scope(), std::nullopt, profile->name(),
                              profile->identity());
        expand(selection, goal.scope());
      }
    }
    std::sort(members.begin(), members.end());
    members.erase(std::unique(members.begin(), members.end()), members.end());
    return members;
  }

  void expand(selected_package& issuer,
              const pkgsource::requirement_scope& scope)
  {
    expansion_key key{issuer.identity().hex(), scope};
    if (!expanded_.insert(key).second)
      return;

    if (const auto* candidate = issuer.candidate()) {
      std::vector<pkgsource::resolved_requirement> requirements;
      switch (scope.kind()) {
      case pkgsource::requirement_scope_kind::build:
        requirements = candidate->source().recipe().build_requirements();
        break;
      case pkgsource::requirement_scope_kind::run:
        requirements = candidate->source().recipe().run_requirements();
        break;
      case pkgsource::requirement_scope_kind::check:
        requirements = candidate->source().recipe().check_requirements();
        break;
      case pkgsource::requirement_scope_kind::lifecycle:
        requirements = candidate->source().recipe().lifecycle_requirements(
            *scope.action());
        break;
      }
      for (const auto& requirement : requirements) {
        const auto environment = dependency_environment(
            issuer.environment(), requirement.scope());
        selected_package& required = select(
            requirement.package(), environment,
            selection_requirement::flexible);
        requirement_witness witness = requirement_witness::catalog(
            candidate->source().identity(), requirement.origins());
        add_edge(issuer, required, requirement.scope(), environment,
                 std::move(witness));
        expand(required, pkgsource::requirement_scope::run());
      }
      return;
    }

    const auto* installed = issuer.installed();
    if (scope.kind() == pkgsource::requirement_scope_kind::build ||
        scope.kind() == pkgsource::requirement_scope_kind::check)
      throw error(error_code::inconsistent_authority,
                  "installed authority cannot supply build/check requirements");

    std::vector<pkgstate::package_requirement> requirements;
    if (scope.kind() == pkgsource::requirement_scope_kind::run)
      requirements = installed->control().source().runtime_requirements();
    else
      requirements = installed->control().source().lifecycle_requirements(
          state_action(*scope.action()));

    for (const auto& requirement : requirements) {
      pkgsource::package_reference package(requirement.package().name());
      const auto environment = dependency_environment(
          issuer.environment(), scope);
      selected_package& required = select(
          package, environment, selection_requirement::flexible);
      requirement_witness witness = requirement_witness::installed(
          installed->identity(), requirement.origins());
      add_edge(issuer, required, scope, environment, std::move(witness));
      expand(required, pkgsource::requirement_scope::run());
    }
  }

  void add_edge(selected_package& issuer,
                selected_package& required,
                const pkgsource::requirement_scope& scope,
                resolution_environment environment,
                requirement_witness witness)
  {
    const auto identity = edge_identity(
        request_, issuer.identity(), required.identity(), scope,
        environment, witness);
    edges_.emplace_back(issuer.identity(), required.identity(), scope,
                        environment, std::move(witness), identity);
    reasons_.emplace_back(required.identity(), reason_for_scope(scope), scope,
                          issuer.identity(), std::nullopt, std::nullopt);
  }

  resolved_goal make_goal(const resolution_goal& goal,
                          std::vector<goal_member> members) const
  {
    std::set<std::string> selected_ids;
    std::set<std::string> edge_ids;
    std::vector<std::string> queue;
    for (const auto& member : members) {
      selected_ids.insert(member.selection().hex());
      queue.push_back(member.selection().hex());
    }
    for (std::size_t index = 0; index < queue.size(); ++index) {
      for (const auto& edge : edges_) {
        if (edge.issuer().hex() != queue[index])
          continue;
        edge_ids.insert(edge.identity().hex());
        if (selected_ids.insert(edge.required().hex()).second)
          queue.push_back(edge.required().hex());
      }
    }
    std::vector<package_selection_identity> selections;
    for (const auto& value : selected_ids)
      selections.push_back(package_selection_identity::from_sha256(value));
    std::vector<requirement_edge_identity> edges;
    for (const auto& value : edge_ids)
      edges.push_back(requirement_edge_identity::from_sha256(value));

    detail::identity_writer writer;
    writer.text("libpkgresolve/goal-closure/1");
    writer.text(request_.identity().hex());
    write_scope(writer, goal.scope());
    writer.text(pkgsource::to_string(goal.subject().kind()));
    writer.text(goal.subject().text());
    writer.number(members.size());
    for (const auto& member : members) {
      writer.text(member.package().name());
      writer.text(member.selection().hex());
      writer.boolean(member.profile().has_value());
      if (member.profile()) writer.text(member.profile()->hex());
      write_source_steps(writer, member.expansion());
    }
    writer.number(selections.size());
    for (const auto& selection : selections) writer.text(selection.hex());
    writer.number(edges.size());
    for (const auto& edge : edges) writer.text(edge.hex());
    return resolved_goal(goal, std::move(members), std::move(selections),
                         std::move(edges),
                         goal_closure_identity::from_sha256(writer.finish()));
  }

  resolution_request request_;
  std::map<selection_key, selected_package> selections_;
  std::vector<requirement_edge> edges_;
  std::vector<selection_reason> reasons_;
  std::set<expansion_key> expanded_;
};

} // namespace

resolution_result resolve(resolution_request request)
{
  return engine(std::move(request)).run();
}

} // namespace pkgresolve
