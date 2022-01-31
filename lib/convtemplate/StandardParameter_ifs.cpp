

#include <iostream>
//
#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"
#include "convtemplate/StandardParameter_ifs.h"

const DataSchema_ifs* StandardParameter_ifs::getPropertySchema() { return data_schema_; }

const HierarchicalData_ifs* StandardParameter_ifs::getProperty(const std::string& prop_path) const {
    return getBranch(parameter_field_tree_, prop_path);
}

std::string StandardParameter_ifs::getPropertyAsTxt(const std::string& prop_path) const {
    return toString(getBranch(parameter_field_tree_, prop_path), "");
}

bool StandardParameter_ifs::setProperty(const std::string& prop_path, const Value& value) {
    auto ptr = (ParameterFieldTree_ifs*)getBranch(parameter_field_tree_, prop_path);
    if (ptr == nullptr || !ptr->isValue()) {
        error_message_ = "non-existent path";
        return false;
    }

    if (!ptr->setValue(value)) {
        error_message_ = "incompatible types";
        return false;
    }
    return true;
}

bool StandardParameter_ifs::setProperty(const std::string& prop_path, const HierarchicalData_ifs* hierarchical_data) {
    return false;
}

bool StandardParameter_ifs::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    auto ptr = (ParameterFieldTree_ifs*)getBranch(parameter_field_tree_, prop_path);
    if (ptr == nullptr || !ptr->isValue()) {
        error_message_ = "non-existent path";
        return false;
    }

    if (!ptr->setValue(value)) {
        error_message_ = "incompatible types";
        return false;
    }
    return true;
}