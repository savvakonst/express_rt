#ifndef EXO_PORT_H
#define EXO_PORT_H

#ifdef _MSC_VER
#    if COMMON_CREATE_SHARED_LIBRARY
#        define COMMON_API_ __declspec(dllexport)
#    else  // elif COMMON__LINKED_AS_SHARED_LIBRARY
#        define COMMON_API_ __declspec(dllimport)
#    endif
#elif __GNUC__ >= 4 || defined(__clang__)
#    define COMMON_API_ __attribute__((visibility("default")))
#endif  // _MSC_VER

#ifndef COMMON_API_
#    define COMMON_API_
#endif

#endif