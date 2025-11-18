# Milestone 1 – Baseline Assessment

This document reconciles the current codebase state with the functionality promised in `README.md`. It also catalogues every TODO or missing implementation across `src/*.c` to surface capability gaps before development proceeds.

## README Features vs Header Support

| Feature area | README promise | Header/API reality | Gap summary |
|--------------|----------------|--------------------|-------------|
| Database management (`OPEN`, `SAVE`) | Load and save tab-separated files; default file path; dirty tracking | `database.h` exposes `cms_database_load/save`, `StudentDatabase` tracks `file_path`, `is_loaded`, `is_dirty` | APIs exist but implementations are placeholders. No strategy documented for invalid file formats or recovering from failed loads, which explains the current “opening invalid file” issue. |
| CRUD (`INSERT`, `QUERY`, `UPDATE`, `DELETE`) | Full create/read/update/delete workflow with validation and prompts | `commands.h` declares handlers; `database.h` exposes record ops | Handler prototypes exist but `cmd_insert/update/delete` and `cms_database_*` bodies are `CMS_STATUS_NOT_IMPLEMENTED`. No API yet for interactive prompts (e.g., partial updates). |
| Data validation | Validate IDs, names, programmes, marks; CLI prevents invalid entries | `utils.h` declares validation helpers, read/trim utilities | `cms_validate_*` functions are minimal and lack bounds/format checks; `cms_trim_string` is unimplemented yet `cmd_open` uses a non-existent `cms_trim`. Case-insensitive comparisons (`cms_string_equals_ignore_case`) are called but undeclared. |
| Sorting (`SHOW ID/MARK ASC|DESC`) | Sort records by ID or mark in both orders | `summary.h` offers `cms_sort_by_id/mark` using `SortOrder` | Sorting APIs exist but depend on `SortOrder` enum (duplicate of `CmsSortOrder`). No header exposes `cms_show_all_sorted`, although `cmd_show` calls it. Summary math not wired into command layer. |
| Statistics (`SHOW SUMMARY`) | Display count/average/highest/lowest | `summary.h` defines `SummaryStats`, `cms_calculate_summary`, `cms_display_summary` | Statistics functions declared but `cms_calculate_summary` returns `CMS_STATUS_NOT_IMPLEMENTED`; `cmd_show` calls `cms_show_summary`, which is undefined. |
| Interactive CLI | Prompt-driven CLI with parsing, help, exit commands | `commands.h` exposes `cms_command_loop` and `cms_parse_command` | Loop + parser exist but parser always returns `CMS_STATUS_NOT_IMPLEMENTED`. No exit handling, status feedback, or dirty-check prompts described in README. |
| Error handling | Comprehensive status messages | `cms.h` status enum; `cms_status.c` maps statuses | Status mapping works, but `cms_print_declaration` stub remains unimplemented; no structured surfacing of status details back to user (e.g., in command loop). |

## TODO / Missing Implementation Inventory (`src/`)

### `src/main.c`
- `cms_database_init`, welcome banner, `cms_command_loop`, and cleanup are invoked, but there is no validation around return codes beyond initialization. Comments still marked “TODO” although calls exist; no declaration printing or default database loading on startup.

### `src/commands.c`
- `cmd_open` trims path via undefined `cms_trim` and relies on `cms_database_load`, which is unimplemented.
- `cmd_show` references `cms_trim`, `cms_string_equals_ignore_case`, `cms_show_summary`, `cms_show_all`, and `cms_show_all_sorted`; none are implemented or declared in headers.
- `cmd_insert`, `cmd_update`, and `cmd_delete` return `CMS_STATUS_NOT_IMPLEMENTED`; no prompts, validation, or confirmation logic.
- `cmd_query` only prints record via `cms_database_show_record`, but error messages from missing record aren’t surfaced to CLI.
- `cmd_save` bypasses input sanitization and depends on unimplemented save routine.
- `cmd_help` prints commands but doesn’t clarify syntax nuances (e.g., `SHOW ID ASC`), nor mention dirty-state warnings.
- `cms_command_loop` lacks exit condition, error reporting, dirty flag checks, or default command on empty input.
- `cms_parse_command` entirely unimplemented: no tokenization, case normalization, or delegation.

### `src/database.c`
- `cms_database_init/cleanup` work, but they don’t allocate the initial capacity or set defaults beyond zeroing state.
- `cms_database_load`/`save` return `CMS_STATUS_NOT_IMPLEMENTED`; no file parsing, validation, or error propagation, explaining invalid-file handling gaps.
- `cms_database_insert`/`update`/`delete` only validate pointers; they neither enforce unique IDs nor resize backing storage.
- `cms_database_query` always returns `CMS_STATUS_NOT_FOUND`; simple linear search not yet written.
- `cms_database_show_all` lacks printing implementation (currently returns `CMS_STATUS_NOT_IMPLEMENTED`).

### `src/summary.c`
- Comparators mostly implemented, but `cms_sort_by_id` changes the live database order without preserving previous ordering; no `is_dirty` update after sorting.
- `cms_calculate_summary` returns `CMS_STATUS_NOT_IMPLEMENTED`; `cms_display_summary` prints using an uninitialized `SummaryStats`.

### `src/utils.c`
- `cms_validate_student_id` only checks for positive numbers; README expects stricter (likely 7-digit) format.
- `cms_validate_name`/`programme` do not trim whitespace or reject numeric characters.
- `cms_trim_string` is blank; commands use a non-existent `cms_trim`.
- No helper for case-insensitive compares despite usage.
- Input functions (`cms_read_int/float/string`) mix `scanf` with `fgets`, which can desynchronize CLI prompts; no validation loops or error messaging.

### `src/cms_status.c`
- `cms_status_message` is complete, but `cms_print_status` only prints errors to stderr without context text.
- `cms_print_declaration` remains unimplemented despite README’s “interactive CLI” mention; no banner function to match.

## Capability Gap Summary

1. **File handling is non-functional**: Without `cms_database_load/save`, the system cannot honor README promises like `OPEN`, `SAVE`, or default database recovery, leading directly to the “invalid file” issue.
2. **Command routing missing**: `cms_parse_command` and most `cmd_*` handlers lack logic, so none of the documented CLI commands operate beyond placeholder responses.
3. **Data validation incomplete**: Utilities neither trim nor rigorously validate inputs, making it impossible to guarantee clean records as the README claims.
4. **Display and analytics absent**: `SHOW ALL`, sorting, and summary statistics depend on unimplemented helpers, so even after loading data there is no way to view it.
5. **Advanced feature hooks undefined**: Enhancements like a B-tree index would require well-defined insert/query/update flows that currently do not exist.

Addressing these gaps will require implementing the missing database I/O pipeline first, then layering command parsing, validation, and display logic to align the codebase with the advertised features.

