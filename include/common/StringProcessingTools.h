// TODO: rename file to string processing tools

#ifndef EXRT_TOOLS_H
#define EXRT_TOOLS_H

#include <cstring>
#include <list>
#include <regex>

#include "common/Port.h"

inline std::list<std::string> split(const std::string &text, char character) {
    auto begin_ptr = text.c_str();
    auto current_ptr = begin_ptr;

    std::list<std::string> ret;

    while (*current_ptr) {
        if (*current_ptr == character) {
            if (current_ptr != begin_ptr) ret.emplace_back(begin_ptr, current_ptr - begin_ptr);
            begin_ptr = current_ptr + 1;
        }
        current_ptr++;
    }

    if (current_ptr != begin_ptr) ret.emplace_back(begin_ptr, current_ptr - begin_ptr);

    return ret;
}

inline std::list<std::string> split(const char *text, char character) {
    auto begin_ptr = text;
    auto current_ptr = begin_ptr;

    std::list<std::string> ret;

    while (*current_ptr) {
        if (*current_ptr == character) {
            if (current_ptr != begin_ptr) ret.emplace_back(begin_ptr, current_ptr - begin_ptr);
            begin_ptr = current_ptr + 1;
        }
        current_ptr++;
    }

    if (current_ptr != begin_ptr) ret.emplace_back(begin_ptr, current_ptr - begin_ptr);

    return ret;
}

inline std::pair<std::string, std::string> firstCharPos(const std::string &text, char character) {
    auto c_str = text.c_str();
    auto first_point = c_str;
    while ((*first_point != 0) && (*first_point != character)) first_point++;
    auto size = first_point - c_str;

    std::string begin_path(c_str, size);
    std::string end_path(first_point + 1);

    return {begin_path, end_path};
}

inline std::pair<std::string, std::string> lastCharPos(const std::string &text, char character) {
    auto c_str = text.c_str();
    auto end_point = c_str + text.size();
    while ((c_str < end_point) && (*end_point != character)) end_point--;
    auto size = end_point - c_str;

    std::string begin_path(c_str, size);
    std::string end_path(end_point + 1);

    return {begin_path, end_path};
}

inline bool isNumber(const char *val) {
    auto it = val;
    if (*it == 0) return false;
    if (*it == '-') it++;
    while ((*it != 0) && std::isdigit(*it)) it++;
    return *it == 0;
}

inline bool isNumber(const std::string &val) { return isNumber(val.c_str()); }

inline std::vector<std::string> splitPath(const std::string &path) {
    static const std::regex separator("[^/]+");
    std::vector<std::string> ret;
    ret.reserve(2);

    auto begin = std::sregex_iterator(path.begin(), path.end(), separator);
    auto end = std::sregex_iterator();

    for (std::sregex_iterator i = begin; i != end; ++i) ret.push_back(i->str());

    return ret;
}

COMMON_API_ bool globMatch(const std::string &text, const std::string &glob);

COMMON_API_ bool filePatternMatch(const std::string &text, const std::string &glob);

#endif  // EXRT_TOOLS_H
