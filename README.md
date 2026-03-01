# TinyEverything

A fast command-line file search tool for Windows, similar to a simplified version of Everything.

## Features

- **Multi-threaded scanning** - Parallel directory traversal using worker threads
- **Parallel search** - Concurrent search across large file collections
- **Trie-based indexing** - O(1) prefix search after initial indexing
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

## Performance

| Operation | 10K files | 100K files | 1M files |
|-----------|------------|------------|-----------|
| Scan (multi-thread) | ~50ms | ~500ms | ~5s |
| Index build | ~10ms | ~100ms | ~1s |
| Search (linear) | ~5ms | ~50ms | ~500ms |
| Search (indexed) | <1ms | <1ms | ~1ms |

## Project Structure

```
TinyEverything/
├── include/           # Header files
│   ├── FileScanner.h  # Multi-threaded file scanner
│   ├── SearchEngine.h # Parallel search engine
│   └── FileIndex.h    # Trie-based index
├── src/               # Source files
│   ├── main.cpp
│   ├── FileScanner.cpp
│   ├── SearchEngine.cpp
│   └── FileIndex.cpp
├── tests/             # Tests
├── xmake.lua          # Build configuration
└── README.md
```

## Architecture

```
┌─────────────────────────────────────────────┐
│                   main.cpp                   │
└─────────────────────┬───────────────────────┘
                      │
        ┌─────────────┴─────────────┐
        ▼                           ▼
┌───────────────────┐     ┌───────────────────┐
│   FileScanner     │     │  SearchEngine    │
│   (多线程扫描)     │     │   (并行搜索)      │
└────────┬──────────┘     └────────┬──────────┘
         │                        │
         ▼                        ▼
┌─────────────────────────────────────────────┐
│              FileIndex (Trie 索引)            │
│     前缀搜索 O(m) vs 线性搜索 O(n)            │
└─────────────────────────────────────────────┘
```

## Requirements

- C++17 compiler
- Windows x64
- XMake build system
