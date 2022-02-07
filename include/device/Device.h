#ifndef EXO_DEVICE_H
#define EXO_DEVICE_H

#include <string>
//
#include "common/BaseClass_ifs.h"
#include "common/Port.h"
//
#include "Module_ifs.h"

inline std::string stringId(uint32_t id) {
    char char_id[5] = {0, 0, 0, 0, 0};
    *((uint32_t *)char_id) = id;
    return std::string(char_id);
}

class COMMON_API_ Device : public Module_ifs {
   private:
    struct DEVICE_DATE {
        uint8_t day;
        uint8_t month;
        uint16_t year;
    };

    struct TASK_HEADER {
        uint32_t taskSize;      //Общий размер задания, Б
        uint32_t headerSize;    //Размер заголовка задания, = 64 Б
        uint16_t taskVersion;   //Версия задания
        uint16_t checkSumm;     //Контрольная сумма задания
        char planeType[16];     //Идентификатор борта
        DEVICE_DATE rdDate;     //Дата
        uint8_t reserved_a[4];  //Резерв
        uint32_t planeNo;       //Номер борта
        uint16_t flightNo;      //Номер полёта
        uint16_t minVersion;    //Минимальная версия
        uint32_t deviceID;  //Идентификатор устройства сбора (режима функционирования)
        uint16_t syncSrc;        //Номер слота источника синхронизации
        uint8_t reserved_c[14];  //Резерв
    };

    std::vector<Module_ifs *> modules_;

    TASK_HEADER task_header_;

    size_t size_ = 0;

   public:
    Device(const void *ptr, size_t size, ExtensionManager *context);

    ~Device() override;

    [[nodiscard]] bool hasTransceiver() const override;

    [[nodiscard]] EthernetSettings getSrcAddress() const override;

    [[nodiscard]] bool isAvailable() const override { return true; }

    [[nodiscard]] std::string getID() const override { return "KSD"; }

    /* not yet implemented inherited members  begin*/
    std::map<std::string, PrmBuffer_ifs *> getPrmBufferMap() override { return {}; }

    const DataSchema_ifs *getPropertySchema() override { return nullptr; }

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

    /* not yet implemented inherited members  end*/

    [[nodiscard]] bool storeTaskToBuffer(void *pointer) const override { return false; }

    [[nodiscard]] size_t getTaskSize() const override;

    [[nodiscard]] std::vector<std::pair<std::string, Module_ifs *>> getSubModules() const override;

    ModuleStream_ifs *createModuleStream() override { return nullptr; }

    /* */
    [[nodiscard]] std::vector<std::pair<std::string, Module_ifs *>> getModulesFromPath(const std::string &name);

    [[nodiscard]] static status checkValExistence(const std::string &path);

    // TODO : remove this
    Module_ifs *getTopModule() {
        if (modules_.size()) return modules_.front();
        return nullptr;
    }
};

#endif