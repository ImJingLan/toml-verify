# TOML File Validator

A TOML file format validator written in C++, compiled with g++.

## Features

- Validates TOML file format correctness
- Supports drag-and-drop file to executable
- Provides detailed error location information
- Supports various TOML data types:
  - Strings
  - Integers
  - Floats
  - Booleans
  - Date-time
  - Arrays
  - Tables
  - Inline tables

## Compilation

### Method 1: Using g++ command
```bash
g++ -std=c++17 -Wall -Wextra -o toml-verify.exe toml_verify_simple.cpp
```

### Method 2: Using Makefile
```bash
make
```

### Method 3: Using build.bat (Windows)
Double-click the `build.bat` file

## Usage

### Method 1: Drag and Drop
Drag and drop a TOML file onto `toml-verify.exe` to automatically validate it.

### Method 2: Command Line
```bash
toml-verify.exe <filepath>
```

## Examples

### Validating a valid TOML file
```bash
toml-verify.exe example_valid.toml
```

Output:
```
========================================
Validation Successful!
File: example_valid.toml
========================================
TOML file format is correct

Press any key to exit...
```

### Validating an invalid TOML file
```bash
toml-verify.exe example_invalid.toml
```

Output:
```
========================================
Validation Failed!
File: example_invalid.toml
========================================
Error: TOML syntax error
Location: Line X, Column Y

Press any key to exit...
```

## Test Files

The project includes the following test files:
- `example_valid.toml` - Valid TOML file example
- `example_invalid.toml` - Invalid TOML file example
- `test_simple.toml` - Simple TOML file test
- `test_table.toml` - TOML file with tables test

## Technical Details

- Uses C++17 standard
- Implements complete lexical analyzer and syntax validator
- Supports basic TOML 1.0 standard syntax
- Zero external dependencies, only C++ standard library

## Clean Up

### Using make
```bash
make clean
```

### Manual deletion
```bash
del toml-verify.exe
```

## Notes

- The validator mainly checks syntax format and does not perform semantic validation
- Some advanced TOML features may not be fully supported
- For complex TOML files, it is recommended to use a dedicated TOML parsing library for complete validation

## License

This project is for learning and reference purposes only.
