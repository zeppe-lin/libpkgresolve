# Testing

`libpkgresolve` qualification is separated by evidence role. Resolver coverage
is not measured by how much policy one large scenario happens to traverse; the
suite keeps request/value semantics, neighbor-boundary composition, graph
closure, public-header closure, and static contracts independently diagnosable.

## Qualification roles

- `tests/unit/` covers resolver-owned values that do not need a real resolution
  pass: all five identity types, enum spellings, architecture context, policy,
  and semantic goal comparison.
- `tests/integration/` constructs genuine `libpkgsource`, `libpkgcatalog`, and
  `libpkgstate` authority and drives the real resolver. These tests qualify
  request sealing, candidate/installed selection, typed closure traversal,
  lifecycle authority, profiles, cross architecture, cycles, result closure,
  query views, and typed failures.
- `tests/fixtures/` constructs deterministic source/catalog/state fiction using
  the real public APIs. Fixtures do not replace dependencies with resolver-local
  mocks and do not encode expected resolver policy.
- `tests/support/` contains assertions and graph lookup helpers only.
- `tests/contracts/` checks authority direction, release/pkg-config metadata,
  and the qualification topology itself.
- the `header` suite compiles every installed public header as a standalone
  translation unit through the declared dependency closure.

`resolve()` is the producer authority for normalized, referentially closed
results. Downstream callers remain responsible for fail-closed admission if a
result is reconstructed outside that producer. The caller-side half of that
seam is intentionally tested in `libpkgtransaction`; this repository does not
acquire an upward dependency merely to test an orchestrator.

## Behavioral matrix

The native suite proves:

- `unit/identity_test.cpp` qualifies all five resolver identity domains,
  comparison semantics, lowercase SHA-256 validation, and malformed refusal;
- `unit/model_test.cpp` qualifies enum vocabulary, build/target architecture
  selection, installed preference values, and diagnostic-origin independence;
- `unit/witness_test.cpp` proves catalog/state witness origin normalization,
  authority-specific payloads, and empty-evidence refusal;
- `integration/request_test.cpp` proves request normalization, catalog/state
  retention, reordered-goal identity stability, duplicate semantic-goal
  refusal, and empty-request refusal;
- `integration/runtime_closure_test.cpp` proves recursive runtime traversal,
  exact catalog-source witnesses, origins, result query views, and goal closure;
- `integration/installed_authority_test.cpp` proves compatible installed
  retention, installed-state witness binding, recursive installed runtime
  authority, and explicit catalog preference;
- `integration/build_check_test.cpp` keeps build and check scopes distinct and
  proves build-environment dependency/runtime closure selection;
- `integration/lifecycle_test.cpp` proves install lifecycle requires catalog
  authority while removal lifecycle can operate from installed authority
  without a current package candidate;
- `integration/profile_goal_test.cpp` retains exact sealed profile identity,
  expansion paths, and profile selection reasons;
- `integration/architecture_test.cpp` proves incompatible selection refusal and
  separate build/target architecture contexts during cross resolution;
- `integration/mixed_authority_test.cpp` proves one package may legitimately
  have distinct catalog and installed selections under different goal demands;
- `integration/cycle_test.cpp` proves runtime cycles remain finite graph cycles
  without invented execution order;
- `integration/result_integrity_test.cpp` proves every resolver-produced edge,
  goal, member, and reason reference is retained in the same result, normalized
  identity collections remain ordered/unique, and reordered semantic inputs
  reproduce the same complete result identity;
- `integration/failure_test.cpp` distinguishes unknown package/profile,
  missing candidate, missing installed authority, architecture mismatch, and
  inconsistent installed release authority;
- every public header is self-contained under the declared dependency closure;
  and
- release, pkg-config, authority, and test-layout contracts remain explicit.

## Running

Run all evidence roles:

```sh
meson test -C build --print-errorlogs
```

Run one role while diagnosing a failure:

```sh
meson test -C build --suite unit --print-errorlogs
meson test -C build --suite integration --print-errorlogs
meson test -C build --suite header --print-errorlogs
meson test -C build --suite contract --print-errorlogs
```

Release qualification still requires separate shared/static build directories
and the project sanitizer matrix. Passing a unit or source-contract subset is
not proof of the source/catalog/state-to-resolver composition seam.

The categorized sources are pinned by
`tests/contracts/check_test_layout.sh`; new tests should enter the evidence role
they prove rather than accumulate again in `tests/` root.
