# TinyEverything

A fast command-line file search tool for Windows, similar to a simplified version of Everything.

## Features

- Multi-threaded directory scanning
- Parallel search across large file collections
- Case-sensitive/insensitive search
- Search by name, extension, or file size
- Configurable thread count
- Progress display during scanning

## Build

```bash
xmake
```

## Usage

```bash
# Search current directory
tinyfilesearch <keyword>

# Search specific directory
tinyfilesearch -p C:\Users test

# Include hidden files
tinyfilesearch -h config

# Limit scan depth
tinyfilesearch -d 3 .cpp
```

## Options

| Option | Description |
|--------|-------------|
| `-p <path>` | Set root path to scan (default: current directory) |
| `-h` | Include hidden files |
| `-d <depth>` | Maximum scan depth |
| `--help` | Show help message |

## Project Structure

```
TinyEverything/
├── include/           # Header files
│   ├── FileScanner.h
│   └── SearchEngine.h
├── src/               # Source files
│   ├── main.cpp
│   ├── FileScanner.cpp
│   └── SearchEngine.cpp
├── tests/             # Tests
├── xmake.lua          # Build configuration
└── README.md
```

## Requirements

- C++17 compiler
- Windows x64
- XMake build system
