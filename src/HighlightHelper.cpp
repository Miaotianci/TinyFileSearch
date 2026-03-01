#include "HighlightHelper.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace TinyFileSearch {

bool HighlightHelper::color_enabled_ = false;

// ANSI color codes
const char* HighlightHelper::RED = "\033[31m";
const char* HighlightHelper::GREEN = "\033[32m";
const char* HighlightHelper::YELLOW = "\033[33m";
const char* HighlightHelper::BLUE = "\033[34m";
const char* HighlightHelper::CYAN = "\033[36m";
const char* HighlightHelper::RESET = "\033[0m";

void HighlightHelper::enableColor() {
    if (color_enabled_) {
        return;
    }

#ifdef _WIN32
    // Enable ANSI escape sequences on Windows 10+
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
            color_enabled_ = true;
        }
    }
#else
    color_enabled_ = true;
#endif
}

std::string HighlightHelper::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

size_t HighlightHelper::findIgnoreCase(const std::string& text, const std::string& pattern, size_t pos) {
    if (pattern.empty()) {
        return std::string::npos;
    }

    std::string lower_text = toLower(text.substr(pos));
    std::string lower_pattern = toLower(pattern);

    size_t found = lower_text.find(lower_pattern);
    if (found != std::string::npos) {
        return found + pos;
    }
    return std::string::npos;
}

std::string HighlightHelper::highlight(const std::string& text, const std::string& query, bool case_sensitive) {
    if (!color_enabled_ || query.empty()) {
        return text;
    }

    std::string result;
    std::string highlight_text = text;
    std::string search_query = query;
    std::string search_text = text;

    // Convert to lowercase for case-insensitive search if needed
    std::string lower_query;
    std::string lower_text;
    if (!case_sensitive) {
        lower_query = toLower(search_query);
        lower_text = toLower(search_text);
    }

    size_t pos = 0;
    while (pos < search_text.length()) {
        size_t found_pos;
        if (case_sensitive) {
            found_pos = search_text.find(search_query, pos);
        } else {
            found_pos = lower_text.find(lower_query, pos);
        }

        if (found_pos == std::string::npos) {
            // Add remaining text
            result.append(highlight_text.substr(pos));
            break;
        }

        // Add text before match
        result.append(highlight_text.substr(pos, found_pos - pos));

        // Add highlighted match
        result.append(YELLOW);
        result.append(highlight_text.substr(found_pos, search_query.length()));
        result.append(RESET);

        // Move past the match
        pos = found_pos + search_query.length();
    }

    return result;
}

} // namespace TinyFileSearch
