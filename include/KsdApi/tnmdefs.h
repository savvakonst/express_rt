#ifndef EXRT_TNMDEFS_H
#define EXRT_TNMDEFS_H

#define TNMLIB_API __stdcall

typedef void *TNMLIB_HANDLE;
#define TNMLIB_INVALID_HANDLE ((TNMLIB_HANDLE)0)

#define _TNMLIB_MAX_DEVICES 128

#include <stdint.h>
#include <windows.h>  // TODO:  remove this anyway,it is best to split this file into architecture independent and architecture depended files

typedef BOOL(CALLBACK *LPTNMLIB_DATAPROCESSPROC)(LPVOID buffer, uint32_t buffer_size, LPVOID param);
typedef BOOL(CALLBACK *LPTNMLIB_PROGRESSPROC)(LPVOID param);

typedef BOOL(CALLBACK *LPTNMLIB_STAGEDREADPROC)(LPVOID stage_buffer, uint32_t stage_size, LPVOID param);
typedef BOOL(CALLBACK *LPTNMLIB_STAGEDWRITEPROC)(LPVOID *stage_buffer, uint32_t stage_size, LPVOID param);

//
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

#include <pshpack1.h>

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
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
            uint8_t millisecond;
        };
        uint32_t u_time;
    };
} TNMLIB_DEVICE_TIME, *LPTNMLIB_DEVICE_TIME;

typedef struct _TNMLIB_DEVICE_VERSION_INFO {
    uint16_t tn_version;
    uint16_t tn_serial_number;
} TNMLIB_DEVICE_VERSION_INFO, *LPTNMLIB_DEVICE_VERSION_INFO;

typedef struct _TNMLIB_DEVICE_MEMORY_INFO {
    uint64_t total_main_memory_size;
    uint64_t installed_main_memory_size;
    uint64_t free_main_memory_size;
    uint32_t records_memory_size;
    uint32_t tasks_memory_size;
    uint32_t bad_blocks_memory_size;
    uint32_t block_size;
    uint32_t number_of_main_memory_chips;
    uint32_t number_of_records;
    uint32_t number_of_tasks;
    uint32_t number_of_bad_blocks;
    uint32_t number_of_memory_types;
    uint8_t reserved[4];
} TNMLIB_DEVICE_MEMORY_INFO, *LPTNMLIB_DEVICE_MEMORY_INFO;

typedef struct _TNMLIB_DEVICE_RECORD_INFO {
    uint64_t size;
    TNMLIB_DEVICE_DATE rd_date;
    TNMLIB_DEVICE_TIME rt_start;
    TNMLIB_DEVICE_TIME rt_stop;
    uint16_t task_number;
    uint16_t fly_number;
    uint16_t data_unit_serial;
    uint16_t data_unit_version;
    uint8_t reserved[4];
} TNMLIB_DEVICE_RECORD_INFO, *LPTNMLIB_DEVICE_RECORD_INFO;

typedef struct _TNMLIB_RECORD_MODULE_INFO {
    uint32_t module_id;
    union {
        struct {
            uint16_t version;
            uint16_t serial_number;
        };
        uint32_t version_serial;
    };

    int32_t mib_address;
    uint32_t status;
    /*
    #define MSTATUS_VALID_MATCH_ENABLED			0
    #define MSTATUS_VALID_NA_DISABLED			1   // answered, task unknown, denied
    #define MSTATUS_NI_NI_DISABLED			2   // didn't answer, task unknown, denied >> not found
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
    uint32_t offset;
    uint32_t size;
    uint32_t tag;
    uint16_t min_version;
    uint16_t reserved;
} TNMLIB_DEVICE_TASK_INFO, *LPTNMLIB_DEVICE_TASK_INFO;

typedef struct _TNMLIB_DEVICE_MEMORY_RWINFO {
    uint32_t total_pages;
    uint32_t page_size;
} TNMLIB_DEVICE_MEMORY_RWINFO, *LPTNMLIB_DEVICE_MEMORY_RWINFO;

typedef struct _TNMLIB_DEVICE_MEMORY_EXINFO {
    TNMLIB_DEVICE_MEMORY_RWINFO rw[2];
} TNMLIB_DEVICE_MEMORY_EXINFO, *LPTNMLIB_DEVICE_MEMORY_EXINFO;

typedef struct _TNMLIB_DEVICE_RECORD_PARAM {
    TNMLIB_DEVICE_TIME rs_time;
    TNMLIB_DEVICE_DATE rs_date;

    union {
        struct {
            uint16_t task_number;
            uint16_t fly_number;
        };
        uint32_t u_task_fly;
    };

    union {
        struct {
            uint16_t data_unit_serial;
            uint16_t data_unit_version;
        };
        uint32_t u_serial_version;
    };

    union {
        uint8_t reserved[4];
        uint32_t u_reserved;
    };

} TNMLIB_DEVICE_RECORD_PARAM, *LPTNMLIB_DEVICE_RECORD_PARAM;

#include <poppack.h>

#endif  // EXRT_TNMDEFS_H
