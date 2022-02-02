#ifndef EXRT_ANALOG_H
#define EXRT_ANALOG_H

#include "convtemplate/ParameterFieldTree.h"
#include "convtemplate/StandardParameter_ifs.h"
#include "extensions/PDefaultBaseIO_ifs.h"

class ExtensionUnit;

class [[maybe_unused]] AnalogParameter : public StandardParameter_ifs {
   public:
    AnalogParameter(ExtensionManager* manager, const std::string& name);

    ~AnalogParameter() override;

    [[nodiscard]] PrmBuffer_ifs* createBuffer() const override;

    [[nodiscard]] std::string getType() const override;

    [[nodiscard]] bool isValid() const override;
};

/*
class [[maybe_unused]] AccelerationParameter : public StandardParameter_ifs {
   public:
    AccelerationParameter(ExtensionManager* manager, const std::string& name);

    ~AccelerationParameter() override;

    [[nodiscard]] PrmBuffer_ifs* createBuffer() const override;

    [[nodiscard]] std::string getType() const override;

    [[nodiscard]] bool isValid() const override;
};
*/
#endif  // EXRT_ANALOG_H
