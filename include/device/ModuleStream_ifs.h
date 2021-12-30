#ifndef EXRT_MODULESTREAM_IFS_H
#define EXRT_MODULESTREAM_IFS_H

#include "common/BaseClass_ifs.h"

class Module_ifs;
class ModuleStreamContext_ifs;

class PrmBuffer_ifs;

class ModuleStream_ifs : public BaseClass_ifs {
   public:
    virtual ~ModuleStream_ifs() {}

    virtual void readFramePeace(ModuleStreamContext_ifs* context, char* ptr, size_t size) = 0;

    virtual int getStatistic() = 0;

    virtual void addPrmBuffer(std::string path, PrmBuffer_ifs* prm_buffer) {}
    virtual void addPrmBuffer(size_t index, size_t sub_index, PrmBuffer_ifs* prm_buffer) {}

    // TODO: need to delete
    virtual std::map<std::string, PrmBuffer_ifs*> getPrmBufferMap() { return std::map<std::string, PrmBuffer_ifs*>(); }

    virtual const Module_ifs* getModule() = 0;
};

#endif