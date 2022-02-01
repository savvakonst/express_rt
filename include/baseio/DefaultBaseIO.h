#ifndef DEFAULTPARSERBUILDER_H
#define DEFAULTPARSERBUILDER_H

#include "common/BaseClass_ifs.h"
#include "common/DataSchema_ifs.h"

class ExtensionManager;
class PDefaultBaseIO_ifs;
class ConversionTemplate;

class DefaultBaseIO : public BaseClass_ifs {
   public:
    DefaultBaseIO();

    ~DefaultBaseIO();

    virtual ConversionTemplate *parseDocument(ExtensionManager *manager, const std::string &str);

    std::string createDocument(const ConversionTemplate *conv_template);

    [[nodiscard]] const ErrorInfo_ifs *getErrorInfo() const override { return nullptr; };

    void addPPBM(PDefaultBaseIO_ifs *p);

   private:
    typedef std::vector<const PDefaultBaseIO_ifs *> PPBList;
    std::map<uint32_t, PPBList *> PPBMap_;

    [[nodiscard]] const PPBList *getPPBMList(uint32_t type) const;
};

#endif  // !DEFAULTPARSERBUILDER_H
