

#include "convtemplate/ConversionTemplate.h"

#include "common/ExtensionManager.h"
#include "convtemplate/ParameterFieldTree.h"
#include "convtemplate/Parameter_ifs.h"

ConversionTemplate::ConversionTemplate(ExtensionManager* manager) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "conversion_template");

    if (unit && unit->ptr) {
        info_schema_ = (DataSchema_ifs*)unit->ptr;
        info_schema_->init(manager);
    }
    info_data_ = newParameterFieldTree(info_schema_);
}

status ConversionTemplate::setName(std::string name) {
    auto bool_status = ((ParameterFieldTree_ifs*)info_data_)->setValue("name", Value(name), error_message_);
    return bool_status ? status::succes : status::failure;
}

status ConversionTemplate::addInfo(const std::string& path, const Value& value) {
    auto bool_status = ((ParameterFieldTree_ifs*)info_data_)->setValue(path, value, error_message_);
    return bool_status ? status::succes : status::failure;
}

status ConversionTemplate::addInfo(const std::string& path, const std::string& value) {
    // TODO: implement this member function
    return failure;
}

const HierarchicalData_ifs* ConversionTemplate::getInfo(const std::string& path) const {
    return getBranch(info_data_, path);
}

const DataSchema_ifs* ConversionTemplate::getInfoSchema() const { return info_schema_; }

/*
 *
 *
 */

status ConversionTemplate::addParameter(Parameter_ifs* parameter) {
    auto name = parameter->getPropertyAsTxt("name");
    if (parameters_.find(name) == parameters_.end()) {
        error_message_ = "parameter \"" + name + "\" already exists";
        return status::failure;
    }
    return status::succes;
}

const Parameter_ifs* ConversionTemplate::getParameter(std::string name) const {
    auto prm_iter = parameters_.find(name);
    if (prm_iter != parameters_.end()) {
        return prm_iter->second;
    }
    return nullptr;
}

exo_container<const Parameter_ifs*> ConversionTemplate::getSingleTypeParameters(std::string type) const {
    exo_container<const Parameter_ifs*> container;
    size_t path_len = type.size();
    for (const auto& param : parameters_) {
        if (param.second->getPropertyAsTxt("type") == type) container.push_back(param.second);
    }
    return container;
}

exo_container<const Parameter_ifs*> ConversionTemplate::getParametersFromPath(std::string searching_path) const {
    exo_container<const Parameter_ifs*> container;
    size_t path_len = searching_path.size();
    for (const auto& param : parameters_) {
        const auto path = param.second->getPropertyAsTxt("path");
        if (path.size() >= path_len)
            if (path.substr(0, path_len) == path) container.push_back(param.second);
    }

    return container;
}

status ConversionTemplate::changeParameterName(std::string old, std::string new_name) {  //
    return status::failure;
}

status ConversionTemplate::removeParameter(std::string name) {
    if (parameters_.find(name) == parameters_.end()) {
        error_message_ = "parameter \"" + name + "\" doesn't exist";
        return status::failure;
    }

    delete parameters_[name];
    parameters_.erase(name);

    return status::succes;
}

status ConversionTemplate::removeParametersFromPath(std::string path_to_delete) {
    std::list<std::string> names_to_remove;
    size_t path_len = path_to_delete.size();
    for (const auto& param : parameters_) {
        const auto path = param.second->getPropertyAsTxt("path");
        if (path.size() >= path_len)
            if (path.substr(0, path_len) == path_to_delete) names_to_remove.push_back(param.first);
    }

    for (auto& name : names_to_remove) {
        delete parameters_[name];
        parameters_.erase(name);
    }

    if (names_to_remove.size()) return status::succes;

    error_message_ = "there are no parameters on \"" + path_to_delete + "\" path";
    return status::failure;
}

/*
 *
 *
 */

status ConversionTemplate::addModule(std::string path) {
    if (modules_.find(path) != modules_.end()) {
        error_message_ = "module \"" + path + "\" already exists";
        return status::failure;
    }

    modules_.insert(path);
    return status::succes;
}

status ConversionTemplate::removeModulesFromPath(std::string path_to_delete) {
    std::list<std::string> paths_to_remove;
    size_t path_len = path_to_delete.size();
    for (auto& path : modules_) {
        if (path.size() >= path_len)
            if (path.substr(0, path_len) == path_to_delete) paths_to_remove.push_back(path);
    }

    for (auto& path : paths_to_remove) modules_.erase(path);

    if (paths_to_remove.size()) return status::succes;

    error_message_ = "there are no modules on \"" + path_to_delete + "\" path";
    return status::failure;
}

const exo_container<std::string> ConversionTemplate::getModulesFromPath(std::string path_to_find) const {
    exo_container<std::string> container;
    size_t path_len = path_to_find.size();
    for (auto path : modules_) {
        if (path.size() >= path_len)
            if (path.substr(0, path_len) == path) container.push_back(path);
    }
    return container;
}

/*
 *
 *
 */

const ErrorInfo_ifs* ConversionTemplate::getErrorInfo() const { return nullptr; }

status ConversionTemplate::getModulesFromPath(std::string path) { return failure; }
