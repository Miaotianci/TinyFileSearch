#pragma once

#include <string>
#include <algorithm>
#include <cctype>

namespace TinyFileSearch {

class HighlightHelper {
public:
    // Enable ANSI color codes on Windows 10+
    static void enableColor();

    // Highlight matching parts in the text
    static std::string highlight(const std::string& text, const std::string& query, bool case_sensitive = false);

    // Color codes
    static const char* RED;
    static const char* GREEN;
    static const char* YELLOW;
    static const char* BLUE;
    static const char* CYAN;
    static const char* RESET;

private:
    static bool color_enabled_;

    // Case-insensitive string search
    static size_t findIgnoreCase(const std::string& text, const std::string& pattern, size_t pos = 0);

    // Get lowercase version of string
    static std::string toLower(const std::string& str);
};

} // namespace TinyFileSearch
