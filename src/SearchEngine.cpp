#include "SearchEngine.h"
#include <algorithm>
#include <cctype>
#include <future>

namespace TinyFileSearch {

SearchEngine::SearchEngine() : index_(std::make_unique<FileIndex>()) {}

SearchEngine::~SearchEngine() = default;

void SearchEngine::setFileData(std::shared_ptr<std::vector<FileInfo>> files) {
    file_data_ = files;
    if (use_index_ && files) {
        index_->build(files);
    }
}

void SearchEngine::setSearchOption(SearchOption option) {
    search_option_ = option;
}

void SearchEngine::setThreadCount(size_t count) {
    thread_count_ = count > 0 ? count : 1;
}

void SearchEngine::enableIndex(bool enable) {
    use_index_ = enable;
    if (enable) {
        auto files = file_data_.lock();
        if (files) {
            index_->build(files);
        }
    }
}

void SearchEngine::buildIndex() {
    auto files = file_data_.lock();
    if (files) {
        index_->build(files);
    }
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool SearchEngine::matchName(const std::string& name, const std::string& pattern) const {
    if (search_option_ == SearchOption::CaseSensitive) {
        return name.find(pattern) != std::string::npos;
    } else {
        std::string lower_name = toLower(name);
        std::string lower_pattern = toLower(pattern);
        return lower_name.find(lower_pattern) != std::string::npos;
    }
}

template<typename Func>
std::vector<FileInfo> SearchEngine::parallelSearch(Func&& matchFunc) {
    auto files = file_data_.lock();
    if (!files || files->empty()) {
        return {};
    }

    size_t file_count = files->size();
    size_t num_threads = std::min(thread_count_, file_count);
    size_t chunk_size = (file_count + num_threads - 1) / num_threads;

    std::vector<std::future<std::vector<FileInfo>>> futures;
    futures.reserve(num_threads);

    for (size_t t = 0; t < num_threads; ++t) {
        size_t start = t * chunk_size;
        size_t end = std::min(start + chunk_size, file_count);

        futures.emplace_back(std::async(std::launch::async, [&files, &matchFunc, start, end]() {
            std::vector<FileInfo> local_results;
            for (size_t i = start; i < end; ++i) {
                if (matchFunc((*files)[i])) {
                    local_results.push_back((*files)[i]);
                }
            }
            return local_results;
        }));
    }

    std::vector<FileInfo> results;
    for (auto& future : futures) {
        auto partial = future.get();
        results.insert(results.end(), partial.begin(), partial.end());
    }

    return results;
}

std::vector<FileInfo> SearchEngine::search(const std::string& query) {
    if (use_index_ && index_->getIndexedCount() > 0) {
        return index_->search(query);
    }
    return searchByName(query);
}

std::vector<FileInfo> SearchEngine::searchByName(const std::string& name_pattern) {
    if (use_index_ && index_->getIndexedCount() > 0) {
        return index_->search(name_pattern);
    }
    return parallelSearch([this, &name_pattern](const FileInfo& file) {
        return matchName(file.name, name_pattern);
    });
}

std::vector<FileInfo> SearchEngine::searchByExtension(const std::string& extension) {
    if (use_index_ && index_->getIndexedCount() > 0) {
        return index_->searchByExtension(extension);
    }

    std::string ext = extension;
    if (!ext.empty() && ext[0] != '.') {
        ext = "." + ext;
    }

    if (search_option_ == SearchOption::CaseInsensitive) {
        ext = toLower(ext);
    }

    return parallelSearch([this, &ext](const FileInfo& file) {
        std::string file_ext = file.extension;
        if (search_option_ == SearchOption::CaseInsensitive) {
            file_ext = toLower(file_ext);
        }
        return file_ext == ext;
    });
}

std::vector<FileInfo> SearchEngine::searchBySize(uint64_t min_size, uint64_t max_size) {
    return parallelSearch([min_size, max_size](const FileInfo& file) {
        return file.size >= min_size && file.size <= max_size;
    });
}

size_t SearchEngine::getIndexedFileCount() const {
    return index_->getIndexedCount();
}

}
