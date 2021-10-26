#ifndef EXRT_FIELDTYPE_H
#define EXRT_FIELDTYPE_H
#include <string>
#include <vector>

#include "CustomTypes.h"

class FieldInfo;
/*
 * this is class of uint that contain some arbitary value
 */

class EnumInfo {
   public:
    EnumInfo(DataType type) : type_(type) {}
    ~EnumInfo() {
        if (isPtr(type_))
            for (auto data_ : enum_vals_) delete[] data_.str_v;
    }

    void resize(size_t val) {
        enum_keys_.resize(val);
        enum_vals_.resize(val);
    }

    bool addValue(std::string key, std::string val) {
        enum_keys_.push_back(key);
        enum_vals_.push_back(createArbitraryData(val, type_));
        return true;
    }

    bool addValue(std::string val) {
        enum_keys_.push_back(val);
        enum_vals_.push_back(createArbitraryData(val, type_));
        return true;
    }

    std::vector<std::string> enum_keys_;
    std::vector<ArbitraryData> enum_vals_;

   private:
    DataType type_ = DataType::none_v;
};

/*
 * this is class of field descriptor
 */
class FieldInfo {
   public:
    /* constructor of simple (not enum) FieldInfo*/
    FieldInfo(DataType type, const std::string &name, const std::string &ddescription = "",
              const std::string &help_info = "")
        :  // map_ptr_(nullptr),  //
          name_(name),
          type_(type),
          description_(ddescription),
          help_info_(help_info) {}

    /* constructor of enum FieldInfo with memory allocation for enumerated values
     * list*/
    FieldInfo(DataType type, const std::string &name, EnumInfo *enum_info, const std::string &description = "",
              const std::string &help_info = "")
        :  // map_ptr_(nullptr),
          name_(name),
          type_(type),
          enum_info_(enum_info),
          description_(description),
          help_info_(help_info) {
        // enum_vals_ = new EnumChunck[enum_cnt];
    }

    ~FieldInfo() {
        if (enum_info_) delete enum_info_;
    }
    /*

    */
    // enum_chunks_
    const bool isEnum() const { return (bool)enum_info_; }

    std::string name_ = "";
    std::string description_ = "";
    std::string help_info_ = "";

    DataType type_ = none_v;
    union {
        std::map<std::string, FieldInfo> *map_ptr_ = 0;
        std::vector<FieldInfo> *vec_ptr_;
    };

    ArbitraryData default_value_;
    EnumInfo *enum_info_ = nullptr;
};

class InfoList : public std::vector<FieldInfo *> {
   public:
    InfoList(const std::vector<std::vector<FieldInfo *> *> sets) {
        for (auto set : sets) {
            for (auto field_info : *set) {
                this->push_back(field_info);
            }
        }
    }
};

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


#endif