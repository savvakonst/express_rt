
#ifndef EXRT_STANDARDPARAMETER_IFS_H
#define EXRT_STANDARDPARAMETER_IFS_H

#include <iostream>

#include "common/Port.h"
#include "convtemplate/ParameterFieldTree.h"
#include "extensions/PDefaultBaseIO_ifs.h"

class COMMON_API_ StandardParameter_ifs : public Parameter_ifs {
   public:
    ~StandardParameter_ifs() override {
        if (parameter_field_tree_) delete parameter_field_tree_;
    };

    const DataSchema_ifs* getPropertySchema() override;

    [[nodiscard]] const HierarchicalData_ifs* getProperty(const std::string& prop_path) const override;

    [[nodiscard]] std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value& value) override;

    bool setProperty(const std::string& prop_path, const HierarchicalData_ifs* hierarchical_data) override;

    bool setPropertyAsTxt(const std::string& prop_path, const std::string& value) override;

   protected:
    ParameterFieldTree_ifs* parameter_field_tree_ = nullptr;
    DataSchema_ifs* data_schema_ = nullptr;
};

#endif  // EXRT_STANDARDPARAMETER_IFS_H
