#ifndef EXRT_KSDCONNECTED_H
#define EXRT_KSDCONNECTED_H

#include <winsock2.h>

#include <list>
#include <map>
#include <string>
#include <vector>

#include "tnmdefs.h"

union ModuleAddress {
    uint32_t address;
    struct {
        uint8_t sub;
        uint8_t slot;
        uint8_t slave;
        uint8_t block;
    };
    bool operator==(const ModuleAddress &ref) const { return address == ref.address; }
};

#pragma pack(push, 1)

struct ModuleIdent {
    uint32_t id;
    uint16_t version;
    uint16_t serial_number;
};

struct ModuleStatus {
    uint32_t command;
    uint32_t status;
};

struct ModuleInfo {
    ModuleIdent mid;
    int8_t mib_addr;
    uint8_t status;
    uint16_t task_offset;
};

struct UdpRecordModulesMap {
    ModuleInfo module_info[16];
};

struct CxW_Params {
    uint32_t u32[12];
};

#pragma pack(pop)

class KsdConnected {
   public:
    explicit KsdConnected();
    explicit KsdConnected(const int index);

    explicit KsdConnected(const int index, uint16_t lun, sockaddr candidate_sockaddr, SOCKET socket);
    ~KsdConnected();

    uint32_t mibMakeCommand(const uint8_t request, const uint8_t address, uint16_t length);
    bool mibExec(const uint8_t request, const uint8_t address, uint16_t length);
    bool mibReadBuffer(void *data, int32_t size, int32_t *p_transferred);
    bool mibWriteBuffer(void *data, int32_t size, int32_t *p_transferred);

    bool mibExecRead(const uint8_t request, const uint8_t addr, uint16_t length, void *data, int32_t size,
                     int32_t *p_transferred);
    bool mibWriteExec(const uint8_t request, const uint8_t addr, uint16_t length, void *data, int32_t size,
                      int32_t *p_transferred);

    bool mibGetId(const uint8_t address, ModuleIdent *mid);
    bool mibGetStatus(const uint8_t address, ModuleStatus *mst);
    bool mibGetTask(const uint8_t address, void *p, uint32_t size);
    bool mibSetTask(const uint8_t address, void *p, uint16_t size);

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
                   CxW_Params *cbw_params, CxW_Params *csw_params, int32_t timeout);
    bool ioRequestSync(uint8_t request, uint8_t flags, void *data, uint32_t transferSize, uint32_t *ptransfered,
                       CxW_Params *cbw_params, CxW_Params *csw_params, int32_t timeout);

    ModuleAddress module_address_;

    SOCKET socket_;
    sockaddr sockaddr_;

    uint16_t lun_;
    uint32_t device_id_;

    UdpRecordModulesMap record_modules_map_;

    std::vector<char> task_buffer_;
    std::vector<char> record_modules_map_buffer_;

    uint16_t sequence_id_ = 0;
    uint16_t sequence_cnt_ = 0;

   private:
    void setError(const std::string &err_msg) { error_msg_ += err_msg + "\n"; }
    std::string error_msg_ = "";
};

std::list<KsdConnected *> devicePing(const EthernetAddress &host_addr, std::string &error_msg);

#endif  // EXRT_KSDCONNECTED_H
