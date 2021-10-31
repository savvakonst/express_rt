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

    TaskMapper(DataType type);

    TaskMapper(size_t len, const TaskMapper &value);

    typedef std::pair<std::string, TaskMapper> tuple;

    explicit TaskMapper(std::vector<tuple> vecmap);

    ~TaskMapper() override;

    [[nodiscard]] bool isArray() const override { return struct_type_ == StructType::array; };
    [[nodiscard]] bool isMap() const override { return struct_type_ == StructType::map; };
    [[nodiscard]] bool isValue() const override { return struct_type_ == StructType::value; };

    [[nodiscard]] Value getValue() const override {  // necessarry to add guard
        return {ptr_, type_};
    };

    [[nodiscard]] std::vector<HierarchicalData_ifs *> getArray() const override;

    [[nodiscard]] std::map<std::string, HierarchicalData_ifs *> getMap() const override;

    [[nodiscard]] HierarchicalData_ifs *getArrayUint(size_t id) const override;

    [[nodiscard]] HierarchicalData_ifs *getMapUint(std::string id) const override;

    [[maybe_unused]] bool setValue(Value data);

    [[maybe_unused]] bool setValue(std::string data);

    void setReferencePtr(void *ptr);

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
    explicit KSDModule(const TaskMapper &field_map) : field_map_(field_map) {}

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
#pragma pack()

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

    const TaskMapper *getBranch(const std::string &prop_path) const;

   public:
    // const InfoList& getPropertiesInfoList() override;

    const InfoList *getPropertiesInfoList() override {
        error_mesadge_ = "The getPropertiesInfoList function is not realised yet";
        return nullptr;
    }

    [[nodiscard]] ResValue getProperty(const std::string &prop_path) const override;
    [[nodiscard]] std::string getPropertyAsTxt(const std::string &prop_path) const override;

    bool setProperty(const std::string &prop_path, Value value) override;
    bool setPropertyAsTxt(const std::string &prop_path, const std::string &valie) override;
};

#endif
