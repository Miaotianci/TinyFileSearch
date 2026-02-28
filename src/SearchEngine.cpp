#include "SearchEngine.h"
#include <algorithm>
#include <cctype>

namespace TinyFileSearch {

SearchEngine::SearchEngine() = default;

SearchEngine::~SearchEngine() = default;

void SearchEngine::setFileData(std::shared_ptr<std::vector<FileInfo>> files) {
    file_data_ = files;
}

void SearchEngine::setSearchOption(SearchOption option) {
    search_option_ = option;
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::vector<FileInfo> SearchEngine::search(const std::string& query) {
    return searchByName(query);
}

std::vector<FileInfo> SearchEngine::searchByName(const std::string& name_pattern) {
    auto files = file_data_.lock();
    if (!files) return {};

    std::vector<FileInfo> results;
    std::string pattern = name_pattern;

    if (search_option_ == SearchOption::CaseInsensitive) {
        pattern = toLower(pattern);
    }

    for (const auto& file : *files) {
        std::string name = file.name;
        if (search_option_ == SearchOption::CaseInsensitive) {
            name = toLower(name);
        }

        if (name.find(pattern) != std::string::npos) {
            results.push_back(file);
        }
    }

    return results;
}

std::vector<FileInfo> SearchEngine::searchByExtension(const std::string& extension) {
    auto files = file_data_.lock();
    if (!files) return {};

    std::vector<FileInfo> results;
    std::string ext = extension;
    if (!ext.empty() && ext[0] != '.') {
        ext = "." + ext;
    }

    if (search_option_ == SearchOption::CaseInsensitive) {
        ext = toLower(ext);
    }

    for (const auto& file : *files) {
        std::string file_ext = file.extension;
        if (search_option_ == SearchOption::CaseInsensitive) {
            file_ext = toLower(file_ext);
        }

        if (file_ext == ext) {
            results.push_back(file);
        }
    }

    return results;
}

std::vector<FileInfo> SearchEngine::searchBySize(uint64_t min_size, uint64_t max_size) {
    auto files = file_data_.lock();
    if (!files) return {};

    std::vector<FileInfo> results;
    for (const auto& file : *files) {
        if (file.size >= min_size && file.size <= max_size) {
            results.push_back(file);
        }
    }

    return results;
}

size_t SearchEngine::getIndexedFileCount() const {
    auto files = file_data_.lock();
    return files ? files->size() : 0;
}

}
