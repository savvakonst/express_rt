#ifndef QKSDCONNECTED_H
#define QKSDCONNECTED_H

#include <winsock2.h>

#include <list>
#include <map>
#include <string>
#include <vector>

#include "tnmdefs.h"

typedef union _MODULE_ADDRESS {
    uint32_t address;
    struct {
        uint8_t sub;
        uint8_t slot;
        uint8_t slave;
        uint8_t block;
    };

    bool operator==(const _MODULE_ADDRESS &ref) const { return address == ref.address; }
} MODULE_ADDRESS;

#pragma pack(push, 1)

typedef struct _MODULE_IDENT {
    uint32_t u32ID;
    uint16_t u16Version;
    uint16_t u16SerialNumber;
} MODULE_IDENT;

typedef struct _MODULE_STATUS {
    uint32_t command;
    uint32_t status;
} MODULE_STATUS;

typedef struct _MODULE_INFO {
    MODULE_IDENT mid;
    int8_t i8MIBAddr;
    uint8_t u8Status;
    uint16_t u16TaskOffset;
} MODULE_INFO;

typedef struct _UDP_RECORD_MODULES_MAP {
    MODULE_INFO MI[16];
} UDP_RECORD_MODULES_MAP;

typedef struct _CxW_PARAMS {
    uint32_t u32[12];
} CxW_PARAMS;

#pragma pack(pop)

class KsdConnected {
   public:
    explicit KsdConnected();
    explicit KsdConnected(const int index);

    explicit KsdConnected(const int index, uint16_t lun, sockaddr candidate_sockaddr, SOCKET socket);
    ~KsdConnected();

    uint32_t MIB_MakeCommand(const uint8_t request, const uint8_t address, uint16_t length);
    bool MIB_Exec(const uint8_t request, const uint8_t address, uint16_t length);
    bool MIB_ReadBuffer(void *data, int32_t size, int32_t *p_transferred);
    bool MIB_WriteBuffer(void *data, int32_t size, int32_t *p_transferred);

    bool MIB_ExecRead(const uint8_t request, const uint8_t addr, uint16_t length, void *data, int32_t size,
                      int32_t *p_transferred);
    bool MIB_WriteExec(const uint8_t request, const uint8_t addr, uint16_t length, void *data, int32_t size,
                       int32_t *p_transferred);

    bool MIB_GetID(const uint8_t address, MODULE_IDENT *mid);
    bool MIB_GetSTATUS(const uint8_t address, MODULE_STATUS *mst);
    bool MIB_GetTASK(const uint8_t address, void *p, uint32_t size);
    bool MIB_SetTASK(const uint8_t address, void *p, uint16_t size);

    bool readRecordModulesMap(void *p, uint32_t offset, uint32_t size, uint32_t &transferred);
    bool readTaskMemory(void *p, uint32_t offset, uint32_t size, uint32_t &transferred);
    bool ask();

    const std::vector<char> &getTask();
    const std::vector<char> &getRecordModulesMap();

    bool hasError() { return !error_msg_.empty(); }
    std::string getErrorMsg() { return error_msg_; }

    void closeSocket();

   private:
    bool ioSyncGen(uint8_t request, uint8_t flags, void *data, uint32_t transferSize, uint32_t *ptransferred,
                   CxW_PARAMS *cbw_params, CxW_PARAMS *csw_params, int32_t timeout);
    bool ioRequestSync(uint8_t request, uint8_t flags, void *data, uint32_t transferSize, uint32_t *ptransfered,
                       CxW_PARAMS *cbw_params, CxW_PARAMS *csw_params, int32_t timeout);

    MODULE_ADDRESS module_address_;

    SOCKET socket_;
    sockaddr sockaddr_;

    uint16_t lun_;
    uint32_t device_id_;

    UDP_RECORD_MODULES_MAP record_modules_map_;

    std::vector<char> task_buffer_;
    std::vector<char> record_modules_map_buffer_;

    uint16_t sequence_id_ = 0;
    uint16_t sequence_cnt_ = 0;

   private:
    void setError(const std::string &err_msg) { error_msg_ += err_msg + "\n"; }
    std::string error_msg_ = "";
};

std::list<KsdConnected *> devicePing(std::string &error_msg);

#endif  // QKSDCONNECTED_H
