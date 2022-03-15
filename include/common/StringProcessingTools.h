// TODO: rename file to string processing tools

#ifndef EXRT_TOOLS_H
#define EXRT_TOOLS_H

#include <cstring>
#include <regex>

#include "common/Port.h"

inline bool isNumber(const char *val) {
    auto it = val;
    if (*it == 0) return false;
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
