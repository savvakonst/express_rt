#ifndef MPARSERBUILDER_H
#define MPARSERBUILDER_H

#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/Parameter_ifs.h"

enum ParseStatus : size_t
{
    success = 0,
    invalid_configuration = 1,
    parsing_error = 2
};

class PDefaultBaseIO_ifs {
   public:
    [[nodiscard]] virtual std::string getTypeIdentifier() const = 0;
    [[nodiscard]] virtual int getPrmType() const = 0;

    virtual Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other, HierarchicalData_ifs* header,
                                 const std::string& path_to_module = 0) const = 0;
};

#endif