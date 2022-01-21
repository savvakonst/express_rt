#ifndef EXRT_PARAMETERFIELDTREE_H
#define EXRT_PARAMETERFIELDTREE_H

#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/FieldInfo.h"


class ParameterFieldTree : public HierarchicalData_ifs {
   public:

    enum StructType
    {
        value,
        array,
        map,
    };

    const StructType struct_type_;

    typedef std::pair<std::string, ParameterFieldTree> tuple_t;
    
    explicit ParameterFieldTree(DataSchema_ifs *data_schema, size_t dim = 0);

    ~ParameterFieldTree() override;

    [[nodiscard]] bool isArray() const override { return struct_type_ == StructType::array; };

    [[nodiscard]] bool isMap() const override { return struct_type_ == StructType::map; };

    [[nodiscard]] bool isValue() const override { return struct_type_ == StructType::value; };

    [[nodiscard]] Value getValue() const override {  // necessarry to add guard
        return value_;
    };

    [[nodiscard]] std::vector<HierarchicalData_ifs *> getArray() const override;

    [[nodiscard]] std::map<std::string, HierarchicalData_ifs *> getMap() const override;

    [[nodiscard]] HierarchicalData_ifs *getArrayUint(size_t id) const override;

    [[nodiscard]] HierarchicalData_ifs *getMapUint(std::string id) const override;

    [[maybe_unused]] bool setValue(Value data);

    [[maybe_unused]] bool setValue(std::string data);


   private:
    size_t size_ = 0;
    DataType type_ = DataType::none_v;
    Value value_ = Value();
    std::vector<ParameterFieldTree> vector_;
    std::vector<tuple_t> vecmap_;
    std::map<std::string, HierarchicalData_ifs *> map_;
};


#endif  // EXRT_PARAMETERFIELDTREE_H
