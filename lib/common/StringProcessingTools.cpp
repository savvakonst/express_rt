#include "common/StringProcessingTools.h"

#define PATHSEP '/'
// returns true if text string matches gitignore-style glob pattern
bool globMatch(const std::string &text_s, const std::string &glob_s) {
    const char *text = text_s.c_str();
    const char *glob = glob_s.c_str();

    const char *text1_backup = nullptr;
    const char *glob1_backup = nullptr;
    const char *text2_backup = nullptr;
    const char *glob2_backup = nullptr;
    // match pathname if glob contains a / otherwise match the basename
    if (*glob == '/') {
        // if pathname starts with ./ then ignore these pairs
        while (*text == '.' && text[1] == PATHSEP) text += 2;
        // if pathname starts with / then ignore it
        if (*text == PATHSEP) text++;
        glob++;
    } else if (strchr(glob, '/') == nullptr) {
        const char *sep = strrchr(text, PATHSEP);
        if (sep) text = sep + 1;
    }
    while (*text != '\0') {
        switch (*glob) {
        case '*':
            if (*++glob == '*') {
                // trailing ** match everything after /
                if (*++glob == '\0') return true;
                // ** followed by a / match zero or more directories
                if (*glob != '/') return false;
                // new **-loop, discard *-loop
                text1_backup = nullptr;
                glob1_backup = nullptr;
                text2_backup = text;
                glob2_backup = ++glob;
                continue;
            }
            // trailing * matches everything except /
            text1_backup = text;
            glob1_backup = glob;
            continue;
        case '?':
            // match any character except /
            if (*text == PATHSEP) break;
            text++;
            glob++;
            continue;
        case '[': {
            int lastchr;
            int matched = false;
            int reverse = glob[1] == '^' || glob[1] == '!';
            // match any character in [...] except /
            if (*text == PATHSEP) break;
            // inverted character class
            if (reverse) glob++;
            // match character class
            for (lastchr = 256; *++glob != '\0' && *glob != ']'; lastchr = *glob)
                if (lastchr < 256 && *glob == '-' && glob[1] != ']' && glob[1] != '\0'
                        ? *text <= *++glob && *text >= lastchr
                        : *text == *glob)
                    matched = true;
            if (matched == reverse) break;
            text++;
            if (*glob != '\0') glob++;
            continue;
        }
        case '\\':
            // literal match \-escaped character
            glob++;
            // FALLTHROUGH
        default:
            // match the current non-NUL character
            if (*glob != *text && !(*glob == '/' && *text == PATHSEP)) break;
            text++;
            glob++;
            continue;
        }
        if (glob1_backup != nullptr && *text1_backup != PATHSEP) {
            // *-loop: backtrack to the last * but do not jump over /
            text = ++text1_backup;
            glob = glob1_backup;
            continue;
        }
        if (glob2_backup != nullptr) {
            // **-loop: backtrack to the last **
            text = ++text2_backup;
            glob = glob2_backup;
            continue;
        }
        return false;
    }
    // ignore trailing stars
    while (*glob == '*') glob++;
    // at end of text means success if nothing else is left to match
    return *glob == '\0';
}

bool filePatternMatch(const std::string &needle, const std::string &haystack) {
    char const *needle_s = needle.c_str();
    char const *haystack_s = haystack.c_str();

    for (; *needle_s != '\0'; ++needle_s) {
        switch (*needle_s) {
        case '?':
            if (*haystack_s == '\0') return false;
            ++haystack_s;
            break;
        case '*': {
            if (needle_s[1] == '\0') return true;
            size_t max = strlen(haystack_s);
            for (size_t i = 0; i < max; i++)
                if (filePatternMatch(needle_s + 1, haystack_s + i)) return true;
            return false;
        }
        default:
            if (*haystack_s != *needle_s) return false;
            ++haystack_s;
        }
    }
    return *haystack_s == '\0';
}
