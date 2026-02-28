#include "FileScanner.h"
#include <filesystem>
#include <algorithm>

namespace TinyFileSearch {

void FileScanner::setRootPath(const std::string& path) {
    root_path_ = path;
}

void FileScanner::setIncludeHidden(bool include) {
    include_hidden_ = include;
}

void FileScanner::setMaxDepth(int depth) {
    max_depth_ = depth;
}

void FileScanner::setProgressCallback(ProgressCallback callback) {
    progress_callback_ = std::move(callback);
}

bool FileScanner::scan() {
    files_.clear();
    stop_requested_ = false;

    try {
        std::filesystem::path root(root_path_);
        if (!std::filesystem::exists(root)) {
            return false;
        }

        auto visitor = [this](const std::filesystem::path& path) {
            if (stop_requested_) {
                return;
            }

            if (!std::filesystem::is_directory(path)) {
                FileInfo info;
                info.path = path.string();
                info.name = path.filename().string();
                info.extension = path.extension().string();

                try {
                    auto status = std::filesystem::status(path);
                    info.size = std::filesystem::is_regular_file(status) 
                        ? std::filesystem::file_size(path) : 0;
                    info.modified_time = std::filesystem::last_write_time(path);
                } catch (...) {
                    info.size = 0;
                }

                if (!info.name.empty() && (include_hidden_ || info.name[0] != '.')) {
                    files_.push_back(info);
                    if (progress_callback_) {
                        progress_callback_(files_.size());
                    }
                }
            }
        };

        if (max_depth_ > 0) {
            for (auto it = std::filesystem::recursive_directory_iterator(root); 
                 it != std::filesystem::recursive_directory_iterator(); 
                 ++it) {
                if (stop_requested_) break;
                if (it.depth() > max_depth_) {
                    it.disable_recursion_pending();
                    continue;
                }
                visitor(it->path());
            }
        } else {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
                if (stop_requested_) break;
                visitor(entry.path());
            }
        }

    } catch (const std::exception&) {
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
