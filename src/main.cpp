#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include "FileScanner.h"
#include "SearchEngine.h"
#include "HighlightHelper.h"

using namespace TinyFileSearch;

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options] <search_query>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -p <path>    Set root path to scan (default: current directory)\n";
    std::cout << "  -h           Include hidden files\n";
    std::cout << "  -d <depth>   Set maximum scan depth\n";
    std::cout << "  --help       Show this help message\n";
}

int main(int argc, char* argv[]) {
    // Enable console colors
    HighlightHelper::enableColor();

    std::string root_path = ".";
    std::string search_query;
    bool include_hidden = false;
    int max_depth = -1;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-?") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-p" && i + 1 < argc) {
            root_path = argv[++i];
        } else if (arg == "-h") {
            include_hidden = true;
        } else if (arg == "-d" && i + 1 < argc) {
            max_depth = std::stoi(argv[++i]);
        } else {
            search_query = arg;
        }
    }

    if (search_query.empty()) {
        printUsage(argv[0]);
        return 1;
    }

    auto file_data = std::make_shared<std::vector<FileInfo>>();

    FileScanner scanner;
    scanner.setRootPath(root_path);
    scanner.setIncludeHidden(include_hidden);
    if (max_depth > 0) {
        scanner.setMaxDepth(max_depth);
    }

    scanner.setProgressCallback([](size_t count) {
        if (count % 1000 == 0) {
            std::cout << "\rScanned: " << count << " files..." << std::flush;
        }
    });

    std::cout << "Scanning directory: " << root_path << "\n";
    auto start_time = std::chrono::high_resolution_clock::now();

    if (!scanner.scan()) {
        std::cerr << "Error: Failed to scan directory\n";
        return 1;
    }

    auto scan_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start_time
    );

    *file_data = scanner.getFiles();
    std::cout << "\rScanned: " << file_data->size() << " files in "
              << scan_duration.count() << "ms\n";

    SearchEngine engine;
    engine.setFileData(file_data);

    auto search_start = std::chrono::high_resolution_clock::now();
    auto results = engine.search(search_query);
    auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - search_start
    );

    std::cout << "Found " << results.size() << " matches in "
              << search_duration.count() << "ms\n\n";

    for (const auto& file : results) {
        std::cout << HighlightHelper::highlight(file.path, search_query, false) << "\n";
    }

    return 0;
}
