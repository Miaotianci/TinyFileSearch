#pragma once

#include "FileScanner.h"
#include "FileIndex.h"
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

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
    void setThreadCount(size_t count);
    void enableIndex(bool enable);
    void buildIndex();

    std::vector<FileInfo> search(const std::string& query);
    std::vector<FileInfo> searchByName(const std::string& name_pattern);
    std::vector<FileInfo> searchByExtension(const std::string& extension);
    std::vector<FileInfo> searchBySize(uint64_t min_size, uint64_t max_size);

    size_t getIndexedFileCount() const;

private:
    template<typename Func>
    std::vector<FileInfo> parallelSearch(Func&& matchFunc);

    bool matchName(const std::string& name, const std::string& pattern) const;

    std::weak_ptr<std::vector<FileInfo>> file_data_;
    std::unique_ptr<FileIndex> index_;
    SearchOption search_option_ = SearchOption::CaseInsensitive;
    size_t thread_count_ = std::thread::hardware_concurrency();
    bool use_index_ = true;
};

}
