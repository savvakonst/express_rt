// TODO: rename file to string processing tools

#ifndef EXRT_TOOLS_H
#define EXRT_TOOLS_H

#include <regex>

inline std::vector<std::string> splitPath(const std::string &path) {
    static const std::regex separator("[^/]+");
    std::vector<std::string> ret;
    ret.reserve(2);

    auto begin = std::sregex_iterator(path.begin(), path.end(), separator);
    auto end = std::sregex_iterator();

    for (std::sregex_iterator i = begin; i != end; ++i) ret.push_back(i->str());

    return ret;
}


#endif  // EXRT_TOOLS_H
