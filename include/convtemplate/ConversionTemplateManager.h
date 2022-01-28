#ifndef EXRT_CONVERSIONTEMPLATEMANAGER_H
#define EXRT_CONVERSIONTEMPLATEMANAGER_H

#include <string>

#include "ConversionTemplate.h"
#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"

class ConversionTemplateManager : public BaseClass_ifs {
   public:
    void addConversionTemplate(ConversionTemplate* conversion_template) {
        list_.push_back(conversion_template);
        emit_();
    }

    bool removeConversionTemplateByIndex(size_t index) {
        if (index >= list_.size()) {
            error_mesadge_ = "index is out of range";
            return false;
        }
        ConversionTemplate* conversion_template = list_[index];
        return removeConversionTemplate(conversion_template);
    }

    bool removeConversionTemplate(ConversionTemplate* conversion_template) {
        if (conversion_template == nullptr) return false;
        list_.remove(conversion_template);
        delete conversion_template;
        emit_();
    }

    const std::list<ConversionTemplate*>& getConversionTemplateList() const { return list_; }

   private:
    std::list<ConversionTemplate*> list_;
};

#endif  // EXRT_CONVERSIONTEMPLATEMANAGER_H
