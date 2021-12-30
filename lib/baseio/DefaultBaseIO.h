#ifndef DEFAULTPARSERBUILDER_H
#define DEFAULTPARSERBUILDER_H

#include "common/BaseClass_ifs.h"
#include "common/Fieldinfo.h"

class PDefaultBaseIO_ifs;

class ConversionTemplate;

class DefaultBaseIO : public BaseClass_ifs {
public:
    DefaultBaseIO();

    ~DefaultBaseIO();

    ConversionTemplate *parseDocument(const std::string &str);

    const std::string createDocument(const ConversionTemplate *conv_template);

    const ErrorInfo_ifs *getErrorInfo() const override { return nullptr; };

private:
    typedef std::vector<const PDefaultBaseIO_ifs *> PPBList;
    std::map<uint32_t, PPBList *> PPBMap_;

    void addPPBM(PDefaultBaseIO_ifs *p);

    const PPBList *getPPBMList(uint32_t type) const;
};

#endif  // !DEFAULTPARSERBUILDER_H
