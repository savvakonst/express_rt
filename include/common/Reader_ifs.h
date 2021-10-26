#ifndef READER_IFS_H
#define READER_IFS_H
#include <cstdint>

class Reader_ifs {
   public:
    struct Borders {
        uint64_t begin;
        uint64_t end;
    };

    virtual Borders getAvailibleBorders() = 0;
};

#endif