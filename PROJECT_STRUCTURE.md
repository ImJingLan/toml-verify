# Project Structure

## Source Files
- `toml_verify.cpp` - Main source file containing the TOML validator implementation

## Build Files
- `Makefile` - Makefile for building the project on Windows/Linux
- `build.bat` - Windows batch script for building the project

## Executable
- `toml-verify.exe` - Compiled executable (generated after building)

## Test Files
- `example_valid.toml` - Valid TOML file example
- `example_invalid.toml` - Invalid TOML file example
- `test_simple.toml` - Simple TOML file test
- `test_table.toml` - TOML file with tables test
- `test_correct_comma.toml` - TOML file with correct comma usage
- `test_missing_comma.toml` - TOML file with missing comma (for testing error detection)
- `frpc.toml` - Real-world TOML configuration file

## Documentation
- `README.md` - Project documentation and usage instructions

## Configuration
- `.vscode/` - VS Code configuration files

## Building the Project

### Windows
```bash
build.bat
```

### Linux/Unix
```bash
make
```

### Manual
```bash
g++ -std=c++17 -Wall -Wextra -o toml-verify.exe toml_verify.cpp
```

## Features

- Complete TOML lexical analyzer
- Syntax validator with detailed error reporting
- Detects missing commas in arrays
- Provides exact line and column numbers for errors
- Drag-and-drop file support
- English language output
- Press any key to exit functionality

## Dependencies

- C++17 standard library only
- No external dependencies required
- Windows: Requires `conio.h` for `_getch()` function
