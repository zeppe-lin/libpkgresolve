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
