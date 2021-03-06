#include <algorithm>
#include <sstream>
//
#include "Module_DCU_.h"
#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"
#include "device/Device_ifs.h"

class EthernetDCU_Stream : public ModuleStream_ifs {
   public:
    explicit EthernetDCU_Stream(Module_DCU_ *module);

    ~EthernetDCU_Stream() override;

    void readFramePeace(ModuleStreamContext_ifs *context, char *ptr, size_t size) override;

    int getStatistic() override {
        // TODO:
        return 0;
    }

    [[nodiscard]] const RelativeTime &getTime() const override;

    const Module_ifs *getModule() override { return module_; }

   protected:
    bool lock_ = true;

    RelativeTime time_{0, 0};

    Module_ifs *module_;
    ModuleStream_ifs **sub_streams_;
    size_t number_of_slots_ = 0;
    size_t data_offset_ = 0;
};

EthernetDCU_Stream::EthernetDCU_Stream(Module_DCU_ *module) : module_(module) {
    sub_streams_ = new ModuleStream_ifs *[module->modules_.size() + 1];  // module->modules_.size();

    // TODO: need to choose only available modules
    auto ptr = sub_streams_;
    for (auto i : module->modules_)
        if (i->isAvailable()) {
            *(ptr++) = i->createModuleStream();
            number_of_slots_ += 1;
        }
    *(ptr) = nullptr;

    time_ = {0, 0};

    data_offset_ = 2 + 4 + number_of_slots_ * 2 + 2;
}

EthernetDCU_Stream::~EthernetDCU_Stream()  {
    module_->deattachModuleStream();

    for(size_t i = 0 ; i<number_of_slots_; i++ )
        delete sub_streams_[i];
}

const size_t g_cnt_offset = 6;

const RelativeTime &EthernetDCU_Stream::getTime() const { return time_; }

void EthernetDCU_Stream::readFramePeace(ModuleStreamContext_ifs *context, char *ptr, size_t size) {
    uint32_t time = *((uint32_t *)(ptr + 2));

    if ((0x3ff & time) == 0) lock_ = false;
    if (lock_) return;

    time_ = time_ + RelativeTime{1 << (32 - 10), 0};

    ModuleStream_ifs **module_ptr = sub_streams_;

    auto *cnt = (uint16_t *)(ptr + g_cnt_offset);
    uint16_t *cnt_end = cnt + number_of_slots_;

    auto *data_ptr = (uint16_t *)(ptr + data_offset_);
    while (cnt != cnt_end) {
        if ((*cnt != 0) && (*module_ptr != nullptr)) (*module_ptr)->readFramePeace(context, (char *)data_ptr, *(cnt));
        module_ptr++;
        data_ptr += *cnt;
        cnt++;
    }
}

/*
 *
 *
 *
 *
 *
 */


static std::string createCHxxId(size_t slots) {
    char data[3] = {0, 0, 0};
    std::sprintf(data, "%02d", int(slots));
    return "CH" + std::string(data);
}

Module_DCU_::Module_DCU_(const std::string &module_id)
    : KSDModule(  //
          TaskMapper({std::make_pair("header", header_map_)})),
      module_id_(module_id) {
    field_map_.setReferencePtr(&task_);
}

Module_DCU_::Module_DCU_(size_t number_of_slots, const void *ptr, size_t size, ExtensionManager *manager)
    : Module_DCU_(createCHxxId(number_of_slots)) {
    task_ = *((Task *)ptr);
    number_of_slots_ = number_of_slots + 1;
    size_ = (size_t)task_.header.size;
    size_t offset = sizeof(Task);

    do {
        char *current_ptr = (char *)ptr + offset;
        auto *header = (MODULE_HEADER *)(current_ptr);

        auto id = stringId(header->id);
        auto constructor = (moduleConstructor_f)manager->getLastVersionExtensionObject("module", id);
        Module_ifs *module = constructor ? constructor(current_ptr, (size_t)header->size, manager) : nullptr;

        if (module == nullptr) {
            error_message_ = "cant find module with \"" + stringId(header->id) + "\" id";
            return;
        }
        module->setParentModule(this);

        if (module->hasError()) error_message_ += "\n" + error_message_;

        offset += (size_t)header->size;
        modules_.push_back(module);
        // std::cout << "\n" + stringId(header->id) << ":" << module->printProperties("");
    } while (offset < size_);

    if (offset != size_) {
        error_message_ = "module invalid task size of " + getID() + " module";
    }
}

