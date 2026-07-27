# Migration

There is no pre-native resolver API to preserve.

Historical pkgman dependency traversal, Pkgfile dependency strings, collection
path precedence, and `build_and_run` semantics are not accepted by this
library. A compatibility frontend must first produce native source snapshots,
a native catalog snapshot, and native installed state.
