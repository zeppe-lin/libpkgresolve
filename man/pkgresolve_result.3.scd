pkgresolve_result(3)

# NAME

pkgresolve_result - selected package authorities and complete witnesses

# DESCRIPTION

*resolution_result* retains normalized *selected_package* values,
*requirement_edge* values, *resolved_goal* closures, *selection_reason* values,
and a complete result identity.

A selected package is backed by either one effective catalog candidate or one
installed package. Requirement witnesses retain either native source origins or
durable installed-state origins. Build, check, run, and lifecycle scopes remain
distinct.

# SEE ALSO

*libpkgresolve*(3), *pkgresolve_request*(3), *pkgresolve_semantics*(7)
