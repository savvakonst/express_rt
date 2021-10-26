#ifndef PARAMETER_IFS_H
#define PARAMETER_IFS_H
#include <string>

#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/FieldInfo.h"

class PrmBuffer_ifs;

typedef size_t StatusFlags;

typedef struct {
    FieldInfo* field_info_;
    StatusFlags data_status_;
    size_t size_;
    char* value_;
} ResValue_ifs;

struct SelfValue : ResValue_ifs {
    /*   SelfValue(FieldInfo* field_info) {
           field_info_ = field_info;
           if (field_info_->default_value_) {
               size_  = field_info_->default_value_size_;
               value_ = new char[size_];
               memcpy(value_, field_info_->default_value_, size_);
           } else {
               isNum()
           }
       }
       ~SelfValue() { delete[] value; }
       */
};

class COMMON_API_ Parameter_ifs : public BaseClass_ifs {
   public:
    Parameter_ifs(const std::string& name) : name_(name){};
    ~Parameter_ifs(){};

    virtual PrmBuffer_ifs* createBuffer() const = 0;
    virtual const std::string getType() const = 0;

    virtual const ResValue_ifs* getProperty(const std::string& prop_path) const = 0;
    virtual const std::string getPropertyAsTxt(const std::string& prop_path) const = 0;

    virtual bool setProperty(const std::string& prop_path, const Value value) = 0;
    virtual bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) = 0;

    virtual const InfoList& getPropertiesInfoList() = 0;

    virtual const bool isValid() const = 0;
    bool isLocked() const { return (bool)prm_buffer_; }

   protected:
    std::string name_ = std::string("");
    std::string path_ = std::string("");

    friend PrmBuffer_ifs;
    PrmBuffer_ifs* prm_buffer_ = nullptr;
};

extern std::vector<FieldInfo*> g_common_parameter_fields;

#endif