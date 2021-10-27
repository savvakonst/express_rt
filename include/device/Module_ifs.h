#ifndef EXO_MODULE_IFS_H
#define EXO_MODULE_IFS_H

#include <string>

#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/FieldInfo.h"

class ModuleStream_ifs;

class Module_ifs : public BaseClass_ifs {
   private:
    /* data */
   public:
    virtual ~Module_ifs() = 0;

    [[nodiscard]] virtual std::string getID() const = 0;

    virtual const InfoList& getPropertiesInfoList() = 0;

    [[nodiscard]] virtual const ResValue_ifs* getProperty(const std::string& prop_path) const = 0;
    [[nodiscard]] virtual const std::string getPropertyAsTxt(const std::string& prop_path) const = 0;

    virtual bool setProperty(const std::string& prop_path, Value value) = 0;
    virtual bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) = 0;

    [[nodiscard]] virtual const void* getTaskPtr() const = 0;
    [[nodiscard]] virtual size_t getTaskSize() const = 0;

    virtual Module_ifs* getSubModule() { return nullptr; }

    virtual ModuleStream_ifs* createModuleStream() = 0;
};




#endif