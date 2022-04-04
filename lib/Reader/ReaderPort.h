//
// Created by SVK on 30.03.2022.
//

#ifndef EXRT_READER_PORT_H
#define EXRT_READER_PORT_H

#ifdef _MSC_VER
#    if READER_CREATE_SHARED_LIBRARY
#        define READER_API_ __declspec(dllexport)
#    else  // elif COMMON__LINKED_AS_SHARED_LIBRARY
#        define READER_API_ __declspec(dllimport)
#    endif
#elif __GNUC__ >= 4 || defined(__clang__)
#    define READER_API_ __attribute__((visibility("default")))
#endif  // _MSC_VER

#endif  // EXRT_PORT_H
