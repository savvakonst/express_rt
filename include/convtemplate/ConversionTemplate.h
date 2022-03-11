#ifndef CONVERSIONTEMPLATE_H
#define CONVERSIONTEMPLATE_H

#include <string>

#include "common/BaseClass_ifs.h"
#include "common/Port.h"

class ExtensionManager;
class Parameter_ifs;
class DataSchema_ifs;

class COMMON_API_ ConversionTemplate : public BaseClass_ifs {
   public:
    explicit ConversionTemplate(ExtensionManager* manager);

    /*
     *  base description voids
     *
     */

    status setName(const std::string& name);
    status addInfo(const std::string& path, const Value& value);
    static status addInfo(const std::string& path, const std::string& value);

    [[nodiscard]] const HierarchicalData_ifs* getInfo(const std::string& path) const;
    [[nodiscard]] const DataSchema_ifs* getInfoSchema() const;
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
    [[nodiscard]] const exo_container<std::string> getModulesFromPath(const std::string& path) const;

    const ErrorInfo_ifs* getErrorInfo() const override;

   private:
    DataSchema_ifs* info_schema_ = nullptr;
    HierarchicalData_ifs* info_data_ = nullptr;

    std::string name_;
    exo_set<std::string> modules_;
    exo_map<std::string, Parameter_ifs*> parameters_;
};

#endif  // CONVERSIONTEMPLATE_H
