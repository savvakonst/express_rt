#ifndef EXRT_DEVICELIST_H
#define EXRT_DEVICELIST_H

#include <iterator>
#include <string>

#include "Device_ifs.h"
#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"
#include "common/StringProcessingTools.h"

class DeviceManager : public BaseClass_ifs {
   public:
    void addDevice(Device_ifs* device) {
        vector_.push_back(device);
        emit_();
    }

    [[nodiscard]] size_t getEntriesNumber() const { return vector_.size(); }

    Device_ifs* getDeviceByIndex(size_t index) {
        if (index >= vector_.size()) {
            return nullptr;
        }
        return vector_[index];
    }

    Device_ifs* getDeviceByPath(const std::string& path) {
        for (auto i : vector_) {
            auto current_path = i->getSource() + "//";
            if (current_path == path) return i;
        }
        return nullptr;
    }

    bool removeDeviceByIndex(size_t index) {
        if (index >= vector_.size()) {
            error_message_ = "index is out of range";
            return false;
        }
        vector_.erase(vector_.begin() + ptrdiff_t(index));
        emit_();
        return true;
    }

    size_t getIndex(Device_ifs* ptr) {
        size_t index = 0;
        for (auto i : vector_) {
            if (i == ptr) return index;
            index++;
        }
        return std::numeric_limits<size_t>::max();
    }

    bool removeDevice(Device_ifs* device) {
        if (device == nullptr) return false;
        auto new_end = remove(vector_.begin(), vector_.end(), device);
        delete device;
        emit_();
    }

    [[nodiscard]] const std::vector<Device_ifs*>& getConversionTemplateList() const { return vector_; }

   private:
    std::vector<Device_ifs*> vector_;
};

#endif
