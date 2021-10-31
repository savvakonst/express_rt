#ifndef EXO_DEVICE_H
#define EXO_DEVICE_H
#include <string>

#include "common/BaseClass_ifs.h"
#include "common/Port.h"
#include "device/Module_ifs.h"

class Module_ifs;

class DeviceBuildingContext_ifs {
   public:
    DeviceBuildingContext_ifs() {}
    virtual moduleConstructor_f getConstructor() = 0;

    virtual Module_ifs* createMdule(const std::string& module_id, const void* ptr, size_t size,
                                    DeviceBuildingContext_ifs* context) {
        return context->getConstructor()(ptr, size, context);
    }
};

// i thin it will be better to inherit this class from Module_ifs
// and use task mapper.it needs change name TaskMapper to PackedStructMapper.
// and i think it is possiple to remove all task structures by replacing them with PackedStructMapper

class COMMON_API_ Device : public BaseClass_ifs {
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

    std::vector<Module_ifs*> modules_;

    TASK_HEADER task_header_;

   public:
    Device(const void* ptr, size_t size, DeviceBuildingContext_ifs* context);

    ~Device();

    exo_container<const Module_ifs*> getAllModules();

    const Module_ifs* getModuleFromPath(std::string name);

    exo_container<const Module_ifs*> getLineFromPath(std::string path);

    status checkValExistence(std::string path);

    status isTranciverEnabled();

    std::string getSrcAddress();
};

#endif