# Milestone 2 – Validation & Utilities Review

Goal: audit `src/utils.c` for missing trimming, casing, and input-safety logic, then outline concrete fixes that unblock reliable command handling (especially `OPEN`).

## Current Gaps

| Area | Observation | Impact |
|------|-------------|--------|
| Trimming | `cms_trim_string` is an empty stub, yet `cmd_open` calls an undefined `cms_trim` | File paths from `OPEN` are never stripped of spaces or control chars, so `" Sample-CMS.txt "` fails and can propagate hidden characters into `cms_database_load`, triggering the “invalid file” issue. |
| Case-insensitive compares | `cmd_show` uses `cms_string_equals_ignore_case`, which isn’t implemented or declared | CLI cannot normalize user input, forcing exact casing and causing link errors when compiled. |
| Mixed input APIs | `cms_read_line` uses `fgets`, while `cms_read_int`/`cms_read_float` use `scanf` + manual buffer flushing | Alternating these functions desynchronizes stdin (newline leakage), so prompts may be skipped. Numeric readers also lack validation loops and error messaging. |
| Validation helpers | `cms_validate_student_id` only checks `id > 0`; name/programme validators don’t trim or reject control characters | Duplicate IDs slip through, and whitespace-only names pass validation, contradicting README promises. |
| Buffer hygiene | `cms_read_string` simply prints a prompt and calls `cms_read_line`, but there is no guarantee buffers are zeroed or truncated gracefully | Potential for trailing garbage when reusing buffers for filenames or programmes. |

## Proposed Fixes

### 1. Robust trimming + safe alias

Implement `cms_trim_string` and optionally expose a thin alias `cms_trim` to match existing callers:

```
35:76:src/utils.c
void cms_trim_string(char *str) {
    if (str == NULL) return;
    char *start = str;
    while (*start && isspace((unsigned char)*start)) start++;

    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) end--;
    *end = '\0';

    if (start != str) memmove(str, start, end - start + 1);
}
```

- Ensures paths like `" Sample-CMS.txt "` and `"Sample-CMS.txt\r\n"` become a clean filename before `cms_database_load`.
- Prevents zero-length paths caused by whitespace-only input.
- Supports trimming for names/programmes before validation.

### 2. Case-insensitive comparison helper

Provide a reusable utility:

```
78:98:src/utils.c
bool cms_string_equals_ignore_case(const char *a, const char *b) {
    if (!a || !b) return false;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
        a++; b++;
    }
    return *a == *b;
}
```

- Allows `cmd_show`, `cms_parse_command`, and future command handlers to accept lowercase inputs, matching README’s “user-friendly CLI” claim.

### 3. Buffered numeric parsing

Replace `scanf`-based readers with line-buffered parsing to avoid stdin desync:

```
100:150:src/utils.c
static bool cms_parse_long(const char *input, long *out) {
    char *end = NULL;
    errno = 0;
    long value = strtol(input, &end, 10);
    if (errno != 0 || input == end || *end != '\0') return false;
    *out = value;
    return true;
}

bool cms_read_int(const char *prompt, int *value) {
    char buffer[CMS_MAX_COMMAND_LEN];
    while (true) {
        printf("%s", prompt);
        if (!cms_read_line(buffer, sizeof(buffer))) return false;
        cms_trim_string(buffer);
        long tmp;
        if (cms_parse_long(buffer, &tmp) && tmp >= INT_MIN && tmp <= INT_MAX) {
            *value = (int)tmp;
            return true;
        }
        printf("Invalid integer. Please try again.\n");
    }
}
```

- Guarantees prompts are shown consistently.
- Enables re-prompts on invalid input, reducing accidental blank IDs that could corrupt the database or lead to invalid filenames when used as part of a path.

Analogous logic using `strtof` should power `cms_read_float`.

### 4. Strengthen validation helpers

- **Student ID**: Enforce 7-digit numeric IDs via `cms_read_int` and `cms_validate_student_id` range checks (`1000000–9999999`) and uniqueness verification inside `cms_database_insert`.
- **Name/Programme**: Trim first, ensure UTF-8 printable characters, and reject strings containing tabs/newlines.

These checks stop malformed filenames like `" ../TeamName-CMS.txt"` from slipping through when users mistakenly paste paths into prompts.

### 5. Sanitized filename helper for `OPEN`

Add a dedicated function:

```
bool cms_sanitize_file_path(char *path) {
    cms_trim_string(path);
    if (path[0] == '\0') return false;
    for (char *p = path; *p; ++p) {
        if (*p == '\t' || *p == '\n') return false;
    }
    return true;
}
```

`cmd_open` can call this before `cms_database_load`, failing fast with a clear error when the path is empty or contains control characters, eliminating the current “invalid file” crash loop.

## Impact on `OPEN` Invalid File Bug

1. **Whitespace removal** ensures filenames read from CLI don’t contain stray `\r`/`\t`, which currently cause `fopen` to fail even when the visible name appears correct.
2. **Input re-prompting** prevents `cmd_open` from receiving empty strings after a failed `scanf`, removing a major cause of `CMS_STATUS_INVALID_ARGUMENT`.
3. **Sanitization helper** centralizes checks so other commands (`SAVE`, future `IMPORT`) cannot pass unsafe paths to the database layer.

Together, these utilities establish a reliable foundation for file operations and user input, enabling subsequent milestones (command parsing, database I/O, advanced structures like a B-tree index) to assume clean, validated data.