Module_DCU_::~Module_DCU_() = default;

bool Module_DCU_::hasTransceiver() const {
    // TODO:  try to use:// return std::any_of(modules_.begin(),modules_.end(), [](auto i){ return i->hasTransceiver();
    // });  // instead
    for (auto i : modules_)
        if (i->hasTransceiver()) return true;
    return false;
}

EthernetSettings Module_DCU_::getSrcAddress() const {
    for (auto i : modules_) {
        if (i->hasTransceiver()) return i->getSrcAddress();
    }
    return {};
}

std::list<Module_ifs *> Module_DCU_::getSubModulesFromPath(const std::string &modules_path) const {
    std::list<Module_ifs *> ret_list;
    for (auto i : modules_) ret_list.splice(ret_list.cend(), ::getSubmodules(i, modules_path));

    return ret_list;
}

const DataSchema_ifs *Module_DCU_::getPropertySchema() const { return nullptr; }

const HierarchicalData_ifs *Module_DCU_::getProperty(const std::string &prop_path) const {
    return KSDModule::getProperty(prop_path);
}

std::string Module_DCU_::getPropertyAsTxt(const std::string &prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_DCU_::setProperty(const std::string &prop_path, const Value &value) {
    return KSDModule::setProperty(prop_path, value);
}

bool Module_DCU_::setPropertyAsTxt(const std::string &prop_path, const std::string &value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

size_t Module_DCU_::getTaskSize() const {
    { return size_; }
}

ModuleStream_ifs *Module_DCU_::createModuleStream() {
    if (ethernet_stream_ != nullptr) return nullptr;
    ethernet_stream_ = new EthernetDCU_Stream(this);
    return ethernet_stream_;
}

std::vector<std::pair<std::string, Module_ifs *>> Module_DCU_::getSubModules() const {
    std::vector<std::vector<std::pair<size_t, Module_ifs *>>> temp_vector(
        number_of_slots_, std::vector<std::pair<size_t, Module_ifs *>>());

    for (auto i : modules_) {
        auto slot = i->getProperty("header/slot")->getValue().value_.u64;
        auto sub_slot = i->getProperty("header/sub")->getValue().value_.u64;
        temp_vector[slot].emplace_back(sub_slot, i);
    }

    std::vector<std::pair<std::string, Module_ifs *>> ret;
    ret.reserve(number_of_slots_);
    for (size_t slot = 0; slot < number_of_slots_; slot++) {
        auto &v = temp_vector[slot];
        if (v.empty()) ret.emplace_back(std::to_string(slot), nullptr);
        else {
            for (auto &i : v) {
                auto sub_slot = i.first;
                auto path = std::to_string(slot) + ((sub_slot == 0) ? "" : "." + std::to_string(sub_slot));
                ret.emplace_back(path, i.second);
            }
        }
    }

    return ret;
}

const ErrorInfo_ifs *Module_DCU_::getErrorInfo() const { return nullptr; }

bool Module_DCU_::isChannelAvailable(const std::string &prop_path) const {
    auto path = lastCharPos(prop_path, '/');
    auto modules = getSubModulesFromPath(path.first);  //::getSubmodules(this, path.first);
    if (modules.size() == 1) return modules.front()->isChannelAvailable(path.second);
    return false;
}

/*
 *
 *
 *
 *
 */

