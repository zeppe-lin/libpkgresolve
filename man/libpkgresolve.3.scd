libpkgresolve(3)

# NAME

libpkgresolve - native deterministic package-resolution authority

# SYNOPSIS

*#include <libpkgresolve/libpkgresolve.h>*

# DESCRIPTION

*libpkgresolve* consumes a sealed package catalog, sealed installed state,
explicit package or profile goals, exact architecture context, and typed policy.
It returns exact package selections, typed requirement edges, per-goal closures,
retained witnesses, selection reasons, and a sealed result identity.

Version 1 resolves exact package names only.

# BOUNDARY

The library does not discover collections, parse YAML, compare versions,
construct operation plans, build packages, inspect archives, mutate filesystems,
publish installed state, or preserve historical pkgman behavior.

# ERRORS

Contract and resolution failures throw *pkgresolve::error*. Branch on
*error_code*, not diagnostic text.

# SEE ALSO

*pkgresolve_model*(3), *pkgresolve_request*(3), *pkgresolve_result*(3),
*pkgresolve_semantics*(7), *libpkgcatalog*(3), *libpkgstate*(3)
