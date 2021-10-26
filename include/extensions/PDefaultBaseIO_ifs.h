#ifndef MPARSERBUILDER_H
#define MPARSERBUILDER_H

#include "convtemplate/Parameter_ifs.h"

enum ParseStatus : size_t
{
    success = 0,
    invalid_configuration = 1,
    parsing_error = 2
};

class PDefaultBaseIO_ifs {
   public:
    virtual std::string getTypeIdentifier() const = 0;
    virtual int getPrmType() const = 0;

    virtual Parameter_ifs* Parse(HierarchicalData_ifs* header, HierarchicalData_ifs* other) const = 0;
};

#endif