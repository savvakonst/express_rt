#ifndef EXRT_DEVICELIST_H
#define EXRT_DEVICELIST_H

#include <iterator>
#include <string>

#include "Device.h"
#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"

class DeviceManager : public BaseClass_ifs {
   public:
    void addConversionTemplate(Device* device) {
        vector_.push_back(device);
        emit_();
    }

    [[nodiscard]] size_t getEntriesNumber() const { return vector_.size(); }

    Device* getConversionTemplateByIndex(size_t index) {
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

    size_t getIndex(Device* ptr) {
        size_t index = 0;
        for (auto i : vector_) {
            if (i == ptr) return index;
            index++;
        }
        return std::numeric_limits<size_t>::max();
    }

    bool removeConversionTemplate(Device* device) {
        if (device == nullptr) return false;
        auto new_end = remove(vector_.begin(), vector_.end(), device);
        delete device;
        emit_();
    }

    [[nodiscard]] const std::vector<Device*>& getConversionTemplateList() const { return vector_; }

   private:
    std::vector<Device*> vector_;
};

#endif
