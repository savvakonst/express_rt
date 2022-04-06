
#include <cerrno>
#include <fstream>
#include <memory>
#include <string>
//
#include "KsdIO.h"
#include "common/ExtensionManager.h"
#include "device/DeviceManager.h"
#include "device/Device_ifs.h"
//
#include "KsdApi/tnmdefs.h"

KsdIO::KsdIO() : IO_ifs("*.ksd", "Ksd files", "") {}
KsdIO::~KsdIO() = default;

bool KsdIO::readDocument(ExtensionManager *manager, const std::string &source_path) {
    std::ifstream in(source_path, std::ios::in | std::ios::binary);

    struct {
        TNMLIB_DEVICE_VERSION_INFO version_info_;
        TNMLIB_DEVICE_RECORD_INFO record_info_;
        TNMLIB_RECORD_MODULES_MAP modules_map_;
    } header;

    struct {
        uint32_t data_offset_;
        uint32_t header_offset_;
    } offsets{0, 0};

    auto device_manager =
        (DeviceManager *)manager->getLastVersionExtensionUnit("device_manager", "device_manager")->ptr;

    if (in) {
        in.read((char *)&offsets, sizeof(offsets));

        in.seekg(std::streamsize(offsets.header_offset_ + sizeof(header)), std::ios::beg);
        auto task_size = offsets.data_offset_ - offsets.header_offset_ - sizeof(header);

        std::unique_ptr<char[]> u_buffer(new char[task_size]);
        in.read(u_buffer.get(), std::streamsize(task_size));

        std::unique_ptr<Device_ifs> device(new Device_ifs(u_buffer.get(), task_size, manager));

        device->setSource("file://" + source_path);
        device
            ->

            device_manager->addDevice(device.release());
        return true;
    }
    return false;
}

bool KsdIO::saveDocument(ExtensionManager *manager, const std::string &id, const std::string &dst_path) {
    return false;
}
