#ifndef EXRT_EXTENSION_IFS_H_
#define EXRT_EXTENSION_IFS_H_

#if defined(__cplusplus)
#    define InitExtension(RTYPE) extern "C" __declspec(dllexport) RTYPE
#else /* __cplusplus */
#    define InitExtension(RTYPE) __declspec(dllexport) RTYPE
#endif /* __cplusplus */

#define CONCATENATOR(LEFT, RIGHT) LEFT##RIGHT
#define POST_CONCATENATOR(A, B) CONCATENATOR(A, B)

typedef size_t version_t;

struct ExtensionUnit {
    const char* name;
    const char* type;
    const char* description;
    void* ptr;
    version_t version;
};

struct ExtensionInfo {
    const char* description;
    version_t version;
    ExtensionUnit* units;
};

typedef ExtensionInfo* (*ExtensionInitFunction)(void);

#endif