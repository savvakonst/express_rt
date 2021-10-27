#ifndef EXRT_KSDMODULE_H
#define EXRT_KSDMODULE_H

#include <cstdint>
#include <map>
#include <utility>

#include "device/ModuleStream_ifs.h"
#include "device/Module_ifs.h"

class TaskMapper : public HierarchicalData_ifs {
   public:
    enum StructType
    {
        value,
        array,
        map,
    };
    const StructType struct_type_;

    TaskMapper(DataType type) : struct_type_(StructType::value), type_(type) { size_ = getTSize(type_); }

    TaskMapper(size_t len, const TaskMapper &value) : struct_type_(StructType::array) {
        vector_.reserve(len);
        for (size_t i = 0; i < len; i++) {
            vector_.push_back(value);
        }
        size_ = len * value.size_;
    }

    typedef std::pair<std::string, TaskMapper> tuple;

    explicit TaskMapper(std::vector<tuple> vecmap) : vecmap_(std::move(vecmap)), struct_type_(StructType::map) {
        size_ = 0;
        for (auto &i : vecmap_) {
            size_ += i.second.size_;
            map_[i.first] = &i.second;
        }
    }

    ~TaskMapper() {}

    bool isArray() const override { return struct_type_ == StructType::array; };
    bool isMap() const override { return struct_type_ == StructType::map; };
    bool isValue() const override { return struct_type_ == StructType::value; };

    Value getValue() const override {  // necessarry to add guard
        return Value(ptr_, type_);
    };

    std::vector<HierarchicalData_ifs *> getArray() const override {
        std::vector<HierarchicalData_ifs *> ret;

        ret.reserve(vector_.size());
        for (auto &i : vector_) {
            ret.push_back((TaskMapper *)&i);
        }
        return ret;
    };

    std::map<std::string, HierarchicalData_ifs *> getMap() const override {
        std::map<std::string, HierarchicalData_ifs *> ret;
        for (auto &i : map_) {
            ret[i.first] = (TaskMapper *)&i;
        }
        return ret;
    };

    HierarchicalData_ifs *getArrayUint(size_t id) const override {
        if (id >= vector_.size()) return nullptr;
        return (TaskMapper *)&(vector_[id]);
    }

    HierarchicalData_ifs *getMapUint(std::string id) const override {
        auto map_uint = map_.find(id);
        if (map_uint != map_.end()) return (TaskMapper *)&(map_uint->second);
        return nullptr;
    }

    void setReferencePtr(void *ptr) {
        ptr_ = ptr;
        char *p = (char *)ptr;
        switch (struct_type_) {
        case array:
            for (auto &i : vector_) {
                i.setReferencePtr(p);
                p += i.size_;
            }
            break;
        case map:
            for (auto &i : vecmap_) {
                i.second.setReferencePtr(p);
                p += i.second.size_;
            }
            break;
        default:
            break;
        }
    }

   private:
    size_t size_ = 0;
    DataType type_ = DataType::none_v;
    void *ptr_ = nullptr;
    std::vector<TaskMapper> vector_;
    std::vector<tuple> vecmap_;
    std::map<std::string, HierarchicalData_ifs *> map_;
};

/*
 *
 *
 *
 */
class KSDModule : public Module_ifs {
   protected:
    KSDModule(const TaskMapper &field_map) : field_map_(field_map) {}

#pragma pack(1)
    struct MODULE_HEADER {
        uint32_t size;
        uint32_t id;
        uint8_t sub;
        uint8_t slot;
        uint16_t version;
        uint16_t checkSum;
        uint16_t flag;
        uint16_t dimension;
        uint16_t syncMode;
        uint8_t reserved[12];
    };
#pragma pop()

    TaskMapper header_map_ =  //
        TaskMapper({{"size", u32},
                    {"id", u32},
                    {"sub", u8},
                    {"slot", u8},
                    {"version", u16},
                    {"checkSum", u16},
                    {"flag", u16},
                    {"dimension", u16},
                    {"syncMode", u16},
                    {"reserved", {12, u8}}});

    TaskMapper field_map_;

   public:
    // const InfoList& getPropertiesInfoList() override;

    const ResValue_ifs *getProperty(const std::string &prop_path) const override;
    const std::string getPropertyAsTxt(const std::string &prop_path) const override;

    bool setProperty(const std::string &prop_path, const Value value) override;
    bool setPropertyAsTxt(const std::string &prop_path, const std::string &valie) override;
};

#endif
