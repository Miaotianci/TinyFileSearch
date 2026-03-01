#pragma once

#include "FileScanner.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>

namespace TinyFileSearch {

class FileIndex {
public:
    FileIndex();
    ~FileIndex();

    void build(std::shared_ptr<std::vector<FileInfo>> files);
    void clear();

    std::vector<FileInfo> search(const std::string& query);
    std::vector<FileInfo> searchByExtension(const std::string& extension);

    size_t getIndexedCount() const;

private:
    struct TrieNode {
        std::unordered_map<char, std::unique_ptr<TrieNode>> children;
        std::vector<size_t> file_indices;
    };

    void insertName(const std::string& name, size_t index);
    void insertExtension(const std::string& ext, size_t index);
    std::vector<size_t> searchPrefix(const std::string& prefix);

    std::unique_ptr<TrieNode> name_root_;
    std::unique_ptr<TrieNode> ext_root_;
    std::weak_ptr<std::vector<FileInfo>> file_data_;
    size_t indexed_count_ = 0;
    std::mutex index_mutex_;
};

}
