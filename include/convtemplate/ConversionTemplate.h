#ifndef CONVERSIONTEMPLATE_H
#define CONVERSIONTEMPLATE_H

#include <string>

#include "common/BaseClass_ifs.h"
#include "common/Port.h"

class Parameter_ifs;

class COMMON_API_ ConversionTemplate : public BaseClass_ifs {
   public:
    ConversionTemplate();

    /*
     *  base description voids
     *
     */

    status changeName(std::string name);
    status addHistoryInfo();

    [[nodiscard]] const HierarchicalData_ifs* getInfo(const std::string& path) const;

    /*
     *
     *
     */

    status addParameter(Parameter_ifs*);
    [[nodiscard]] const Parameter_ifs* getParameter(std::string name) const;
    [[nodiscard]] exo_container<const Parameter_ifs*> getSingleTypeParameters(std::string type) const;
    [[nodiscard]] exo_container<const Parameter_ifs*> getParametersFromPath(std::string searching_path) const;
    status changeParameterName(std::string old, std::string new_name);
    status removeParameter(std::string name);
    status removeParametersFromPath(std::string path);

    /*
     *
     *
     */

    status addModule(std::string path);
    status removeModulesFromPath(std::string path);
    status getModulesFromPath(std::string path);
    [[nodiscard]] const exo_container<std::string> getModulesFromPath(std::string path) const;

    const ErrorInfo_ifs* getErrorInfo() const override;

   private:
    std::string name_;
    exo_set<std::string> modules_;
    exo_map<std::string, Parameter_ifs*> parameters_;
};

#endif  // CONVERSIONTEMPLATE_H
