#include "ksdconnected.h"

#include <memory>

#include "common/CustomTypes.h"
#define MAX_INPUT_BUFFER_SIZE (1 << 15)

const int32_t MAX_DEVICE_NUMBER = 16;
const int32_t TIMEOUT = 10;

// namespace udp
//{
typedef uint16_t signature_t;
typedef uint16_t lun_t;
typedef uint8_t request_t;

const size_t COMMAND_BLOCK_WRAPPER_SIZE = 64;
const signature_t COMMAND_BLOCK_WRAPPER_SIGNATURE = 0x4243;
const size_t COMMAND_STATUS_WRAPPER_SIZE = 64;
const signature_t COMMAND_STATUS_WRAPPER_SIGNATURE = 0x5343;
const size_t DATA_BLOCK_WRAPPER_SIZE = 32;
const signature_t DATA_BLOCK_WRAPPER_SIGNATURE = 0x4244;
const size_t DATA_STATUS_WRAPPER_SIZE = 32;
const signature_t DATA_STATUS_WRAPPER_SIGNATURE = 0x5344;

const lun_t LUN_ANY = 0x0000;

const uint16_t DEFAULT_PORT = 4880;

namespace request {
const request_t PING = 0x00;

const request_t READ_TASK_MEMORY = 0x01;
const request_t READ_RECORD_MAP = 0x02;

const request_t MIB_EXT_RWBUFFER = 0x10;
const request_t MIB_EXT_EXECUTE = 0x11;

const request_t MIB_GET_STATUS = 0x40;
const request_t MIB_GET_ID = 0x41;
const request_t MIB_GET_TASK = 0x42;

const request_t MIB_SET_TASK = 0x02;
}  // namespace request

typedef struct _PROTO_HEADER {
    uint16_t signature;
    uint16_t lun;
    uint16_t seqid;
    uint16_t seqcnt;  // Little endian
} PROTO_HEADER;

typedef struct _CBW {
    PROTO_HEADER proto;

    uint32_t dataTransferLength;  // Expected data transfer size by host

    uint8_t requestCode;
    uint8_t flags;  // 7    -- data stage direction (0 - host->device, 1 - device->host)
                    // 6    -- use BROADCAST_MAC:BROADCAST_IP:SENDER_PORT as target endpoint instead of
                    // SENDER_MAC:SENDER_IP:SENDER_PORT 5..0 -- currently reserved
    uint16_t reserved;

    CxW_Params params;
} CBW;

typedef struct _CSW {
    PROTO_HEADER proto;

    uint32_t dataTransferLength;
    uint32_t statusCode;

    CxW_Params params;
} CSW;

typedef struct _DBW {
    PROTO_HEADER proto;
    uint32_t dataTransferLength;
    uint32_t statusCode;

    uint32_t params[4];
} DBW;

typedef struct _DSW {
    PROTO_HEADER proto;
    uint32_t dataTransferLength;
    uint32_t statusCode;
    uint32_t params[4];
} DSW;

void prepareProto(PROTO_HEADER *proto, uint16_t signature, uint16_t lun, uint16_t seqID, uint16_t seqCnt);
bool validateProto(PROTO_HEADER *proto, uint16_t signature, uint16_t lun, uint16_t seqID, uint16_t seqCnt);
bool validateCSW(void *p, int32_t received, uint16_t lun, uint16_t seqID, uint16_t seqCnt, uint32_t tranferSize);
bool validateDBW(void *p, int32_t received, uint16_t lun, uint16_t seqID, uint16_t seqCnt, uint32_t tranferSize);
bool validateDSW(void *p, int32_t received, uint16_t lun, uint16_t seqID, uint16_t seqCnt, uint32_t tranferSize);

//-------------------------------------------------------------------------
KsdConnected::KsdConnected() { module_address_.address = 0; }
//-------------------------------------------------------------------------
KsdConnected::KsdConnected(const int index) {
    module_address_.address = 0;

    module_address_.block = static_cast<uint8_t>(index);
}

KsdConnected::KsdConnected(const int index, uint16_t lun, sockaddr candidate_sockaddr, SOCKET socket)
    : KsdConnected(index) {
    lun_ = lun;
    sockaddr_ = candidate_sockaddr;
    socket_ = socket;
}

