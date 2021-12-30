

#include "Module_DCU_.h"

#include <iostream>

#include "Device/Device.h"

Module_DCU_::Module_DCU_(const std::string &module_id)
    : KSDModule(  //
          TaskMapper({std::make_pair("header", header_map_)})),
      module_id_(module_id) {
    field_map_.setReferencePtr(&task_);
}

Module_DCU_::Module_DCU_(const std::string &module_id, const void *ptr, size_t size, DeviceBuildingContext_ifs *context)
    : Module_DCU_(module_id) {
    task_ = *((Task *)ptr);

    size_ = (size_t)task_.header.size;
    size_t offset = sizeof(Task);

    do {
        char *current_ptr = (char *)ptr + offset;
        MODULE_HEADER *header = (MODULE_HEADER *)(current_ptr);
        Module_ifs *module = context->createModule(stringId(header->id), current_ptr, (size_t)header->size, context);

        if (module == nullptr) {
            error_mesadge_ = "cant find module with \"" + stringId(header->id) + "\" id";
            return;
        }
        if (module->hasError()) {
            error_mesadge_ += "\n" + error_mesadge_;
        }
        offset += (size_t)header->size;
        modules_.push_back(module);
        std::cout << "\n" + stringId(header->id) << ":" << module->printProperties();
    } while (offset < size_);

    if (offset != getTaskSize()) {
        error_mesadge_ = "module invalid task size of " + getID() + " module";
    }
}

Module_DCU_::~Module_DCU_() {}

bool Module_DCU_::hasTransceiver() const {
    for (auto i : modules_) {
        if (i->hasTransceiver()) return true;
    }
    return false;
}

EthernetSettings Module_DCU_::getSrcAddress() const {
    for (auto i : modules_) {
        if (i->hasTransceiver()) return i->getSrcAddress();
    }
    return EthernetSettings();
}

const InfoList *Module_DCU_::getPropertiesInfoList() { return nullptr; }

ResValue Module_DCU_::getProperty(const std::string &prop_path) const { return KSDModule::getProperty(prop_path); }

std::string Module_DCU_::getPropertyAsTxt(const std::string &prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_DCU_::setProperty(const std::string &prop_path, Value value) {
    return KSDModule::setProperty(prop_path, value);
}

bool Module_DCU_::setPropertyAsTxt(const std::string &prop_path, const std::string &value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs *Module_DCU_::createModuleStream() {
    if (ethernet_stream_ == nullptr) ethernet_stream_ = new EthernetDCU_Stream(this);
    return ethernet_stream_;
};

const ErrorInfo_ifs *Module_DCU_::getErrorInfo(void) const { return nullptr; }

size_t Module_DCU_::getTaskSize() const {
    { return size_; }
}

/*
 *
 *
 *
 *
 */

EthernetDCU_Stream::EthernetDCU_Stream(Module_DCU_ *module) : module_(module) {
    sub_modules_ = new ModuleStream_ifs *[module->modules_.size() + 1];  // module->modules_.size();

    // TODO: need to choose only available modules
    auto ptr = sub_modules_;
    for (auto i : module->modules_)
        if (i->isAvailable()) {
            *(ptr++) = i->createModuleStream();
            number_of_modules_ += 1;
        }
    *(ptr) = nullptr;

    data_offset_ = 2 + 4 + number_of_modules_ * 2 + 2;
}

EthernetDCU_Stream::~EthernetDCU_Stream() {}

const size_t cnt_offset = 6;

void EthernetDCU_Stream::readFramePeace(ModuleStreamContext_ifs *context, char *ptr, size_t size) {
    uint32_t time = *((uint32_t *)(ptr + 2));

    if ((0x3ff & time) == 0) lock_ = false;
    if (lock_) return;

    ModuleStream_ifs **module_ptr = sub_modules_;

    uint16_t *cnt = (uint16_t *)(ptr + cnt_offset);
    uint16_t *cnt_end = cnt + number_of_modules_;

    uint16_t *data_ptr = (uint16_t *)(ptr + data_offset_);
    while (cnt != cnt_end) {

        if ((*cnt != 0) && (*module_ptr != nullptr))
            (*module_ptr)->readFramePeace(context, ( char *)data_ptr, *(cnt));
        module_ptr++;
        data_ptr += *cnt;
        cnt++;

    }
}
