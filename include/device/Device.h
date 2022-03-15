#ifndef EXO_DEVICE_H
#define EXO_DEVICE_H

#include <list>
#include <string>
//
#include "common/BaseClass_ifs.h"
#include "common/Port.h"
//
#include "Module_ifs.h"

class COMMON_API_ Device : public Module_ifs {
   private:
    struct DeviceDate {
        uint8_t day;
        uint8_t month;
        uint16_t year;
    };

    struct TaskHeader {
        uint32_t task_size;     //Общий размер задания, Б
        uint32_t header_size;   //Размер заголовка задания, = 64 Б
        uint16_t task_version;  //Версия задания
        uint16_t check_sum;     //Контрольная сумма задания
        char plane_type[16];    //Идентификатор борта
        DeviceDate rd_date;     //Дата
        uint8_t reserved_a[4];  //Резерв
        uint32_t plane_no;      //Номер борта
        uint16_t flight_no;     //Номер полёта
        uint16_t min_version;   //Минимальная версия
        uint32_t device_id;  //Идентификатор устройства сбора (режима функционирования)
        uint16_t sync_src;       //Номер слота источника синхронизации
        uint8_t reserved_c[14];  //Резерв
    };

    std::vector<Module_ifs *> modules_;

    TaskHeader task_header_{};
    size_t size_ = 0;

    std::string source_;

    friend class DeviceManager;

   public:
    Device(const void *ptr, size_t size, ExtensionManager *context);

    ~Device() override;

    void setSource(const std::string &source) { source_ = source; }
    [[nodiscard]] std::string getSource() const { return source_; }

    [[nodiscard]] bool hasTransceiver() const override;

    [[nodiscard]] EthernetSettings getSrcAddress() const override;

    [[nodiscard]] bool isAvailable() const override { return true; }

    [[nodiscard]] std::string getID() const override;

    [[nodiscard]] std::string getModulePath(bool full_path) const override { return getID(); }

    [[nodiscard]] Module_ifs *getParentModule() const override { return nullptr; }

    bool setParentModule(Module_ifs *parent_module) override { return false; }

    [[nodiscard]] std::list<Module_ifs *> getSubModulesFromPath(const std::string &modules_path) const override;

    /* not yet implemented inherited members  begin*/
    std::map<std::string, PrmBuffer_ifs *> getPrmBufferMap() override { return {}; }

    [[nodiscard]] const DataSchema_ifs *getPropertySchema() const override { return nullptr; }

    [[nodiscard]] std::string printProperties(const std::string &indent) const override { return ""; }

    [[nodiscard]] const HierarchicalData_ifs *getProperty(const std::string &prop_path) const override {
        return nullptr;
    }

    [[nodiscard]] std::string getPropertyAsTxt(const std::string &prop_path) const override { return ""; }

    bool setProperty(const std::string &prop_path, const Value &value) override { return false; }

    bool setProperty(const std::string &prop_path, const HierarchicalData_ifs *hierarchical_data) override {
        return false;
    }

    bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) override { return false; }

    bool removeProperty(const std::string &prop_path) override {return false;};

    /* not yet implemented inherited members  end*/

    [[nodiscard]] bool storeTaskToBuffer(void *pointer) const override { return false; }

    [[nodiscard]] size_t getTaskSize() const override;

    [[nodiscard]] std::vector<std::pair<std::string, Module_ifs *>> getSubModules() const override;

    ModuleStream_ifs *createModuleStream() override { return nullptr; }

    // TODO : remove this
    Module_ifs *getTopModule() {
        if (modules_.size()) return modules_.front();
        return nullptr;
    }
};

#endif