//-------------------------------------------------------------------------
KsdConnected::~KsdConnected() { closeSocket(); }

void prepareProto(PROTO_HEADER *proto, uint16_t signature, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt) {
    proto->signature = signature;
    proto->lun = lun;
    proto->seqid = seq_id;
    proto->seqcnt = seq_cnt;
}
//-------------------------------------------------------------------------
bool validateProto(PROTO_HEADER *proto, uint16_t signature, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt) {
    bool result = false;
    result |= static_cast<bool>(proto->signature - signature);

    if (lun != LUN_ANY) result |= static_cast<bool>(proto->lun - lun);

    result |= static_cast<bool>(proto->seqid - seq_id);
    result |= static_cast<bool>(proto->seqcnt - seq_cnt);

    return result;
}
//-------------------------------------------------------------------------
bool validateCSW(void *p, int32_t received, uint16_t lun, uint16_t seqID, uint16_t seqCnt, uint32_t tranfer_size) {
    bool result = false;

    result |= static_cast<bool>(received - COMMAND_STATUS_WRAPPER_SIZE);

    CSW *csw = reinterpret_cast<CSW *>(p);
    result |= validateProto(&csw->proto, COMMAND_STATUS_WRAPPER_SIGNATURE, lun, seqID, seqCnt);

    if (csw->dataTransferLength > tranfer_size) result = true;

    result |= static_cast<bool>(csw->statusCode);

    return result;
}
//-------------------------------------------------------------------------
bool validateDBW(void *p, int32_t received, uint16_t lun, uint16_t seqID, uint16_t seqCnt, uint32_t tranferSize) {
    bool result = false;

    if (received < DATA_BLOCK_WRAPPER_SIZE) result = true;

    DBW *dbw = reinterpret_cast<DBW *>(p);
    result |= validateProto(&dbw->proto, DATA_BLOCK_WRAPPER_SIGNATURE, lun, seqID, seqCnt);

    received -= DATA_BLOCK_WRAPPER_SIZE;
    if (int32_t(dbw->dataTransferLength) > received) result = true;

    result |= static_cast<bool>(dbw->dataTransferLength - tranferSize);
    result |= static_cast<bool>(dbw->statusCode);

    return result;
}
//-------------------------------------------------------------------------
bool validateDSW(void *p, int32_t received, uint16_t lun, uint16_t seqID, uint16_t seqCnt, uint32_t tranferSize) {
    bool result = false;

    result |= static_cast<bool>(received - DATA_STATUS_WRAPPER_SIZE);

    DSW *dsw = reinterpret_cast<DSW *>(p);
    result |= validateProto(&dsw->proto, DATA_STATUS_WRAPPER_SIGNATURE, lun, seqID, seqCnt);
    result |= static_cast<bool>(dsw->dataTransferLength - tranferSize);
    result |= static_cast<bool>(dsw->statusCode);

    return result;
}
//-------------------------------------------------------------------------
bool KsdConnected::ioSyncGen(uint8_t request, uint8_t flags, void *data, uint32_t transferSize, uint32_t *ptransferred,
                             CxW_Params *cbw_params, CxW_Params *csw_params, int32_t timeout) {
    bool status = 0;
    int32_t transmitted = 0;
    uint32_t max_transfer_size = 1024;

    auto unique_buffer = std::unique_ptr<char[]>(new char[MAX_INPUT_BUFFER_SIZE]);
    char *buffer = unique_buffer.get();

    memset(buffer, 0, sizeof(buffer));

    sequence_id_++;
    sequence_cnt_ = 0;

    do {
        CBW Cbw;
        memset(&Cbw, 0, sizeof(Cbw));
        prepareProto(&Cbw.proto, COMMAND_BLOCK_WRAPPER_SIGNATURE, lun_, sequence_id_, sequence_cnt_);

        Cbw.dataTransferLength = transferSize;
        Cbw.requestCode = request;
        Cbw.flags = flags;

        if (cbw_params != NULL) Cbw.params = *cbw_params;

        memcpy(buffer, &Cbw, sizeof(Cbw));

        transmitted = ::sendto(socket_, buffer, sizeof(Cbw), 0, (sockaddr *)&sockaddr_, sizeof(sockaddr_));
        if (transmitted == SOCKET_ERROR) {
            setError("Error: sendto()");
            break;
        }
        sequence_cnt_++;

        sockaddr saFrom;
        int32_t fromLen = sizeof(saFrom);
        int32_t received = ::recvfrom(socket_, buffer, MAX_INPUT_BUFFER_SIZE, 0, &saFrom, &fromLen);
        if (received == SOCKET_ERROR) {
            setError("Error: recvfrom()");
            break;
        }

        CSW *csw = reinterpret_cast<CSW *>(buffer);
        status = validateCSW(buffer, received, lun_, sequence_id_, sequence_cnt_, transferSize);
        if (status) {
            setError("Error: validateCSW()");
            break;
        }
        sequence_cnt_++;

        transferSize = csw->dataTransferLength;
        if (transferSize == 0) break;

        if (csw_params != NULL) *csw_params = csw->params;

        uint32_t transferred = 0;

        if (flags & 0x80) {  // device -> host
            char *pdst = reinterpret_cast<char *>(data);
            do {
                uint32_t currentTransferSize = transferSize;
                if (currentTransferSize > max_transfer_size) currentTransferSize = max_transfer_size;

                memset(buffer, 0, sizeof(buffer));

                received = ::recvfrom(socket_, buffer, MAX_INPUT_BUFFER_SIZE, 0, &saFrom, &fromLen);
                if (received == SOCKET_ERROR) {
                    setError("Error: recvfrom()");
                    break;
                }
                status = validateDBW(buffer, received, lun_, sequence_id_, sequence_cnt_, currentTransferSize);
                if (status != 0) {
                    setError("Error: validateDBW()");
                    break;
                }
                sequence_cnt_++;

                const char *psrc = buffer + sizeof(DBW);

                memcpy(pdst, psrc, currentTransferSize);
                // std::uninitialized_copy_n(psrc, currentTransferSize, pdst);
                pdst += currentTransferSize;
                transferSize -= currentTransferSize;
                transferred += currentTransferSize;

                DSW *dsw = reinterpret_cast<DSW *>(buffer);
                prepareProto(&dsw->proto, DATA_STATUS_WRAPPER_SIGNATURE, lun_, sequence_id_, sequence_cnt_);
                // sequenceCnt ++;

                dsw->dataTransferLength = transferSize;
                dsw->statusCode = 0;

                transmitted =
                    ::sendto(socket_, buffer, sizeof(DSW), 0, (const sockaddr *)&sockaddr_, sizeof(sockaddr_));
                if (transmitted == SOCKET_ERROR) {
                    setError("Error: sendto()");
                    break;
                }
                sequence_cnt_++;
            } while (transferSize > 0);
        } else {  // host -> device
            const char *psrc = reinterpret_cast<const char *>(data);

            do {
                DBW *dbw = reinterpret_cast<DBW *>(buffer);
                char *pdst = buffer + sizeof(DBW);

                uint32_t currentTransferSize = transferSize;
                if (currentTransferSize > max_transfer_size) currentTransferSize = max_transfer_size;

                memcpy(pdst, psrc, currentTransferSize);
                // std::uninitialized_copy_n(psrc, currentTransferSize, pdst);
                psrc += currentTransferSize;

                prepareProto(&dbw->proto, DATA_BLOCK_WRAPPER_SIGNATURE, lun_, sequence_id_, sequence_cnt_);
                dbw->dataTransferLength = currentTransferSize;
                dbw->statusCode = 0;

                transmitted = ::sendto(socket_, buffer, (sizeof(DSW) + currentTransferSize), 0, (sockaddr *)&sockaddr_,
                                       sizeof(sockaddr_));
                if (transmitted == SOCKET_ERROR) {
                    setError("Error: sendto()");
                    break;
                }

                sequence_cnt_++;
                transferSize -= currentTransferSize;

                int32_t received = ::recvfrom(socket_, buffer, MAX_INPUT_BUFFER_SIZE, 0, &saFrom, &fromLen);
                if (received == SOCKET_ERROR) {
                    setError("Error: recvfrom()");
                    break;
                }

                status = validateDSW(buffer, received, lun_, sequence_id_, sequence_cnt_, transferSize);
                if (status != 0) {
                    setError("Error: validateDSW()");
                    break;
                }

                sequence_cnt_++;
                transferred += currentTransferSize;
            } while (transferSize > 0);
        }

        if (ptransferred != NULL) *ptransferred = transferred;
    } while (false);

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::ioRequestSync(uint8_t request, uint8_t flags, void *data, uint32_t transferSize,
                                 uint32_t *ptransfered, CxW_Params *cbw_params, CxW_Params *csw_params,
                                 int32_t timeout) {
    bool status = ioSyncGen(request, flags, data, transferSize, ptransfered, cbw_params, csw_params, timeout);

    if (status) setError("Error: ioRequestSync()");

    return status;
}
//-------------------------------------------------------------------------
uint32_t KsdConnected::mibMakeCommand(const uint8_t request, const uint8_t address, uint16_t length) {
    uint32_t command = 0;
    command |= (uint32_t(request) << 24);
    command |= (uint32_t(address) << 16);
    command |= uint32_t(length);

    return command;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibExec(const uint8_t request, const uint8_t address, uint16_t length) {
    CxW_Params cbw_params;
    memset(&cbw_params, 0, sizeof(cbw_params));

    cbw_params.u32[0] = mibMakeCommand(request, address, length);

    uint32_t transferred = 0;
    bool status = ioRequestSync(request::MIB_EXT_EXECUTE, 0x00, NULL, 0, &transferred, &cbw_params, nullptr, TIMEOUT);

    if (status) setError("Error: mibExec()");

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibReadBuffer(void *data, int32_t size, int32_t *p_transferred) {
    uint32_t transferred = 0;
    bool status = ioRequestSync(request::MIB_EXT_RWBUFFER, 0x80 /*host read*/, data, size, &transferred, nullptr,
                                nullptr, TIMEOUT);

    if (p_transferred != NULL) *p_transferred = transferred;
    else
        status |= static_cast<bool>(size - transferred);

    if (status) setError("Error: mibReadBuffer()");

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibWriteBuffer(void *data, int32_t size, int32_t *p_transferred) {
    uint32_t transferred = 0;
    bool status = ioRequestSync(request::MIB_EXT_RWBUFFER, 0x00, data, size, &transferred, nullptr, nullptr, TIMEOUT);

    if (p_transferred != NULL) *p_transferred = transferred;
    else
        status |= static_cast<bool>(size - transferred);

    if (status) setError("Error: mibWriteBuffer()");

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibExecRead(const uint8_t request, const uint8_t addr, uint16_t length, void *data, int32_t size,
                               int32_t *p_transferred) {
    bool status = mibExec(request, addr, length);

    if (!status) status = mibReadBuffer(data, size, p_transferred);
    else
        setError("Error: mibExecRead()");

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibWriteExec(const uint8_t request, const uint8_t addr, uint16_t length, void *data, int32_t size,
                                int32_t *p_transferred) {
    bool status = mibWriteBuffer(data, size, p_transferred);
    if (!status) status = mibExec(request, addr, length);
    else
        setError("Error: mibWriteExec()");
    return (status);
}
//-------------------------------------------------------------------------
bool KsdConnected::mibGetId(const uint8_t address, ModuleIdent *mid) {
    bool status = mibExecRead(request::MIB_GET_ID, address, sizeof(*mid), mid, sizeof(*mid), nullptr);
    if (status) setError("Error: mibGetId()");
    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibGetStatus(const uint8_t address, ModuleStatus *mst) {
    bool status = mibExecRead(request::MIB_GET_STATUS, address, sizeof(*mst), mst, sizeof(*mst), nullptr);
    if (status) setError("Error: mibGetStatus()");
    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibGetTask(const uint8_t address, void *p, uint32_t size) {
    bool status = mibExecRead(request::MIB_GET_TASK, address, size, p, size, nullptr);
    if (status) setError("Error: mibGetTask()");
    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibSetTask(const uint8_t address, void *p, uint16_t size) {
    bool status = mibWriteExec(request::MIB_SET_TASK, address, size, p, size, nullptr);
    if (status) setError("Error: mibSetTask()");
    return status;
}

//-------------------------------------------------------------------------
bool KsdConnected::readRecordModulesMap(void *p, uint32_t offset, uint32_t size, uint32_t &transferred) {
    CxW_Params cbw_params;
    cbw_params.u32[0] = offset;

    bool status =
        ioRequestSync(request::READ_RECORD_MAP, 0x80 /*host read*/, p, size, &transferred, &cbw_params, NULL, TIMEOUT);
    if (status) setError("Error: readRecordModulesMap()");

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::readTaskMemory(void *p, uint32_t offset, uint32_t size, uint32_t &transferred) {
    // uint32_t    tres    = 0;
    CxW_Params cbw_params;
    cbw_params.u32[0] = offset;

    bool status =
        ioRequestSync(request::READ_TASK_MEMORY, 0x80 /*host read*/, p, size, &transferred, &cbw_params, NULL, TIMEOUT);
    if (status) setError("Error: readTaskMemory()");

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::ask() {
    bool status = false;
    uint32_t transferred = 0;

    do {
        status = readRecordModulesMap(reinterpret_cast<void *>(&record_modules_map_), 0, sizeof(record_modules_map_),
                                      transferred);
        if (status) break;

        record_modules_map_buffer_.resize(sizeof(TNMLIB_RECORD_MODULES_MAP));
        // record_modules_map_buffer_.fill(0);

        TNMLIB_RECORD_MODULES_MAP *ksdRMM =
            reinterpret_cast<TNMLIB_RECORD_MODULES_MAP *>(record_modules_map_buffer_.data());

        for (int j = 0; j < TNMLIB_CFG_MAX_MODULES_ENTRIES; j++) {
            ModuleInfo *udpMI = reinterpret_cast<ModuleInfo *>(&record_modules_map_.module_info[j]);
            TNMLIB_RECORD_MODULE_INFO *ksdMI = reinterpret_cast<TNMLIB_RECORD_MODULE_INFO *>(&ksdRMM->MI[j]);

            ksdMI->dwModuleID = udpMI->mid.id;
            ksdMI->wSerialNumber = udpMI->mid.serial_number;
            ksdMI->wVersion = udpMI->mid.version;
            ksdMI->iMIBAddress = udpMI->mib_addr;
            ksdMI->uiStatus = udpMI->status;
        }

        uint32_t task_size;
        status = readTaskMemory(reinterpret_cast<void *>(&task_size), 0, sizeof(task_size), transferred);
        if (status) break;

        task_buffer_.resize(task_size);

        // buffer.data()
        status = readTaskMemory(task_buffer_.data(), 0, task_size, transferred);
        if (status) break;
    } while (false);

    return status;
}

//-------------------------------------------------------------------------
const std::vector<char> &KsdConnected::getTask() { return task_buffer_; }

//-------------------------------------------------------------------------
const std::vector<char> &KsdConnected::getRecordModulesMap() { return record_modules_map_buffer_; }

/*
 *
 *
 *
 */
void KsdConnected::closeSocket() {
    int status = ::closesocket(socket_);
    if (status) setError("Error: closesocket()");
    socket_ = INVALID_SOCKET;
}

//-------------------------------------------------------------------------
template <typename T>
std::string format(const char *c, T arg) {
    char str[100];
    sprintf_s(str, c, arg);
    return std::string(str);
}

//-------------------------------------------------------------------------
void setError(std::string &error, const std::string &error_msg) { error += error_msg + "\n"; }

//-------------------------------------------------------------------------
std::list<KsdConnected *> devicePing(const EthernetAddress &adapter_address, std::string &error_msg) {
    int32_t status = 0;

    SOCKET ping_socket = INVALID_SOCKET;

    auto unique_buffer = std::unique_ptr<char[]>(new char[MAX_INPUT_BUFFER_SIZE]);
    char *buffer = unique_buffer.get();

    std::list<KsdConnected *> devices;

    ping_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (ping_socket == INVALID_SOCKET) {
        setError(error_msg, "Error: socket() = INVALID_SOCKET");
        return {};
    }

    // Socket Settings
    int32_t buffer_size = 32 * 1024 * 1024;
    status = setsockopt(ping_socket, SOL_SOCKET, SO_RCVBUF, (char *)&buffer_size, sizeof(buffer_size));
    if (status) {
        setError(error_msg, format("Error: setsockopt() = 0x%04X", status));
        return {};
    }

    bool is_addr_reused = true;
    status = setsockopt(ping_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&is_addr_reused, sizeof(is_addr_reused));
    if (status) {
        setError(error_msg, format("Error: setsockopt() = 0x%04X", status));
        return {};
    }

    uint32_t timeout = 200;
    status = setsockopt(ping_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    if (status) {
        setError(error_msg, format("Error: setsockopt() = 0x%04X", status));
        return {};
    }

    uint32_t broadcast = 1;
    status = setsockopt(ping_socket, SOL_SOCKET, SO_BROADCAST, (char *)&broadcast, sizeof(broadcast));
    if (status != 0) {
        setError(error_msg, format("Error: setsockopt() = 0x%04X", status));
        return {};
    }

    // Bind

    // status = ::bind(sock_, (const sockaddr *)&dst_address_, sizeof(dst_address_));

    struct sockaddr_in sa_ping;
    memset(&sa_ping, 0, sizeof(sa_ping));

    sa_ping.sin_family = AF_INET;
    sa_ping.sin_addr.s_addr = INADDR_BROADCAST;
    sa_ping.sin_port = htons(DEFAULT_PORT);

    // TODO:


    struct sockaddr_in host_addr;
    host_addr.sin_family = AF_INET;
    host_addr.sin_addr.s_addr = adapter_address.ip;
    host_addr.sin_port = 0;

    bind(ping_socket, (SOCKADDR *)&host_addr, sizeof(host_addr));

    do {
        uint16_t sequence_cnt = 0;
        uint16_t sequence_id = 0;

        CBW ping_cbw;
        memset(&ping_cbw, 0, sizeof(ping_cbw));

        prepareProto(&ping_cbw.proto, COMMAND_BLOCK_WRAPPER_SIGNATURE, LUN_ANY, sequence_cnt, sequence_id);

        ping_cbw.dataTransferLength = 0x00000000;
        ping_cbw.requestCode = request::PING;
        ping_cbw.flags = 0x00;

        memcpy(buffer, &ping_cbw, sizeof(ping_cbw));

        sockaddr sa_from;
        int32_t from_len = sizeof(sa_from);
        int32_t transmitted = sendto(ping_socket, buffer, sizeof(ping_cbw), 0, (sockaddr *)&sa_ping, sizeof(sa_ping));
        if (transmitted == SOCKET_ERROR) {
            // setError(error,"Error: sendto()");
            break;
        }
        sequence_cnt++;

        do {
            int received = recvfrom(ping_socket, buffer, MAX_INPUT_BUFFER_SIZE, 0, &sa_from, &from_len);
            if (received == SOCKET_ERROR) {
                // setError(error_msg, format("Error: recvfrom(). wsa_error_code = 0x%04X",  WSAGetLastError()) );
                break;
            }

            const CSW *csw = reinterpret_cast<const CSW *>(buffer);
            bool b = validateCSW(buffer, received, LUN_ANY, sequence_id, sequence_cnt, 0);
            if (b) break;

            KsdConnected candidate;

            auto candidate_lun_ = csw->proto.lun;
            auto candidate_sockaddr_ = sa_from;

            auto candidate_socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (candidate_socket_ == INVALID_SOCKET) {
                setError(error_msg, "Error: socket(): INVALID_SOCKET");
                break;
            }

            // Socket Settings
            status = setsockopt(candidate_socket_, SOL_SOCKET, SO_RCVBUF, (char *)&buffer_size, sizeof(buffer_size));
            if (status) {
                setError(error_msg, format("Error: setsockopt() = 0x%04X", status));
                break;
            }

            status = setsockopt(candidate_socket_, SOL_SOCKET, SO_REUSEADDR, (char *)&is_addr_reused,
                                sizeof(is_addr_reused));
            if (status) {
                setError(error_msg, format("Error: setsockopt() = 0x%04X", status));
                break;
            }

            status = setsockopt(candidate_socket_, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
            if (status) {
                setError(error_msg, format("Error: setsockopt() = 0x%04X", status));
                break;
            }

            KsdConnected *dvc =
                new KsdConnected((int)devices.size(), candidate_lun_, candidate_sockaddr_, candidate_socket_);

            devices.push_back(dvc);
            dvc->ask();
        } while (devices.size() < MAX_DEVICE_NUMBER);

        // bRepeat = false;
        if (!devices.size()) {
            setError(error_msg, "Устройств не обнаружено.");
        }
    } while (false);

    status = closesocket(ping_socket);
    if (status) setError(error_msg, format("Error: closesocket() = 0x%04X", status));

    return devices;
}