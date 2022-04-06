//
// Created by SVK on 06.04.2022.
//

#ifndef EXRT_TASKMAPPER_H
#define EXRT_TASKMAPPER_H

#include <cstdint>
#include <map>
#include <utility>

#include "common/CustomTypes.h"

class COMMON_API_ TaskMapper : public HierarchicalData_ifs {
   public:
    enum StructType
    {
        value,
        array,
        map,
    };
    const StructType struct_type_;

    TaskMapper(const TaskMapper &task_mapper);

    TaskMapper(DataType type);

    TaskMapper(size_t len, const TaskMapper &value);

    typedef std::pair<std::string, TaskMapper> tuple_t;

    explicit TaskMapper(std::vector<tuple_t> vecmap);

    ~TaskMapper() override;

    [[nodiscard]] bool isArray() const override { return struct_type_ == StructType::array; };

    [[nodiscard]] bool isMap() const override { return struct_type_ == StructType::map; };

    [[nodiscard]] bool isValue() const override { return struct_type_ == StructType::value; };

    [[nodiscard]] Value getValue() const override {  // TODO: necessary to add guard
        return {ptr_, type_};
    };

    [[nodiscard]] std::vector<HierarchicalData_ifs *> getArray() const override;

    [[nodiscard]] getMapReturn_t getMap() const override;

    [[nodiscard]] HierarchicalData_ifs *getArrayUnit(size_t id) const override;

    [[nodiscard]] HierarchicalData_ifs *getMapUnit(const std::string &id) const override;

    [[maybe_unused]] bool setValue(const Value &data);

    [[maybe_unused]] bool setValue(const std::string &data);

    void setReferencePtr(void *ptr);

   private:
    size_t size_ = 0;
    DataType type_ = none_v;
    void *ptr_ = nullptr;
    std::vector<TaskMapper> vector_;
    std::vector<tuple_t> vecmap_;
    std::map<std::string, HierarchicalData_ifs *> map_;
};

#endif  // EXRT_TASKMAPPER_H
