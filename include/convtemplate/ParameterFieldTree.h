#ifndef EXRT_PARAMETERFIELDTREE_H
#define EXRT_PARAMETERFIELDTREE_H

#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"

// TODO maybe it should be named as something like EditableHierarchicalData
class ParameterFieldTree_ifs : public HierarchicalData_ifs {
   public:
    [[nodiscard]] bool isArray() const override { return false; }

    [[nodiscard]] bool isMap() const override { return false; }

    [[nodiscard]] bool isValue() const override { return false; }

    [[nodiscard]] Value getValue() const override { return {}; }

    [[nodiscard]] std::vector<HierarchicalData_ifs *> getArray() const override { return {}; }

    [[nodiscard]] std::vector<std::pair<std::string, HierarchicalData_ifs *>> getMap() const override { return {}; }

    [[nodiscard]] HierarchicalData_ifs *getArrayUnit(size_t id) const override { return nullptr; }

    [[nodiscard]] HierarchicalData_ifs *getMapUnit(const std::string &id) const override { return nullptr; }

    [[nodiscard]] virtual bool addArrayUnit() { return false; }

    [[nodiscard]] virtual bool removeArrayUnit(size_t index) { return false; }

    [[nodiscard]] virtual size_t getSize() { return 0; }

    [[maybe_unused]] virtual bool setValue(const Value &data) { return false; }

    [[maybe_unused]] virtual bool setValue(const std::string &data) { return false; }

    bool removeUnit(const std::string &prop_path, std::string &error_message);

    [[maybe_unused]] bool setValue(const std::string &prop_path, const Value &value, std::string &error_message);

    /**
     * it isn't finished yet, at now only passing nullptr to expand the array works.
     * it copies content of hierarchical data to the destination path if data is valid.
     * if prop_path points to the next element after the last element in the array or to the "-1" element, array will be extended
     * with new element, also you may pas nullptr to the corresponding path for array extending, in other cases
     * passing nullptr will have no effect.
     * @param prop_path
     * @param hierarchical_data
     * @param error_message
     * @return true if success otherwise return false
     */
    [[maybe_unused]] bool setValue(const std::string &prop_path, const HierarchicalData_ifs *hierarchical_data,
                                   std::string &error_message);
    [[maybe_unused]] bool setValueAsTxt(const std::string &prop_path, const std::string &, std::string &error_message);
};

COMMON_API_ ParameterFieldTree_ifs *newParameterFieldTree(const DataSchema_ifs *data_schema, size_t dim = 0);

#endif  // EXRT_PARAMETERFIELDTREE_H
