pkgresolve_model(3)

# NAME

pkgresolve_model - native resolution goals, environments, and policy

# DESCRIPTION

*architecture_context* binds exact build and target architectures.
*resolution_goal* binds one native requirement scope to an exact package or
sealed profile subject. *resolution_policy* selects whether compatible installed
packages are retained or effective catalog candidates are preferred.

Goal origins are diagnostic provenance and do not enter request identity.

Selections are qualified as *build* or *target* environment values. Build and
check requirements select build-environment packages. Run and lifecycle
requirements remain in the issuer environment.

# SEE ALSO

*libpkgresolve*(3), *pkgresolve_request*(3), *pkgresolve_semantics*(7)
