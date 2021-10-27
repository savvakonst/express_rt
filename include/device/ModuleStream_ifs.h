#ifndef COMMON_MODULESTREAM_IFS_H
#define COMMON_MODULESTREAM_IFS_H

#include "common/BaseClass_ifs.h"

class Module_ifs;
class ModuleStreamContext_ifs;

class ModuleStream_ifs: public BaseClass_ifs{
   public:

    virtual void readFramePeace(ModuleStreamContext_ifs* context, char*ptr, size_t size) = 0;

    virtual int getStatistic() = 0;

    virtual const Module_ifs* getModule() = 0;
};



#endif  // COMMON_MODULESTREAM_IFS_H