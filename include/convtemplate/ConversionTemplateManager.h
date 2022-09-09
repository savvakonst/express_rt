#ifndef EXRT_CONVERSIONTEMPLATEMANAGER_H
#define EXRT_CONVERSIONTEMPLATEMANAGER_H

#include <iterator>
#include <string>

#include "ThreadedATI.h"
#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"
#include "common/StringProcessingTools.h"
#include "convtemplate/ConversionTemplate.h"

struct CTNode {
    ConversionTemplate* cnv_template;
    Ati* tree;
};

int comparison(void* arg_a, void* arg_b) {
    auto* a = (CTNode*)arg_a;
    auto* b = (CTNode*)arg_b;

    std::string a_str =
        a->tree ? std::string((char*)a->cnv_template) : a->cnv_template->getPropertyAsTxt("common/name");

    std::string b_str =
        b->tree ? std::string((char*)b->cnv_template) : b->cnv_template->getPropertyAsTxt("common/name");

    return a_str < b_str;
}

class ConversionTemplateManager : public BaseClass_ifs {
   public:
    ConversionTemplateManager() { tree_ = newAti(comparison); }

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
        emit_();
        return true;
    }

    bool removeConversionTemplateByPath(size_t index) {
        if (index >= vector_.size()) {
            error_message_ = "index is out of range";
            return false;
        }
        vector_.erase(vector_.begin() + index);
        emit_();
        return true;
    }

    std::vector<int> getIndexesOfPath(const std::string& path) {
        std::vector<int> ret;

        auto chunks = split(path, '/');

        for (const auto& i : chunks) {
        }
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
    Ati* tree_ = nullptr;

    std::vector<ConversionTemplate*> vector_;
};

#endif
