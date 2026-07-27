# Testing

The native suite covers:

- identity validation and domain separation;
- request normalization, origin independence, and duplicate-goal rejection;
- package and profile roots;
- deterministic runtime closure traversal;
- installed-state satisfaction and catalog preference;
- source and installed-state requirement witnesses;
- build and check closures in the build environment;
- action-bound install and removal lifecycle closures;
- profile expansion reasons;
- architecture rejection and cross-build environment selection;
- finite cyclic dependency graphs;
- reordered-input result equivalence;
- unknown package and missing installed-authority failures;
- standalone public headers;
- release and authority-boundary contracts.

Run:

```sh
meson test -C build --print-errorlogs
```

Sanitizer, shared, and static builds should use separate build directories.
