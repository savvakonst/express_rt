#ifndef PRMBUFFER_IFS_H_
#define PRMBUFFER_IFS_H_
#include <string>

#include "Parameter_ifs.h"
#include "common/Reader_ifs.h"

enum class DataStatus : uint32_t
{
    success = 0,
    invalid_data_size = 1,
    connection_was_interrupted = 2,
};

class PrmBuffer_ifs {
   public:
    explicit PrmBuffer_ifs(const Parameter_ifs* parent)  //
        : parameter_(parent) {
        if (parameter_) ((Parameter_ifs*)parameter_)->prm_buffer_ = this;
    }

    virtual ~PrmBuffer_ifs() {
        if (parameter_) ((Parameter_ifs*)parameter_)->prm_buffer_ = nullptr;
    }

    /*
    virtual void setU8Data(uint8_t data) = 0;
    virtual void setU16Data(uint16_t data) = 0;
    virtual void setU32Data(uint32_t data) = 0;
    virtual void setU64Data(uint64_t data) = 0;
    virtual void setArrData(void* data_ptr, size_t size) = 0;
    */

    virtual void setData(char* data, size_t length, DataStatus status) = 0;

    virtual bool lock(bool arg) = 0;
    virtual bool isLock() = 0;

    const Parameter_ifs* parameter_ = nullptr;
};

#endif