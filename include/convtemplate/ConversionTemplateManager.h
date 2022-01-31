#ifndef EXRT_CONVERSIONTEMPLATEMANAGER_H
#define EXRT_CONVERSIONTEMPLATEMANAGER_H

#include <iterator>
#include <string>

#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"

class ConversionTemplate;

class ConversionTemplateManager : public BaseClass_ifs {
   public:
    void addConversionTemplate(ConversionTemplate* conversion_template) {
        list_.push_back(conversion_template);
        emit_();
    }

    [[nodiscard]] size_t getEntriesNumber() const { return list_.size(); }

    ConversionTemplate* getConversionTemplateByIndex(size_t index) {
        if (index >= list_.size()) {
            return nullptr;
        }
        auto it = list_.begin();
        std::advance(it, index);
        return *it;
    }

    bool removeConversionTemplateByIndex(size_t index) {
        if (index >= list_.size()) {
            error_message_ = "index is out of range";
            return false;
        }
        auto it = list_.begin();
        std::advance(it, index);
        ConversionTemplate* conversion_template = *it;
        return removeConversionTemplate(conversion_template);
    }

    bool removeConversionTemplate(ConversionTemplate* conversion_template) {
        if (conversion_template == nullptr) return false;
        list_.remove(conversion_template);
        delete conversion_template;
        emit_();
    }

    [[nodiscard]] const std::list<ConversionTemplate*>& getConversionTemplateList() const { return list_; }

   private:
    std::list<ConversionTemplate*> list_;
};

#endif  // EXRT_CONVERSIONTEMPLATEMANAGER_H
