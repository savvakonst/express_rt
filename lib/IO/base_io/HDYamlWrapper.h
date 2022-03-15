#ifndef HIERARCHICALDATAYAMLWRAPPER_H
#define HIERARCHICALDATAYAMLWRAPPER_H
//
//
#include "common/CustomTypes.h"
#include "yaml-cpp/yaml.h"

/*
 * The RdHierarchicalData class realize wrapper for yaml-cpp
 */
class HierarchicalDataYamlWrapper : public HierarchicalData_ifs {
    // better to create move constructor
   public:
    explicit HierarchicalDataYamlWrapper(const YAML::Node &value);

    ~HierarchicalDataYamlWrapper() override;

    bool isArray() const override;
    bool isMap() const override;
    bool isValue() const override;

    Value getValue() const override;
    std::vector<HierarchicalData_ifs *> getArray() const override;
    getMapReturn_t getMap() const override;

    HierarchicalData_ifs *getArrayUnit(size_t) const override;
    HierarchicalData_ifs *getMapUnit(const std::string &) const override;

   private:
    union {
        std::vector<HierarchicalData_ifs *> *vector_;
        std::map<std::string, HierarchicalData_ifs *> *map_;
        // getMapReturn_t *map_;
    };
    const YAML::Node value_;
};

#endif