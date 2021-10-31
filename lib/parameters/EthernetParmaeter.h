#ifndef ETHERNETKSLPARAMETER_H_
#define ETHERNETKSLPARAMETER_H_
#include "common/Extension.h"
#include "convtemplate/Parameter_ifs.h"
#include "convtemplate/PrmBuffer_ifs.h"
#include "map"

class EthernetParameter : public Parameter_ifs {
   public:
    EthernetParameter(const std::string& name);

    ~EthernetParameter() {}

    PrmBuffer_ifs* createBuffer() const override;
    const std::string getType() const override;

    const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const InfoList& getPropertiesInfoList() override { return info_list_; };

    const bool isValid() const;

    const ErrorInfo_ifs* getErrorInfo() const override;

   private:
    static InfoList info_list_;
    std::map<std::string, SelfValue> fields_map;
};

#endif