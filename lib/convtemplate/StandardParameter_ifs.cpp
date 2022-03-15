

#include <iostream>
//
#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"
#include "convtemplate/StandardParameter_ifs.h"

const DataSchema_ifs* StandardParameter_ifs::getPropertySchema() const { return data_schema_; }

const HierarchicalData_ifs* StandardParameter_ifs::getProperty(const std::string& prop_path) const {
    return getBranch(parameter_field_tree_, prop_path);
}

std::string StandardParameter_ifs::getPropertyAsTxt(const std::string& prop_path) const {
    auto branch = getBranch(parameter_field_tree_, prop_path);
    if (branch->isValue()) return branch->getValue().asString();
    return toString(branch, "");
}

bool StandardParameter_ifs::setProperty(const std::string& prop_path, const Value& value) {
    return parameter_field_tree_->setValue(prop_path, value, error_message_);
}

bool StandardParameter_ifs::setProperty(const std::string& prop_path, const HierarchicalData_ifs* hierarchical_data) {
    return parameter_field_tree_->setValue(prop_path, hierarchical_data, error_message_);
}

bool StandardParameter_ifs::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return parameter_field_tree_->setValueAsTxt(prop_path, value, error_message_);
}

bool StandardParameter_ifs::removeProperty(const std::string& prop_path) {
    return parameter_field_tree_->removeUnit(prop_path, error_message_);
}
