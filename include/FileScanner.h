#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include <functional>

namespace TinyFileSearch {

struct FileInfo {
    std::string path;
    std::string name;
    std::string extension;
    uint64_t size;
    std::filesystem::file_time_type modified_time;
};

class FileScanner {
public:
    using ProgressCallback = std::function<void(size_t scanned_count)>;

    FileScanner() = default;
    ~FileScanner() = default;

    void setRootPath(const std::string& path);
    void setIncludeHidden(bool include);
    void setMaxDepth(int depth);

    bool scan();
    void stop();

    const std::vector<FileInfo>& getFiles() const;
    size_t getFileCount() const;
    void setProgressCallback(ProgressCallback callback);

private:
    std::string root_path_;
    bool include_hidden_ = false;
    int max_depth_ = -1;
    std::vector<FileInfo> files_;
    bool stop_requested_ = false;
    ProgressCallback progress_callback_;
};

}
