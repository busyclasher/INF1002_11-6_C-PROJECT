# Student Records CMS (Content Management System)

A command-line based Content Management System for managing student records written in C. This application allows users to store, query, update, and manage student information including ID, name, programme, and marks.

## Features

- **Database Management**: Load and save student records from/to text files
- **CRUD Operations**: Create, Read, Update, and Delete student records
- **Data Validation**: Input validation for student IDs, names, programmes, and marks (0-100)
- **Sorting**: Sort records by ID or marks in ascending/descending order
- **Statistics**: Display summary statistics (count, average, highest, lowest marks)
- **Interactive CLI**: User-friendly command-line interface with prompts
- **Error Handling**: Comprehensive error handling with status messages

## Project Structure

```
INF1002_11-6_C-PROJECT-1/
├── include/              # Header files
│   ├── cms.h            # Core CMS types and status codes
│   ├── commands.h       # Command processing interface
│   ├── config.h         # Configuration constants
│   ├── database.h       # Database structure and operations
│   ├── summary.h        # Sorting and summary functions
│   └── utils.h          # Utility functions
├── src/                 # Source files
│   ├── cms_status.c     # Status message handling
│   ├── commands.c       # Command handlers and CLI loop
│   ├── database.c       # Database operations implementation
│   ├── main.c           # Application entry point
│   ├── summary.c        # Sorting and statistics
│   └── utils.c          # Utility functions
├── Sample-CMS.txt       # Sample database file
├── TeamName-CMS.txt     # Default database file
└── README.md            # This file
```

## Building the Project

### Prerequisites
- GCC compiler or compatible C compiler
- Windows, Linux, or macOS

### Compilation

Using GCC:
```bash
gcc -I./include -o cms.exe src/*.c
```

Or compile individual files:
```bash
gcc -I./include -c src/main.c -o build/main.o
gcc -I./include -c src/database.c -o build/database.o
gcc -I./include -c src/commands.c -o build/commands.o
gcc -I./include -c src/summary.c -o build/summary.o
gcc -I./include -c src/utils.c -o build/utils.o
gcc -I./include -c src/cms_status.c -o build/cms_status.o
gcc -o cms.exe build/*.o
```

## Usage

### Starting the Application

Run the compiled executable:
```bash
./cms.exe
```

The application will start with the `CMS>` prompt, ready to accept commands.

### Available Commands

| Command | Syntax | Description |
|---------|--------|-------------|
| **OPEN** | `OPEN <filename>` | Load a database file |
| **SHOW** | `SHOW [ALL\|SUMMARY\|ID\|MARK] [ASC\|DESC]` | Display records or statistics |
| **INSERT** | `INSERT` | Add a new student record (interactive) |
| **QUERY** | `QUERY <student_id>` | Find and display a specific record |
| **UPDATE** | `UPDATE <student_id>` | Modify an existing record (interactive) |
| **DELETE** | `DELETE <student_id>` | Remove a student record |
| **SAVE** | `SAVE [filename]` | Save changes to file |
| **HELP** | `HELP` | Display help information |
| **EXIT/QUIT** | `EXIT` or `QUIT` | Exit the application |

### Command Examples

#### Opening a Database
```
CMS> OPEN Sample-CMS.txt
```

#### Displaying All Records
```
CMS> SHOW ALL
```

#### Displaying Records Sorted by Mark (Descending)
```
CMS> SHOW MARK DESC
```

#### Displaying Summary Statistics
```
CMS> SHOW SUMMARY
```

#### Inserting a New Record
```
CMS> INSERT
Enter student ID: 2401234
Enter name: Michelle Lee
Enter programme: Information Systems
Enter mark (0-100): 79.5
```

#### Querying a Specific Record
```
CMS> QUERY 2301234
```

#### Updating a Record
```
CMS> UPDATE 2301234
Current record: ID=2301234, Name=Joshua Chen, Programme=Software Engineering, Mark=70.50
Enter name (or press Enter to keep current): 
Enter programme (or press Enter to keep current): 
Enter mark (or press Enter to keep current): 75.0
```

#### Deleting a Record
```
CMS> DELETE 2201234
```

#### Saving Changes
```
CMS> SAVE
```
or save to a different file:
```
CMS> SAVE NewFile-CMS.txt
```

## Database File Format

The database files use a simple tab-separated text format:

```
Table Name: StudentRecords
ID      Name            Programme               Mark
2301234 Joshua Chen     Software Engineering    70.5
2201234 Isaac Teo       Computer Science        63.4
2304567 John Levoy      Digital Supply Chain    85.9
```

### Format Specifications:
- First line: `Table Name: StudentRecords`
- Second line: Column headers (ID, Name, Programme, Mark)
- Following lines: Tab-separated student records
- Marks are floating-point values between 0.0 and 100.0

## Configuration

Key configuration constants defined in `config.h`:

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_MAX_NAME_LEN` | 64 | Maximum length for student names |
| `CMS_MAX_PROGRAMME_LEN` | 64 | Maximum length for programme names |
| `CMS_MAX_COMMAND_LEN` | 256 | Maximum command input length |
| `CMS_MAX_FILE_PATH_LEN` | 260 | Maximum file path length |
| `CMS_MIN_MARK` | 0.0 | Minimum valid mark |
| `CMS_MAX_MARK` | 100.0 | Maximum valid mark |
| `CMS_DEFAULT_DATABASE_FILE` | "TeamName-CMS.txt" | Default database filename |

## Error Handling

The system uses a comprehensive status code system:

- `CMS_STATUS_OK` - Operation successful
- `CMS_STATUS_ERROR` - General error
- `CMS_STATUS_INVALID_ARGUMENT` - Invalid input parameters
- `CMS_STATUS_IO` - File I/O error
- `CMS_STATUS_PARSE_ERROR` - Database parsing error
- `CMS_STATUS_NOT_FOUND` - Record not found
- `CMS_STATUS_DUPLICATE` - Duplicate student ID
- `CMS_STATUS_NOT_IMPLEMENTED` - Feature not yet implemented

## Data Structures

### StudentRecord
```c
typedef struct StudentRecord {
    int id;
    char name[CMS_MAX_NAME_LEN + 1];
    char programme[CMS_MAX_PROGRAMME_LEN + 1];
    float mark;
} StudentRecord;
```

### StudentDatabase
```c
typedef struct StudentDatabase {
    StudentRecord *records;
    size_t count;
    size_t capacity;
    char file_path[CMS_MAX_FILE_PATH_LEN];
    bool is_loaded;
    bool is_dirty;
} StudentDatabase;
```

## Development Notes

- The database uses dynamic memory allocation for storing records
- Records are automatically resized as needed
- The system tracks unsaved changes with the `is_dirty` flag
- All string operations include bounds checking
- Input validation prevents invalid data entry

## Known Limitations

- The `cms_print_declaration()` function is not yet implemented (returns `CMS_STATUS_NOT_IMPLEMENTED`)
- Database file must follow the exact tab-separated format
- Student IDs must be unique integers
- Maximum field lengths are enforced (names, programmes)

## Author

INF1002_11-6 Team
Based on sample by Assistant Prof Oran Zane Devilly

## License

This is an educational project for INF1002 coursework.

---

For bug reports or feature requests, please contact the development team.

