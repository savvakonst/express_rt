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

class COMMON_API_ DeviceBuildingContext_ifs {
   public:
    ~DeviceBuildingContext_ifs() {}

    virtual moduleConstructor_f getConstructor(const std::string &module_id) = 0;

    virtual Module_ifs *createModule(const std::string &module_id, const void *ptr, size_t size,
                                     DeviceBuildingContext_ifs *context) = 0;
};

// TODO: i think it will be better to inherit this class from Module_ifs
//  and use task mapper.it needs change name TaskMapper to PackedStructMapper.
//  and i think it is possible to remove all task structures by replacing them with PackedStructMapper

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
    Device(const void *ptr, size_t size, DeviceBuildingContext_ifs *context);

    ~Device() override;

    [[nodiscard]] bool hasTransceiver() const override;

    [[nodiscard]] EthernetSettings getSrcAddress() const override;

    /* not yet implemented inherited members  begin*/
    [[nodiscard]] virtual bool isAvailable() const = 0;

    [[nodiscard]] virtual std::string getID() const = 0;

    virtual std::map<std::string, PrmBuffer_ifs *> getPrmBufferMap() = 0;

    const DataSchema_ifs *getPropertySchema() override = 0;

    [[nodiscard]] virtual std::string printProperties(const std::string &indent = "") const = 0;

    [[nodiscard]] const HierarchicalData_ifs *getProperty(const std::string &prop_path) const override = 0;

    [[nodiscard]] std::string getPropertyAsTxt(const std::string &prop_path) const override = 0;

    bool setProperty(const std::string &prop_path, const Value &value) override = 0;

    bool setProperty(const std::string &prop_path, const HierarchicalData_ifs *hierarchical_data) override = 0;

    bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) override = 0;

    /* not yet implemented inherited members  end*/

    [[nodiscard]] std::vector<std::pair<std::string, Module_ifs *>> getSubModules() const override;

    [[nodiscard]] std::vector<std::pair<std::string, Module_ifs *>> getModulesFromPath(const std::string &name);

    [[nodiscard]] static status checkValExistence(const std::string &path);

    [[nodiscard]] const void *storeTaskToBuffer() const override;

    [[nodiscard]] size_t getTaskSize() const override;

    // TODO : remove this
    Module_ifs *getTopModule() {
        if (modules_.size()) return modules_.front();
        return nullptr;
    }
};

#endif