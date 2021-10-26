#ifndef PRMBUFFER_IFS_H_
#define PRMBUFFER_IFS_H_
#include <string>

#include "Parameter_ifs.h"
#include "common/Reader_ifs.h"

class PrmBuffer_ifs : public Reader_ifs {
    PrmBuffer_ifs(const Parameter_ifs* parent)  //
        : parameter_(parent) {
        ((Parameter_ifs*)parameter_)->prm_buffer_ = this;
    }

    ~PrmBuffer_ifs() {
        if (parameter_) ((Parameter_ifs*)parameter_)->prm_buffer_ = nullptr;
    }

    virtual void setU8Data(uint8_t data) = 0;
    virtual void setU16Data(uint16_t data) = 0;
    virtual void setU32Data(uint32_t data) = 0;
    virtual void setU64Data(uint64_t data) = 0;
    virtual void setArrData(void* data_ptr, size_t size) = 0;

    const Parameter_ifs* parameter_ = nullptr;
};

#endif