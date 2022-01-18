#ifndef PARAMETER_IFS_H
#define PARAMETER_IFS_H

#include <string>

#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/FieldInfo.h"

class PrmBuffer_ifs;
class Parameter_ifs;
class ExtensionManager;

template <class T>
Parameter_ifs *createParameter(ExtensionManager *manager, const std::string &name) {
    return new T(manager, name);
}

[[maybe_unused]] typedef Parameter_ifs *(*parameterConstructor_f)(ExtensionManager *manager, const std::string &name);

class COMMON_API_ Parameter_ifs : public BaseClass_ifs {
   public:
    explicit Parameter_ifs(const std::string &name) : name_(name){};

    virtual ~Parameter_ifs() = default;

    [[nodiscard]] virtual PrmBuffer_ifs *createBuffer() const = 0;

    [[nodiscard]] virtual std::string getType() const = 0;

    virtual const DataSchema_ifs *getPropertiesInfoList() = 0;

    [[nodiscard]] virtual const ResValue *getProperty(const std::string &prop_path) const = 0;

    [[nodiscard]] virtual std::string getPropertyAsTxt(const std::string &prop_path) const = 0;

    virtual bool setProperty(const std::string &prop_path, const Value &value) = 0;

    virtual bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) = 0;

    [[nodiscard]] virtual bool isValid() const = 0;

    [[nodiscard]] bool isLocked() const { return (bool)prm_buffer_; }

   protected:
    std::string name_ = std::string("");
    std::string path_ = std::string("");

    friend PrmBuffer_ifs;
    PrmBuffer_ifs *prm_buffer_ = nullptr;
};

extern std::vector<FieldInfo *> g_common_parameter_fields;

#endif