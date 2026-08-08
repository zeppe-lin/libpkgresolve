# Test fixtures

The fixture headers construct deterministic fiction through the real public
`libpkgsource`, `libpkgcatalog`, and `libpkgstate` APIs. They do not replace
those libraries with resolver-local mocks.

`source_state.h` owns reusable profile/source/catalog/installed-state fiction.
`resolution.h` adds resolver goals, request sealing, and real `resolve()` calls.

Assertions and graph queries belong in `tests/support/`; fixture code establishes
input authority but does not decide expected resolver semantics.
