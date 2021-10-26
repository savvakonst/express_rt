#ifndef EXO_MODULE_IFS_H
#define EXO_MODULE_IFS_H

#include <string>

class ModuleStream_ifs;

class Module_ifs {
   private:
    /* data */
   public:
    Module_ifs(/* args */);

    ~Module_ifs();

    virtual std::string getID() = 0;
    virtual const void* getTaskPtr() = 0;
    virtual size_t getTaskSize() = 0;
    virtual Module_ifs* getSubModule() { return nullptr; }

    virtual ModuleStream_ifs* createModuleStream() = 0;
};

Module_ifs::Module_ifs(/* args */) {}

Module_ifs::~Module_ifs() {}

#endif