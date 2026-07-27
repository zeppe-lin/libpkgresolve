pkgresolve_request(3)

# NAME

pkgresolve_request - sealed native dependency-resolution request

# DESCRIPTION

*resolution_request::seal()* validates and normalizes one catalog snapshot, one
installed-state snapshot, architecture context, explicit goals, and policy.

The request rejects an empty goal set and duplicate semantic goals. Goal order
and diagnostic origin text do not affect request identity.

# SEE ALSO

*libpkgresolve*(3), *pkgresolve_model*(3), *pkgresolve_result*(3)
