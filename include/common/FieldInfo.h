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

    EnumInfo(std::string str_type) : type_(createDataType(str_type)) {}

    void resize(size_t val) {
        enums_.resize(val);
    }

    bool addValue(std::string key, std::string val) {

        auto v = Value(val, type_);
        if (v) {
            enums_.push_back({val, v});
        }
        return bool(v);
    }

    bool addValue(std::string val) {
        auto v = Value(val, type_);
        if (v) {
            enums_.push_back({val, v});
        }
        return bool(v);
    }

    const std::vector<std::pair<std::string, Value>> &getEnums() const {
        return enums_;
    }

private:
    std::vector<std::pair<std::string, Value>> enums_;

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
    const bool isEnum() const { return (bool) enum_info_; }

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

typedef std::vector<std::pair<std::string, Value>> EnumInfo_t;

class DataSchema_ifs {
public:

    DataSchema_ifs(const std::string &name, const std::string &description) //
            : name_(name), description_(description) {
    }


    virtual ~DataSchema_ifs() {}

    virtual std::string getType() const = 0;

    virtual std::string getRepresentationType() const = 0;

    virtual bool isValid() const = 0;

    virtual bool isMap() const = 0;

    virtual bool isArray() const = 0;

    virtual const std::vector<size_t> getDims() const = 0;

    virtual const std::vector<DataSchema_ifs *> getMapList() const = 0;

    virtual bool validate(HierarchicalData_ifs *data) const = 0;


    std::string name_ = "";
    std::string description_ = "";
    std::string help_ = "";

    Value default_value_ = Value();
    EnumInfo_t enums_ = {};


private:

};


class InfoList : public std::vector<FieldInfo *> {
public:
    InfoList(const std::vector<std::vector<FieldInfo *> *> sets) {
        for (auto set: sets) {
            for (auto field_info: *set) {
                this->push_back(field_info);
            }
        }
    }
};

typedef size_t StatusFlags;

struct ResValue : Value {
    ResValue() : Value() {}

    ResValue(const Value &arg) : Value(arg) {}

    FieldInfo *field_info_ = nullptr;
    StatusFlags data_status_ = 0;
};

#endif