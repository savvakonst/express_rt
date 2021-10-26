#ifndef HIERARCHICALDATAYAMLWRAPPER_H
#define HIERARCHICALDATAYAMLWRAPPER_H

#include "common/CustomTypes.h"
#include "yaml-cpp/yaml.h"

/*
 * The RdHierarchicalData class realize wrapper for yaml-cpp
 */
class HierarchicalDataYamlWrapper : public HierarchicalData_ifs {
    // better to create move constructor
   public:
    HierarchicalDataYamlWrapper(const YAML::Node &value);

    ~HierarchicalDataYamlWrapper();

    bool isArray() const;
    bool isMap() const;
    bool isValue() const;

    Value getValue() const;
    std::vector<HierarchicalData_ifs *> getArray() const;
    std::map<std::string, HierarchicalData_ifs *> getMap() const;

   private:
    union {
        std::vector<HierarchicalData_ifs *> *vector_;
        std::map<std::string, HierarchicalData_ifs *> *map_;
    };
    const YAML::Node value_;
};

#endif