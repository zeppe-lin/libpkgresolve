pkgresolve_semantics(7)

# NAME

pkgresolve_semantics - native closure and authority rules

# CANDIDATES

Only effective catalog candidates are selectable. Shadowed candidates are not
architecture or availability fallbacks.

# INSTALLED SATISFACTION

A compatible installed package may satisfy a flexible demand under explicit
policy. Build and check roots require catalog source authority. Install
lifecycle roots require a candidate; removal lifecycle roots require installed
authority.

# CLOSURES

Run requirements recurse in the issuer environment. Build and check requirements
select build-environment packages, then include those packages' runtime
closures. Lifecycle requirements remain bound to one exact action and include
their runtime closures.

Cycles are retained as finite graph cycles. No execution order is implied.

# AUTHORITY

The result retains exact source or state witnesses for every dependency edge and
exact profile expansion for every profile root. The resolver does not plan or
mutate package state.

# SEE ALSO

*libpkgresolve*(3), *libpkgsource*(3), *libpkgcatalog*(3), *libpkgstate*(3)
