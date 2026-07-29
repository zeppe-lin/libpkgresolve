# Migration

There is no pre-native resolver API to preserve.

Historical pkgman dependency traversal, Pkgfile dependency strings, collection
path precedence, and `build_and_run` semantics are not accepted by this
library. A compatibility frontend must first produce native source snapshots,
a native catalog snapshot, and native installed state.

## Version 2 binary transition

Rebuild consumers against `libpkgresolve.so.2`.  Do not load resolver values
built against `libpkgcatalog.so.1` into a process using `libpkgcatalog.so.2`.
No resolution-input or semantic migration is required.
