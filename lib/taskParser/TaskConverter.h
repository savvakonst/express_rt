#ifndef EXRT_TASKCONVERTER_H
#define EXRT_TASKCONVERTER_H

#include "common/BaseClass_ifs.h"
#include "common/DataSchema_ifs.h"

class PDefaultBaseIO_ifs;
class Device;

class TaskConverter : public BaseClass_ifs {
   public:
    TaskConverter();

    ~TaskConverter();

    Device* parseTask(const void* task_ptr, size_t size);

    const char* createTask(const Device* device);

    const ErrorInfo_ifs* getErrorInfo() const override { return nullptr; };

   private:
    typedef std::vector<const PDefaultBaseIO_ifs*> PPBList;
    std::map<uint32_t, PPBList*> PPBMap_;

    void addConstructor();

    const PPBList* getPPBMList(uint32_t type) const;
};

#endif
