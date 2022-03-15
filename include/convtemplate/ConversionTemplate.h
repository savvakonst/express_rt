#ifndef CONVERSIONTEMPLATE_H
#define CONVERSIONTEMPLATE_H

#include <string>

#include "common/BaseClass_ifs.h"
#include "common/Port.h"
#include "common/Properties_ifs.h"

class ExtensionManager;
class Parameter_ifs;
class ParameterFieldTree_ifs;
class DataSchema_ifs;

class COMMON_API_ ConversionTemplate : public Properties_ifs {
   public:
    explicit ConversionTemplate(ExtensionManager* manager);

    /*
     *  base description voids
     *
     */

    [[nodiscard]] const DataSchema_ifs* getPropertySchema() const override;
    [[nodiscard]] const HierarchicalData_ifs* getProperty(const std::string& path) const override;
    [[nodiscard]] std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setName(const std::string& name);
    bool setProperty(const std::string& path, const Value& value) override;
    bool setProperty(const std::string& prop_path, const HierarchicalData_ifs* hierarchical_data) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& value) override;
    bool removeProperty(const std::string& prop_path) override;
    /*
     *
     *
     */

    status addParameter(Parameter_ifs* prm);
    [[nodiscard]] const exo_map<std::string, Parameter_ifs*>& getAllParameters() const { return parameters_; }
    [[nodiscard]] const Parameter_ifs* getParameter(const std::string& name) const;
    [[nodiscard]] exo_container<const Parameter_ifs*> getSingleTypeParameters(const std::string& type) const;
    [[nodiscard]] exo_container<const Parameter_ifs*> getParametersFromPath(const std::string& searching_path) const;
    status changeParameterName(const std::string& old, const std::string& new_name);
    status removeParameter(const std::string& name);
    status removeParametersFromPath(const std::string& path);

    /*
     *
     *
     */

    status addModule(const std::string& path);
    status removeModulesFromPath(const std::string& path);
    status getModulesFromPath(const std::string& path);
    [[nodiscard]] exo_container<std::string> getModulesFromPath(const std::string& path) const;

    [[nodiscard]] const ErrorInfo_ifs* getErrorInfo() const override;

   private:
    DataSchema_ifs* info_schema_ = nullptr;
    ParameterFieldTree_ifs* info_data_ = nullptr;

    std::string name_;
    exo_set<std::string> modules_;
    exo_map<std::string, Parameter_ifs*> parameters_;
};

#endif  // CONVERSIONTEMPLATE_H
