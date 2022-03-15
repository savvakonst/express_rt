#ifndef PARAMETER_IFS_H
#define PARAMETER_IFS_H

#include <string>

#include "common/Properties_ifs.h"

class PrmBuffer_ifs;
class Parameter_ifs;
class ExtensionManager;

template <class T>
Parameter_ifs *createParameter(ExtensionManager *manager) {
    T *tmp = new T(manager);
    return tmp;
}

[[maybe_unused]] typedef Parameter_ifs *(*parameterConstructor_f)(ExtensionManager *manager, const std::string &name);

class COMMON_API_ Parameter_ifs : public Properties_ifs {
   public:
    ~Parameter_ifs() override = default;

    [[nodiscard]] virtual PrmBuffer_ifs *createBuffer() const = 0;

    [[nodiscard]] virtual std::string getType() const = 0;

    [[nodiscard]] const DataSchema_ifs *getPropertySchema() const override = 0;

    [[nodiscard]] const HierarchicalData_ifs *getProperty(const std::string &prop_path) const override = 0;

    [[nodiscard]] std::string getPropertyAsTxt(const std::string &prop_path) const override = 0;

    bool setProperty(const std::string &prop_path, const Value &value) override = 0;

    bool setProperty(const std::string &prop_path, const HierarchicalData_ifs *hierarchical_data) override = 0;

    bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) override = 0;

    bool removeProperty(const std::string &prop_path) override = 0;

    [[nodiscard]] virtual bool isValid() const = 0;

    [[nodiscard]] bool isLocked() const { return (bool)prm_buffer_; }

   protected:
    friend PrmBuffer_ifs;
    PrmBuffer_ifs *prm_buffer_ = nullptr;
};

#endif