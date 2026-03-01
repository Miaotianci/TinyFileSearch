#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <memory>

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

    FileScanner();
    ~FileScanner();

    void setRootPath(const std::string& path);
    void setIncludeHidden(bool include);
    void setMaxDepth(int depth);
    void setThreadCount(size_t count);

    bool scan();
    void stop();

    const std::vector<FileInfo>& getFiles() const;
    size_t getFileCount() const;
    void setProgressCallback(ProgressCallback callback);

private:
    void workerThread();
    bool processDirectory(const std::filesystem::path& dir_path, int depth);

    std::string root_path_;
    bool include_hidden_ = false;
    int max_depth_ = -1;
    size_t thread_count_ = std::thread::hardware_concurrency();
    
    std::vector<FileInfo> files_;
    std::atomic<bool> stop_requested_{false};
    std::atomic<size_t> scanned_count_{0};
    
    std::queue<std::pair<std::filesystem::path, int>> dir_queue_;
    std::mutex queue_mutex_;
    std::mutex result_mutex_;
    
    ProgressCallback progress_callback_;
};

}
