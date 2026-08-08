// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "../support/test.h"

#include <libpkgresolve/libpkgresolve.h>

#include <string>
#include <vector>

int main()
{
  using namespace pkgresolve;

  const pkgsource::declaration_provenance first_provenance(
      "recipe.yml", "requirements.run[0]", 4, 2);
  const pkgsource::declaration_provenance second_provenance(
      "recipe.yml", "requirements.run[1]", 5, 2);
  const pkgsource::requirement_origin first_source(first_provenance, {});
  const pkgsource::requirement_origin second_source(second_provenance, {});
  const auto source_identity =
      pkgsource::source_snapshot_identity::from_sha256(std::string(64, '1'));

  const auto catalog = requirement_witness::catalog(
      source_identity, {second_source, first_source, second_source});
  TEST_CHECK(catalog.kind() == requirement_authority_kind::catalog_source);
  TEST_CHECK(catalog.catalog_source().has_value());
  TEST_CHECK(*catalog.catalog_source() == source_identity);
  TEST_CHECK(!catalog.installed_package().has_value());
  TEST_CHECK(catalog.catalog_origins().size() == 2);
  TEST_CHECK(catalog.installed_origins().empty());
  TEST_CHECK(catalog.catalog_origins()[0] < catalog.catalog_origins()[1]);
  TEST_THROWS(error_code::inconsistent_authority,
      requirement_witness::catalog(source_identity, {}));

  const pkgstate::declaration_provenance first_state_provenance(
      "recipe.yml", "requirements.run[0]", 4, 2);
  const pkgstate::declaration_provenance second_state_provenance(
      "recipe.yml", "requirements.run[1]", 5, 2);
  const pkgstate::requirement_origin first_state(first_state_provenance, {});
  const pkgstate::requirement_origin second_state(second_state_provenance, {});
  const auto installed_identity = pkgstate::installed_package_identity::from_sha256(
      pkgstate::sha256_digest_bytes{});
  const auto installed = requirement_witness::installed(
      installed_identity, {second_state, first_state, second_state});
  TEST_CHECK(installed.kind() == requirement_authority_kind::installed_state);
  TEST_CHECK(!installed.catalog_source().has_value());
  TEST_CHECK(installed.installed_package().has_value());
  TEST_CHECK(*installed.installed_package() == installed_identity);
  TEST_CHECK(installed.catalog_origins().empty());
  TEST_CHECK(installed.installed_origins().size() == 2);
  TEST_CHECK(installed.installed_origins()[0] < installed.installed_origins()[1]);
  TEST_THROWS(error_code::inconsistent_authority,
      requirement_witness::installed(installed_identity, {}));
  return 0;
}
