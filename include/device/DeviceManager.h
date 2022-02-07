//
// Created by SVK on 07.02.2022.
//

#ifndef EXRT_DEVICELIST_H
#define EXRT_DEVICELIST_H

#include <iterator>
#include <string>

#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"

class Device;

class DeviceList : public BaseClass_ifs {
   public:
    void addConversionTemplate(Device* conversion_template) {
        list_.push_back(conversion_template);
        emit_();
    }

    [[nodiscard]] size_t getEntriesNumber() const { return list_.size(); }

    Device* getConversionTemplateByIndex(size_t index) {
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
        Device* conversion_template = *it;
        return removeConversionTemplate(conversion_template);
    }

    size_t getIndex(Device* ptr) {
        size_t index = 0;
        for (auto i : list_) {
            if (i == ptr) return index;
            index++;
        }
        return std::numeric_limits<size_t>::max();
    }

    bool removeConversionTemplate(Device* conversion_template) {
        if (conversion_template == nullptr) return false;
        list_.remove(conversion_template);
        delete conversion_template;
        emit_();
    }

    [[nodiscard]] const std::list<ConversionTemplate*>& getConversionTemplateList() const { return list_; }

   private:
    std::list<Device*> list_;
};

#endif  // EXRT_DEVICELIST_H
