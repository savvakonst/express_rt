#ifndef EXRT_DEVICELIST_H
#define EXRT_DEVICELIST_H

#include <iterator>
#include <string>

#include "Device.h"
#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"
#include "common/StringProcessingTools.h"

class DeviceManager : public BaseClass_ifs {
   public:
    void addDevice(Device* device) {
        vector_.push_back(device);
        emit_();
    }

    [[nodiscard]] size_t getEntriesNumber() const { return vector_.size(); }

    Device* getDeviceByIndex(size_t index) {
        if (index >= vector_.size()) {
            return nullptr;
        }
        return vector_[index];
    }

    bool removeDeviceByIndex(size_t index) {
        if (index >= vector_.size()) {
            error_message_ = "index is out of range";
            return false;
        }
        vector_.erase(vector_.begin() + index);
        emit_();
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

    bool removeDevice(Device* device) {
        if (device == nullptr) return false;
        auto new_end = remove(vector_.begin(), vector_.end(), device);
        delete device;
        emit_();
    }

    [[nodiscard]] const std::vector<Device*>& getConversionTemplateList() const { return vector_; }

   private:
    std::vector<Device*> vector_;
};

bool Device::isChannelAvailable(const std::string& prop_path) {
    auto path = lastCharPos(prop_path, '/');
    auto modules = getSubModulesFromPath(path.first);  //::getSubmodules(this, path.first);

    if (modules.size() == 1) return modules.front()->isChannelAvailable(path.second);

    return false;
}

#endif
