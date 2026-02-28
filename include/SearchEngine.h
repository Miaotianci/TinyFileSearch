#pragma once

#include "FileScanner.h"
#include <string>
#include <vector>
#include <memory>

namespace TinyFileSearch {

enum class SearchOption {
    CaseSensitive,
    CaseInsensitive,
    Regex,
    Fuzzy
};

class SearchEngine {
public:
    SearchEngine();
    ~SearchEngine();

    void setFileData(std::shared_ptr<std::vector<FileInfo>> files);
    void setSearchOption(SearchOption option);

    std::vector<FileInfo> search(const std::string& query);
    std::vector<FileInfo> searchByName(const std::string& name_pattern);
    std::vector<FileInfo> searchByExtension(const std::string& extension);
    std::vector<FileInfo> searchBySize(uint64_t min_size, uint64_t max_size);

    size_t getIndexedFileCount() const;

private:
    std::weak_ptr<std::vector<FileInfo>> file_data_;
    SearchOption search_option_ = SearchOption::CaseInsensitive;
};

}
