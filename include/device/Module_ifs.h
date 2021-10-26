#ifndef EXO_MODULE_IFS_H
#define EXO_MODULE_IFS_H

#include <string>
#include "common/CustomTypes.h"
#include "common/FieldInfo.h"

class ModuleStream_ifs;

class Module_ifs {
   private:
    /* data */
   public:
    Module_ifs(/* args */);

    ~Module_ifs();

    virtual std::string getID() const = 0;

    virtual const InfoList& getPropertiesInfoList() = 0;

    virtual const ResValue_ifs* getProperty(const std::string& prop_path) const = 0;
    virtual const std::string getPropertyAsTxt(const std::string& prop_path) const = 0;

    virtual bool setProperty(const std::string& prop_path, const Value value) = 0;
    virtual bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) = 0;


    virtual const void* getTaskPtr() = 0;
    virtual size_t getTaskSize() = 0;

    virtual Module_ifs* getSubModule() { return nullptr; }

    virtual ModuleStream_ifs* createModuleStream() = 0;
};

Module_ifs::Module_ifs(/* args */) {}

Module_ifs::~Module_ifs() {}

#endif