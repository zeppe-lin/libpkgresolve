# Maintaining

Release qualification must include:

- strict C++17 builds;
- all model and resolver tests;
- standalone public-header compilation;
- shared and static consumer checks;
- SONAME and dynamic dependency inspection;
- ASan and UBSan where available;
- release metadata and documentation-source checks;
- independent `git am` replay with final-tree and stable patch-ID comparison.

Do not claim generated Meson, scdoc, mandoc, or Doxygen checks unless they were
actually run.
For the 3.0 generation, the reviewed shared ABI is exactly one 134-symbol inventory. Do not
widen the `libpkgsource`, `libpkgcatalog`, or `libpkgstate` generation ranges
without reviewing every resolver public value that retains those owners by
value. A same-size outer `std::variant` is not ABI evidence: the active foreign
alternative layout is part of the resolver ABI.

The x86-64 by-value layout test, installed consumer, and ELF dependency-edge contracts are release gates.
Do not replace them with build-tree symbol references or header-only probes.
