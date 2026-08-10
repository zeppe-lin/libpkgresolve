# Native resolution authority

## Scope

`libpkgresolve` answers one question:

> Given a sealed available-package universe, sealed installed truth, explicit
> goals, architectures, and policy, which exact package authorities satisfy the
> requested typed closures, and why?

It does not plan, build, install, remove, discover collections, parse YAML,
inspect filesystems, publish state, or emulate historical pkgman behavior.

## Inputs

A sealed `resolution_request` contains:

- one `libpkgcatalog::catalog_snapshot`;
- one `libpkgstate::snapshot`;
- exact build and target architecture references;
- one or more package or authoritative profile goals;
- explicit installed-versus-catalog preference policy;
- a domain-separated request identity.

Goal diagnostic origins are retained but excluded from semantic identity.
Reordered goals produce the same request identity. Duplicate semantic goals are
rejected.

## Candidate and installed authority

Version 1 considers effective catalog candidates only. Shadowed candidates
remain catalog provenance and are never fallback candidates.

A flexible package demand may be satisfied by a compatible installed package
or the effective catalog candidate according to explicit policy. Build and
check roots require catalog source authority. Install lifecycle roots require a
catalog candidate. Remove lifecycle roots require installed authority.

Installed compatibility requires the retained selected build and target
architectures to match the requested environment exactly. Catalog compatibility
requires the candidate's declared architecture sets to admit the selected pair.

## Build and target environments

Selections are environment-qualified.

- A target-environment selection uses the request build architecture and target
  architecture.
- A build-environment selection uses the request build architecture as both its
  build and target architecture.

Build and check requirements select packages in the build environment. Run and
lifecycle requirements remain in the issuer's environment. This permits one
package name to have separate build- and target-environment selections during a
cross build.

## Typed closure semantics

The resolver retains build, check, run, and lifecycle scopes as separate native closures. They are never collapsed.

- A run goal follows runtime requirements recursively.
- A build goal follows build requirements; each selected build input then
  contributes its runtime closure in the build environment.
- A check goal follows check requirements and their runtime closures.
- A lifecycle goal follows requirements bound to exactly one lifecycle action
  and then their runtime closures.

Dependency cycles are represented as finite graph cycles. The resolver neither
orders them nor claims they are executable.

## Witnesses and reasons

Every root package selection is retained in a resolved goal. Profile goals
retain the exact sealed profile identity and expansion path.

Every dependency edge retains:

- issuer and required selection identities;
- exact requirement scope and selected environment;
- source-snapshot authority and native source requirement origins, or
- installed-package authority and durable state requirement origins;
- a domain-separated edge identity.

Selection reasons distinguish direct goals, profile membership, runtime,
build, check, and action-bound lifecycle requirements. The resolver does not
collapse those reasons into one installed-state reason; a later destination
adapter may project them under explicit policy.

## Identity domains

Version 1 defines separate SHA-256 domains for:

- resolution requests;
- package selections;
- requirement edges;
- per-goal closures;
- complete resolution results.

Result identity binds the exact request, normalized selections, edges, goal
closures, and selection reasons.

## Result closure invariant

`resolve()` never emits dangling graph references. Every requirement-edge
issuer and required selection, every goal member and goal selection, every goal
edge, and every selection-reason selection or issuer names a value retained in
the same `resolution_result`. Selection, edge, goal-selection, goal-edge, and
reason collections are normalized before the complete result identity is
sealed.

This is a producer guarantee, not permission for downstream callers to trust an
arbitrarily reconstructed value. A boundary that accepts a resolution result
from foreign persistence or another authority must still fail closed if that
reconstruction is inconsistent. The resolver does not make transaction or
planner admission policy by turning its public value constructors into another
orchestration boundary.

## Deliberate omissions

Version 1 has no:

- version ranges or candidate comparison;
- provider/virtual-package semantics;
- alternatives, conflicts, replaces, or recommendations;
- architecture fallback to shadowed collections;
- planner projection;
- filesystem, process, network, or database access;
- migration or compatibility frontend.

## Version 2 ABI boundary

`selected_package` retains `pkgcatalog::catalog_candidate` by value.  The
catalog ABI rebuild for `libpkgsource.so.2` therefore propagates through the
public resolution-result ABI.  Version 2 advances the resolver SONAME while
preserving selection, witness, closure, and identity semantics.

## Version 3 ABI boundary

The current authority graph changes both alternatives retained by
`selection_authority`: catalog candidates are rebuilt for source generation 3,
and installed packages are rebuilt for state generation 4. `resolution_request`
also retains the current catalog and state snapshots by value.

The resolver therefore advances to `libpkgresolve.so.3` and binds explicit
source, catalog, and state provider generations. An unchanged outer
`std::variant` or enclosing-class size is not accepted as ABI evidence; the
layout and semantics of every active by-value alternative are part of the
resolver ABI. Source is a direct public dependency because resolver headers and
values name source-owned types directly rather than inheriting that dependency
accidentally through the catalog.
