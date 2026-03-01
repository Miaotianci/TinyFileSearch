#include "FileIndex.h"
#include <algorithm>
#include <cctype>

namespace TinyFileSearch {

FileIndex::FileIndex() {
    name_root_ = std::make_unique<TrieNode>();
    ext_root_ = std::make_unique<TrieNode>();
}

FileIndex::~FileIndex() = default;

void FileIndex::build(std::shared_ptr<std::vector<FileInfo>> files) {
    std::lock_guard<std::mutex> lock(index_mutex_);
    
    name_root_ = std::make_unique<TrieNode>();
    ext_root_ = std::make_unique<TrieNode>();
    file_data_ = files;
    indexed_count_ = 0;

    if (!files || files->empty()) {
        return;
    }

    for (size_t i = 0; i < files->size(); ++i) {
        const auto& file = (*files)[i];
        
        std::string lower_name = file.name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        insertName(lower_name, i);

        std::string lower_ext = file.extension;
        if (!lower_ext.empty() && lower_ext[0] == '.') {
            lower_ext = lower_ext.substr(1);
        }
        std::transform(lower_ext.begin(), lower_ext.end(), lower_ext.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        if (!lower_ext.empty()) {
            insertExtension(lower_ext, i);
        }
    }

    indexed_count_ = files->size();
}

void FileIndex::clear() {
    std::lock_guard<std::mutex> lock(index_mutex_);
    name_root_ = std::make_unique<TrieNode>();
    ext_root_ = std::make_unique<TrieNode>();
    indexed_count_ = 0;
}

void FileIndex::insertName(const std::string& name, size_t index) {
    TrieNode* node = name_root_.get();
    for (char c : name) {
        if (node->children.find(c) == node->children.end()) {
            node->children[c] = std::make_unique<TrieNode>();
        }
        node = node->children[c].get();
        node->file_indices.push_back(index);
    }
}

void FileIndex::insertExtension(const std::string& ext, size_t index) {
    TrieNode* node = ext_root_.get();
    for (char c : ext) {
        if (node->children.find(c) == node->children.end()) {
            node->children[c] = std::make_unique<TrieNode>();
        }
        node = node->children[c].get();
        node->file_indices.push_back(index);
    }
}

std::vector<size_t> FileIndex::searchPrefix(const std::string& prefix) {
    std::string lower_prefix = prefix;
    std::transform(lower_prefix.begin(), lower_prefix.end(), lower_prefix.begin(),
                  [](unsigned char c) { return std::tolower(c); });

    TrieNode* node = name_root_.get();
    for (char c : lower_prefix) {
        if (node->children.find(c) == node->children.end()) {
            return {};
        }
        node = node->children[c].get();
    }
    return node->file_indices;
}

std::vector<FileInfo> FileIndex::search(const std::string& query) {
    auto files = file_data_.lock();
    if (!files || files->empty()) {
        return {};
    }

    std::vector<size_t> indices = searchPrefix(query);

    std::vector<FileInfo> results;
    results.reserve(indices.size());
    for (size_t idx : indices) {
        if (idx < files->size()) {
            results.push_back((*files)[idx]);
        }
    }

    return results;
}

std::vector<FileInfo> FileIndex::searchByExtension(const std::string& extension) {
    auto files = file_data_.lock();
    if (!files || files->empty()) {
        return {};
    }

    std::string ext = extension;
    if (!ext.empty() && ext[0] == '.') {
        ext = ext.substr(1);
    }
    std::transform(ext.begin(), ext.end(), ext.begin(),
                  [](unsigned char c) { return std::tolower(c); });

    TrieNode* node = ext_root_.get();
    for (char c : ext) {
        if (node->children.find(c) == node->children.end()) {
            return {};
        }
        node = node->children[c].get();
    }

    std::vector<FileInfo> results;
    for (size_t idx : node->file_indices) {
        if (idx < files->size()) {
            results.push_back((*files)[idx]);
        }
    }

    return results;
}

size_t FileIndex::getIndexedCount() const {
    return indexed_count_;
}

}
