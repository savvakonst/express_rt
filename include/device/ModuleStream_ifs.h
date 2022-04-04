#ifndef EXRT_MODULESTREAM_IFS_H
#define EXRT_MODULESTREAM_IFS_H

#include "common/BaseClass_ifs.h"
#include "common/TimeRepresentation.h"
class Module_ifs;
class ModuleStreamContext_ifs;

class PrmBuffer_ifs;

class ModuleStream_ifs : public BaseClass_ifs {
   public:
    ~ModuleStream_ifs() override = default;

    virtual void readFramePeace(ModuleStreamContext_ifs* context, char* ptr, size_t size) = 0;

    virtual int getStatistic() = 0;

    virtual bool addPrmBuffer(const std::string& path, PrmBuffer_ifs* prm_buffer) { return false; }

    [[nodiscard]] virtual const RelativeTime& getTime() const { return {0, 0}; }

    // TODO: need to delete
    virtual std::map<std::string, PrmBuffer_ifs*> getPrmBufferMap() { return {}; }

    virtual const Module_ifs* getModule() = 0;

    virtual ModuleStream_ifs* reduce() { return this; }
};

#endif