# libpkgresolve

`libpkgresolve` is the native Zeppe-Lin package-selection authority.

It consumes an immutable `libpkgcatalog::catalog_snapshot`, an immutable
`libpkgstate::snapshot`, explicit package or profile goals, exact build and
target architectures, and typed selection policy. It returns deterministic
package selections, typed requirement edges, complete goal closures, retained
source/state witnesses, and a sealed result identity.

The library does not discover collections, parse package syntax, mutate state,
construct operation plans, build packages, inspect archives, or apply
filesystem transitions.

## Authority chain

```text
libpkgsource   declares one package release
libpkgcatalog  seals the available package universe
libpkgstate    seals installed truth
libpkgresolve  selects exact authorities and typed closures
libpkgplan     orders an admitted transition later
```

Version 1 resolves exact package names only. It has no version predicates,
virtual providers, conflicts, alternatives, package filename interpretation,
or historical pkgman behavior.

## Build

```sh
meson setup build
meson compile -C build
meson test -C build --print-errorlogs
```

Shared and static libraries use separate build directories. Set
`-Ddefault_library=static -Dlink_mode=static` for a static build.
