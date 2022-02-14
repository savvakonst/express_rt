#ifndef EXRT_KSDCONNECTED_H
#define EXRT_KSDCONNECTED_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include "KsdApi/tnmdefs.h"

struct ModuleIdent;
struct ModuleStatus;
struct CxW_Params;

struct SocketObj;

class KsdConnected {
   public:
    explicit KsdConnected();

    // this function doesn't take ownership of this sobj. it just copies the contents
    explicit KsdConnected(int index, uint16_t lun, const SocketObj *socket_obj);

    ~KsdConnected();

    uint32_t mibMakeCommand(uint8_t request, uint8_t address, uint16_t length);
    bool mibExec(uint8_t request, uint8_t address, uint16_t length);
    bool mibReadBuffer(void *data, int32_t size, int32_t *p_transferred);
    bool mibWriteBuffer(void *data, int32_t size, int32_t *p_transferred);

    bool mibExecRead(uint8_t request, uint8_t addr, uint16_t length, void *data, int32_t size, int32_t *p_transferred);
    bool mibWriteExec(uint8_t request, uint8_t addr, uint16_t length, void *data, int32_t size, int32_t *p_transferred);

    /*read module id to mid ptr. */
    bool mibGetId(uint8_t address, ModuleIdent *mid);

    /*read module status to mst ptr. */
    bool mibGetStatus(uint8_t address, ModuleStatus *mst);

    bool mibGetTask(uint8_t address, void *p, uint32_t size);
    bool mibSetTask(uint8_t address, void *p, uint16_t size);

    bool readRecordModulesMap(void *p, uint32_t offset, uint32_t size, uint32_t &transferred);
    bool readTaskMemory(void *p, uint32_t offset, uint32_t size, uint32_t &transferred);
    bool ask();

    const std::vector<char> &getTask();
    const std::vector<char> &getRecordModulesMap();

    bool hasError() { return !error_msg_.empty(); }
    std::string getErrorMsg() { return error_msg_; }

    void closeSocket();

   private:
    bool ioSyncGen(uint8_t request, uint8_t flags, void *data, uint32_t transfer_size, uint32_t *p_transferred,
                   CxW_Params *cbw_params, CxW_Params *csw_params, int32_t timeout);
    bool ioRequestSync(uint8_t request, uint8_t flags, void *data, uint32_t transfer_size, uint32_t *p_transfered,
                       CxW_Params *cbw_params, CxW_Params *csw_params, int32_t timeout);

    SocketObj *socket_obj_;

    uint16_t lun_ = 0;
    uint32_t device_id_ = 0;

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
