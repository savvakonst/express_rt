#ifndef _tnmdefs_header_
#define _tnmdefs_header_

#define TNMLIB_API __stdcall

typedef void *TNMLIB_HANDLE;
#define TNMLIB_INVALID_HANDLE ((TNMLIB_HANDLE)0)

#define _TNMLIB_MAX_DEVICES 128

#include <stdint.h>
typedef BOOL(CALLBACK *LPTNMLIB_DATAPROCESSPROC)(LPVOID lpBuffer, uint32_t dwBufferSize, LPVOID lpParam);
typedef BOOL(CALLBACK *LPTNMLIB_PROGRESSPROC)(LPVOID lpParam);

typedef BOOL(CALLBACK *LPTNMLIB_STAGEDREADPROC)(LPVOID lpStageBuffer, uint32_t dwStageSize, LPVOID lpParam);
typedef BOOL(CALLBACK *LPTNMLIB_STAGEDWRITEPROC)(LPVOID *lpStageBuffer, uint32_t dwStageSize, LPVOID lpParam);

#define TNMLIB_DEVICE_MEMTYPE_RAWFLASH 0
#define TNMLIB_DEVICE_MEMTYPE_FLASH 1
#define TNMLIB_DEVICE_MEMTYPE_TASKS 2
#define TNMLIB_DEVICE_MEMTYPE_BADBLOCKS 3
#define TNMLIB_DEVICE_MEMTYPE_RECORDS 4
#define TNMLIB_DEVICE_MEMTYPE_BOOT 5
#define TNMLIB_DEVICE_MEMTYPE_FAKETASK 6

#define TNMLIB_DEVICE_MEMTYPES_COUNT 7

#define TNMLIB_DEVICE_MEMORY_EXINFO_READ 0
#define TNMLIB_DEVICE_MEMORY_EXINFO_WRITE 1

#include <PshPack1.h>

typedef struct _TNMLIB_DEVICE_DATE {
    union {
        struct {
            uint16_t wYear;
            uint8_t bMonth;
            uint8_t bDay;
        };
        uint32_t dwUDate;
    };
} TNMLIB_DEVICE_DATE, *LPTNMLIB_DEVICE_DATE;

typedef struct _TNMLIB_DEVICE_TIME {
    union {
        struct {
            uint8_t bHour;
            uint8_t bMinute;
            uint8_t bSecond;
            uint8_t bMillisecond;
        };
        uint32_t dwUTime;
    };
} TNMLIB_DEVICE_TIME, *LPTNMLIB_DEVICE_TIME;

typedef struct _TNMLIB_DEVICE_VERSION_INFO {
    uint16_t wTNVersion;
    uint16_t wTNSerialNumber;
} TNMLIB_DEVICE_VERSION_INFO, *LPTNMLIB_DEVICE_VERSION_INFO;

typedef struct _TNMLIB_DEVICE_MEMORY_INFO {
    uint64_t ldwTotalMainMemorySize;
    uint64_t ldwInstalledMainMemorySize;
    uint64_t ldwFreeMainMemorySize;
    uint32_t dwRecordsMemorySize;
    uint32_t dwTasksMemorySize;
    uint32_t dwBadBlocksMemorySize;
    uint32_t dwBlockSize;
    uint32_t dwNumberOfMainMemoryChips;
    uint32_t dwNumberOfRecords;
    uint32_t dwNumberOfTasks;
    uint32_t dwNumberOfBadBlocks;
    uint32_t dwNumberOfMemoryTypes;
    uint8_t Reserved[4];
} TNMLIB_DEVICE_MEMORY_INFO, *LPTNMLIB_DEVICE_MEMORY_INFO;

typedef struct _TNMLIB_DEVICE_RECORD_INFO {
    uint64_t ldwSize;
    TNMLIB_DEVICE_DATE rdDate;
    TNMLIB_DEVICE_TIME rtStart;
    TNMLIB_DEVICE_TIME rtStop;
    uint16_t wTaskNumber;
    uint16_t wFlyNumber;
    uint16_t wDataUnitSerial;
    uint16_t wDataUnitVersion;
    uint8_t Reserved[4];
} TNMLIB_DEVICE_RECORD_INFO, *LPTNMLIB_DEVICE_RECORD_INFO;

typedef struct _TNMLIB_RECORD_MODULE_INFO {
    uint32_t dwModuleID;
    union {
        struct {
            uint16_t wVersion;
            uint16_t wSerialNumber;
        };
        uint32_t dwVersionSerial;
    };

    LONG32 iMIBAddress;
    uint32_t uiStatus;
    /*
    #define MSTATUS_VALID_MATCH_ENABLED			0
    #define MSTATUS_VALID_NA_DISABLED			1	// answered, task unknown, denied
    #define MSTATUS_NI_NI_DISABLED				2   // didn't answer, task unknown, denied >> not found
    #define MSTATUS_INVALID_NI_DISABLED			3   // answered incorrect
    #define MSTATUS_VALID_MISMATCH_DISABLED		4   // answered, task miss
    #define MSTATUS_VALID_MATCH_DISABLED		5   // answered, task ok, disabled
    #define MSTATUS_VALID_ERROR_DISABLED		6   // answered, task transfer error
    */
} TNMLIB_RECORD_MODULE_INFO, *LPTNMLIB_RECORD_MODULE_INFO;

#define TNMLIB_CFG_MAX_MODULES_ENTRIES 16

typedef struct _TNM_RECORD_MODULES_MAP {
    TNMLIB_RECORD_MODULE_INFO MI[TNMLIB_CFG_MAX_MODULES_ENTRIES];

} TNMLIB_RECORD_MODULES_MAP, *LPTNMLIB_RECORD_MODULES_MAP;

typedef struct _TNMLIB_DEVICE_TASK_INFO {
    uint32_t dwOffset;
    uint32_t dwSize;
    uint32_t dwTag;
    uint16_t wMinVersion;
    uint16_t Reserved;
} TNMLIB_DEVICE_TASK_INFO, *LPTNMLIB_DEVICE_TASK_INFO;

typedef struct _TNMLIB_DEVICE_MEMORY_RWINFO {
    uint32_t dwTotalPages;
    uint32_t dwPageSize;
} TNMLIB_DEVICE_MEMORY_RWINFO, *LPTNMLIB_DEVICE_MEMORY_RWINFO;

typedef struct _TNMLIB_DEVICE_MEMORY_EXINFO {
    TNMLIB_DEVICE_MEMORY_RWINFO RW[2];
} TNMLIB_DEVICE_MEMORY_EXINFO, *LPTNMLIB_DEVICE_MEMORY_EXINFO;

typedef struct _TNMLIB_DEVICE_RECORD_PARAM {
    TNMLIB_DEVICE_TIME rsTime;
    TNMLIB_DEVICE_DATE rsDate;

    union {
        struct {
            uint16_t wTaskNumber;
            uint16_t wFlyNumber;
        };
        uint32_t dwUTaskFly;
    };

    union {
        struct {
            uint16_t wDataUnitSerial;
            uint16_t wDataUnitVersion;
        };
        uint32_t dwUSerialVersion;
    };

    union {
        uint8_t Reserved[4];
        uint32_t dwUReserved;
    };

} TNMLIB_DEVICE_RECORD_PARAM, *LPTNMLIB_DEVICE_RECORD_PARAM;

#include <PopPack.h>

#endif  // _tnmdefs_header_
