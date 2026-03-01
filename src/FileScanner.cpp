#include "FileScanner.h"
#include <algorithm>
#include <deque>

namespace TinyFileSearch {

FileScanner::FileScanner() = default;

FileScanner::~FileScanner() {
    stop();
}

void FileScanner::setRootPath(const std::string& path) {
    root_path_ = path;
}

void FileScanner::setIncludeHidden(bool include) {
    include_hidden_ = include;
}

void FileScanner::setMaxDepth(int depth) {
    max_depth_ = depth;
}

void FileScanner::setThreadCount(size_t count) {
    thread_count_ = count > 0 ? count : 1;
}

void FileScanner::setProgressCallback(ProgressCallback callback) {
    progress_callback_ = std::move(callback);
}

bool FileScanner::scan() {
    files_.clear();
    stop_requested_ = false;
    scanned_count_ = 0;

    std::filesystem::path root(root_path_);
    if (!std::filesystem::exists(root)) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        dir_queue_ = std::queue<std::pair<std::filesystem::path, int>>();
        dir_queue_.push({root, 0});
    }

    std::vector<std::thread> workers;
    workers.reserve(thread_count_);

    for (size_t i = 0; i < thread_count_; ++i) {
        workers.emplace_back(&FileScanner::workerThread, this);
    }

    for (auto& worker : workers) {
        worker.join();
    }

    return true;
}

void FileScanner::workerThread() {
    while (!stop_requested_) {
        std::filesystem::path dir_path;
        int depth;

        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (dir_queue_.empty()) {
                break;
            }
            dir_path = dir_queue_.front().first;
            depth = dir_queue_.front().second;
            dir_queue_.pop();
        }

        processDirectory(dir_path, depth);
    }
}

bool FileScanner::processDirectory(const std::filesystem::path& dir_path, int depth) {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (stop_requested_) {
                return false;
            }

            try {
                if (entry.is_directory()) {
                    if (!entry.path().filename().empty()) {
                        std::string name = entry.path().filename().string();
                        if (include_hidden_ || name[0] != '.') {
                            if (max_depth_ < 0 || depth < max_depth_) {
                                {
                                    std::lock_guard<std::mutex> lock(queue_mutex_);
                                    dir_queue_.push({entry.path(), depth + 1});
                                }
                            }
                        }
                    }
                } else if (entry.is_regular_file()) {
                    FileInfo info;
                    info.path = entry.path().string();
                    info.name = entry.path().filename().string();
                    info.extension = entry.path().extension().string();

                    try {
                        info.size = entry.file_size();
                        info.modified_time = entry.last_write_time();
                    } catch (...) {
                        info.size = 0;
                    }

                    if (!info.name.empty() && (include_hidden_ || info.name[0] != '.')) {
                        {
                            std::lock_guard<std::mutex> lock(result_mutex_);
                            files_.push_back(info);
                            ++scanned_count_;
                        }

                        if (progress_callback_) {
                            auto count = scanned_count_.load();
                            if (count % 1000 == 0) {
                                progress_callback_(count);
                            }
                        }
                    }
                }
            } catch (...) {
                continue;
            }
        }
    } catch (...) {
        return false;
    }

    return true;
}

void FileScanner::stop() {
    stop_requested_ = true;
}

const std::vector<FileInfo>& FileScanner::getFiles() const {
    return files_;
}

size_t FileScanner::getFileCount() const {
    return files_.size();
}

}
