#ifndef DEFAULTPARSERBUILDER_H
#define DEFAULTPARSERBUILDER_H

#include "common/BaseClass_ifs.h"
#include "common/IO_ifs.h"
class ExtensionManager;
class PDefaultBaseIO_ifs;
class ConversionTemplate;

class BaseIO : public IO_ifs {
   public:
    BaseIO();

    ~BaseIO() override;

    bool readDocument(ExtensionManager *manager, const std::string &source_path) override;
    bool saveDocument(const std::string &id, const std::string &dst_path) override;

    ConversionTemplate *parseDocument(ExtensionManager *manager, const std::string &str,
                                      const std::string &source_path = "");

    void addPpbm(PDefaultBaseIO_ifs *p);

   private:
    virtual ConversionTemplate *readOrParseDocument(ExtensionManager *manager, bool is_file, const std::string &str,
                                                    const std::string &source_path);

    typedef std::vector<const PDefaultBaseIO_ifs *> PPBList;
    std::map<uint32_t, PPBList *> PPBMap_;

    [[nodiscard]] const PPBList *getPPBMList(uint32_t type) const;
};

#endif  // !DEFAULTPARSERBUILDER_H
