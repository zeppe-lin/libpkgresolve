# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
scdoc = find_program('scdoc', required: get_option('man_pages'))
if scdoc.found()
  pages = [
    ['libpkgresolve.3.scd', 'libpkgresolve.3', 'man3'],
    ['pkgresolve_model.3.scd', 'pkgresolve_model.3', 'man3'],
    ['pkgresolve_request.3.scd', 'pkgresolve_request.3', 'man3'],
    ['pkgresolve_result.3.scd', 'pkgresolve_result.3', 'man3'],
    ['pkgresolve_semantics.7.scd', 'pkgresolve_semantics.7', 'man7'],
  ]
  foreach page : pages
    custom_target(
      page[1],
      input: page[0],
      output: page[1],
      command: [scdoc],
      feed: true,
      capture: true,
      install: true,
      install_dir: join_paths(get_option('mandir'), page[2]),
    )
  endforeach
endif
