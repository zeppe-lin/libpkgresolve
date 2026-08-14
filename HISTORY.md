# History

## libpkgresolve 4.0.0

Catalog-4/source-4 authority ABI rebuild.

- Rebuilt catalog-retaining request and selection values against
  `libpkgcatalog.so.4` and advanced the resolver to `libpkgresolve.so.4`.
- Require `libpkgsource >= 4.0.0, < 5.0.0`,
  `libpkgcatalog >= 4.0.0, < 5.0.0`, and the existing
  `libpkgstate >= 3.1.0, < 4.0.0` authority.
- Preserve resolution semantics and identity domains. Source-3/catalog-3
  carriers are not admitted or reconstructed.
- Keep the reviewed symbol surface unchanged while advancing the provider and
  by-value carrier ABI generation.

## libpkgresolve 3.0.0

Source/catalog/state authority ABI rebuild and qualification closure.

- Rebuilt public request and selection values against `libpkgsource.so.3`,
  `libpkgcatalog.so.3`, and `libpkgstate.so.4`.
- Advanced `libpkgresolve` to SONAME 3 because `resolution_request`,
  `selection_authority`, and `selected_package` retain foreign authority values
  by value and their generation-2 layouts are not ABI-compatible with the
  current owners.
- Made the direct `libpkgsource` dependency explicit instead of inheriting a
  public source dependency accidentally through `libpkgcatalog`.
- Requires `libpkgsource >= 3.0.1, < 4.0.0`, `libpkgcatalog >= 3.0.1, < 4.0.0`,
  and `libpkgstate >= 3.1.0, < 4.0.0`.
- Refuses unsupported resolver vocabulary at public admission boundaries rather
  than stringifying it into sealed authority.
- Freezes one reviewed ELF surface, anchors public error RTTI, and qualifies
  the installed shared/static pkg-config product under GCC, Clang, and
  ASan/UBSan.
- Preserves resolver identity domains and dependency-selection semantics; this
  is an ABI-owner correction, not a resolution-policy redesign.

## libpkgresolve 2.0.0

ABI rebuild for libpkgcatalog 2 and libpkgsource 2.

- Rebuilt catalog-candidate-retaining selection values against `libpkgcatalog.so.2`.
- Raised the catalog dependency floor to `libpkgcatalog >= 2.0.0`.
- Advanced `libpkgresolve` to SONAME 2.
- Preserved resolution semantics, authority rules, and identity domains.

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
