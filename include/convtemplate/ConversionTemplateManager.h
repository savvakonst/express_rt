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
        vector_.push_back(conversion_template);
        emit_();
    }

    [[nodiscard]] size_t getEntriesNumber() const { return vector_.size(); }

    ConversionTemplate* getConversionTemplateByIndex(size_t index) {
        if (index >= vector_.size()) {
            return nullptr;
        }

        return vector_[index];
    }

    bool removeConversionTemplateByIndex(size_t index) {
        if (index >= vector_.size()) {
            error_message_ = "index is out of range";
            return false;
        }
        vector_.erase(vector_.begin() + index);
        return true;
    }

    size_t getIndex(ConversionTemplate* ptr) {
        size_t index = 0;
        for (auto i : vector_) {
            if (i == ptr) return index;
            index++;
        }
        return std::numeric_limits<size_t>::max();
    }

    bool removeConversionTemplate(ConversionTemplate* conversion_template) {
        if (conversion_template == nullptr) return false;
        auto new_end = remove(vector_.begin(), vector_.end(), conversion_template);
        delete conversion_template;
        emit_();
    }

    [[nodiscard]] const std::vector<ConversionTemplate*>& getConversionTemplateList() const { return vector_; }

   private:
    std::vector<ConversionTemplate*> vector_;
};

#endif  // EXRT_CONVERSIONTEMPLATEMANAGER_H
