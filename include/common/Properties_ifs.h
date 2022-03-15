#ifndef EXRT_PROPERTIES_IFS_H
#define EXRT_PROPERTIES_IFS_H

#include <string>

#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"

class COMMON_API_ Properties_ifs : public BaseClass_ifs {
   public:
    ~Properties_ifs() override = default;

    [[nodiscard]] virtual const DataSchema_ifs *getPropertySchema() const = 0;

    [[nodiscard]] virtual const HierarchicalData_ifs *getProperty(const std::string &prop_path) const = 0;

    [[nodiscard]] virtual std::string getPropertyAsTxt(const std::string &prop_path) const = 0;

    virtual bool setProperty(const std::string &prop_path, const Value &value) = 0;

    // if hierarchical_data == nullptr it will try to add standard unit
    virtual bool setProperty(const std::string &prop_path, const HierarchicalData_ifs *hierarchical_data) = 0;

    virtual bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) = 0;

    virtual bool removeProperty(const std::string &prop_path) = 0;
};

#endif  // EXRT_PROPERTIES_IFS_H
