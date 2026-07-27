# History

## libpkgresolve 1.0.0

Initial native package-resolution authority.

- seals deterministic requests from exact catalog and installed snapshots;
- accepts package and authoritative profile goals;
- distinguishes build, check, run, and action-bound lifecycle closures;
- separates build- and target-environment package selections;
- selects effective catalog candidates or compatible installed packages under
  explicit policy;
- retains complete source and installed-state requirement witnesses;
- retains profile expansion and selection reasons;
- represents dependency cycles without inventing execution order;
- publishes domain-separated request, selection, edge, closure, and result
  identities;
- deliberately excludes discovery, parsing, planning, execution, mutation, and
  historical compatibility.
