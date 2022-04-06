//#include <winsock2.h>
//
#include <memory>
//
#include "common/CustomTypes.h"
#include "ksdconnected.h"

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

enum Request : request_t
{
    PING = 0x00,

    READ_TASK_MEMORY = 0x01,
    READ_RECORD_MAP = 0x02,

    MIB_EXT_RWBUFFER = 0x10,
    MIB_EXT_EXECUTE = 0x11,

    MIB_GET_STATUS = 0x40,
    MIB_GET_ID = 0x41,
    MIB_GET_TASK = 0x42,

    MIB_SET_TASK = 0x02,
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

struct ProtoHeader {
    uint16_t signature;
    uint16_t lun;
    uint16_t seq_id;
    uint16_t seq_cnt;  // Little endian
};

struct CBW {
    ProtoHeader proto;
    uint32_t data_transfer_length;  // Expected data transfer size by host
    uint8_t request_code;
    uint8_t flags;  // 7    -- data stage direction (0 - host->device, 1 - device->host)
                    // 6    -- use BROADCAST_MAC:BROADCAST_IP:SENDER_PORT as target endpoint instead of
                    // SENDER_MAC:SENDER_IP:SENDER_PORT 5..0 -- currently reserved
    uint16_t reserved;
    CxW_Params params;
};

struct CSW {
    ProtoHeader proto;
    uint32_t data_transfer_length;
    uint32_t status_code;
    CxW_Params params;
};

typedef struct _DBW {
    ProtoHeader proto;
    uint32_t data_transfer_length;
    uint32_t status_code;
    uint32_t params[4];
} DBW;

typedef struct _DSW {
    ProtoHeader proto;
    uint32_t data_transfer_length;
    uint32_t status_code;
    uint32_t params[4];
} DSW;

struct SocketObj {
    SOCKET socket_;
    sockaddr sockaddr_;
};

void prepareProto(ProtoHeader *proto, uint16_t signature, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt);
bool validateProto(ProtoHeader *proto, uint16_t signature, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt);
bool validateCSW(void *p, int32_t received, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt, uint32_t transfer_size);
bool validateDBW(void *p, int32_t received, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt, uint32_t transfer_size);
bool validateDSW(void *p, int32_t received, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt, uint32_t transfer_size);

//-------------------------------------------------------------------------
KsdConnected::KsdConnected() { socket_obj_ = new SocketObj; }

//-------------------------------------------------------------------------
KsdConnected::KsdConnected(int index, uint16_t lun, const SocketObj *socket_obj) : KsdConnected() {
    lun_ = lun;
    socket_obj_->sockaddr_ = socket_obj->sockaddr_;
    socket_obj_->socket_ = socket_obj->socket_;
}

//-------------------------------------------------------------------------
KsdConnected::~KsdConnected() {
    closeSocket();
    delete socket_obj_;
}

void prepareProto(ProtoHeader *proto, uint16_t signature, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt) {
    proto->signature = signature;
    proto->lun = lun;
    proto->seq_id = seq_id;
    proto->seq_cnt = seq_cnt;
}
//-------------------------------------------------------------------------
bool validateProto(ProtoHeader *proto, uint16_t signature, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt) {
    bool result = false;
    result |= static_cast<bool>(proto->signature - signature);

    if (lun != LUN_ANY) result |= static_cast<bool>(proto->lun - lun);

    result |= static_cast<bool>(proto->seq_id - seq_id);
    result |= static_cast<bool>(proto->seq_cnt - seq_cnt);

    return result;
}
//-------------------------------------------------------------------------
bool validateCSW(void *p, int32_t received, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt, uint32_t transfer_size) {
    bool result = false;

    result |= static_cast<bool>(received - COMMAND_STATUS_WRAPPER_SIZE);

    CSW *csw = reinterpret_cast<CSW *>(p);
    result |= validateProto(&csw->proto, COMMAND_STATUS_WRAPPER_SIGNATURE, lun, seq_id, seq_cnt);

    if (csw->data_transfer_length > transfer_size) result = true;

    result |= static_cast<bool>(csw->status_code);

    return result;
}
//-------------------------------------------------------------------------
bool validateDBW(void *p, int32_t received, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt, uint32_t transfer_size) {
    bool result = false;

    if (received < DATA_BLOCK_WRAPPER_SIZE) result = true;

    DBW *dbw = reinterpret_cast<DBW *>(p);
    result |= validateProto(&dbw->proto, DATA_BLOCK_WRAPPER_SIGNATURE, lun, seq_id, seq_cnt);

    received -= DATA_BLOCK_WRAPPER_SIZE;
    if (int32_t(dbw->data_transfer_length) > received) result = true;

    result |= static_cast<bool>(dbw->data_transfer_length - transfer_size);
    result |= static_cast<bool>(dbw->status_code);

    return result;
}
//-------------------------------------------------------------------------
bool validateDSW(void *p, int32_t received, uint16_t lun, uint16_t seq_id, uint16_t seq_cnt, uint32_t transfer_size) {
    bool result = false;

    result |= static_cast<bool>(received - DATA_STATUS_WRAPPER_SIZE);

    DSW *dsw = reinterpret_cast<DSW *>(p);
    result |= validateProto(&dsw->proto, DATA_STATUS_WRAPPER_SIGNATURE, lun, seq_id, seq_cnt);
    result |= static_cast<bool>(dsw->data_transfer_length - transfer_size);
    result |= static_cast<bool>(dsw->status_code);

    return result;
}
//-------------------------------------------------------------------------
bool KsdConnected::ioSyncGen(uint8_t request, uint8_t flags, void *data, uint32_t transfer_size,
                             uint32_t *p_transferred, CxW_Params *cbw_params, CxW_Params *csw_params, int32_t timeout) {
    bool status = false;
    int32_t transmitted = 0;
    uint32_t max_transfer_size = 1024;

    auto unique_buffer = std::unique_ptr<char[]>(new char[MAX_INPUT_BUFFER_SIZE]);
    char *buffer = unique_buffer.get();

    memset(buffer, 0, sizeof(buffer));

    sequence_id_++;
    sequence_cnt_ = 0;

    do {
        CBW cbw{};
        memset(&cbw, 0, sizeof(cbw));
        prepareProto(&cbw.proto, COMMAND_BLOCK_WRAPPER_SIGNATURE, lun_, sequence_id_, sequence_cnt_);

        cbw.data_transfer_length = transfer_size;
        cbw.request_code = request;
        cbw.flags = flags;

        if (cbw_params != nullptr) cbw.params = *cbw_params;

        memcpy(buffer, &cbw, sizeof(cbw));

        transmitted = ::sendto(socket_obj_->socket_, buffer, sizeof(cbw), 0, (sockaddr *)&socket_obj_->sockaddr_,
                               sizeof(socket_obj_->sockaddr_));
        if (transmitted == SOCKET_ERROR) {
            setError("Error: sendto()");
            break;
        }
        sequence_cnt_++;

        sockaddr sa_from{};
        int32_t from_len = sizeof(sa_from);
        int32_t received = ::recvfrom(socket_obj_->socket_, buffer, MAX_INPUT_BUFFER_SIZE, 0, &sa_from, &from_len);
        if (received == SOCKET_ERROR) {
            setError("Error: recvfrom()");
            break;
        }

        CSW *csw = reinterpret_cast<CSW *>(buffer);
        status = validateCSW(buffer, received, lun_, sequence_id_, sequence_cnt_, transfer_size);
        if (status) {
            setError("Error: validateCSW()");
            break;
        }
        sequence_cnt_++;

        transfer_size = csw->data_transfer_length;
        if (transfer_size == 0) break;

        if (csw_params != nullptr) *csw_params = csw->params;

        uint32_t transferred = 0;

        if (flags & 0x80) {  // device -> host
            char *p_dst = reinterpret_cast<char *>(data);
            do {
                uint32_t current_transfer_size = transfer_size;
                if (current_transfer_size > max_transfer_size) current_transfer_size = max_transfer_size;

                memset(buffer, 0, sizeof(buffer));

                received = ::recvfrom(socket_obj_->socket_, buffer, MAX_INPUT_BUFFER_SIZE, 0, &sa_from, &from_len);
                if (received == SOCKET_ERROR) {
                    setError("Error: recvfrom()");
                    break;
                }
                status = validateDBW(buffer, received, lun_, sequence_id_, sequence_cnt_, current_transfer_size);
                if (status != 0) {
                    setError("Error: validateDBW()");
                    break;
                }
                sequence_cnt_++;

                const char *p_src = buffer + sizeof(DBW);

                memcpy(p_dst, p_src, current_transfer_size);
                // std::uninitialized_copy_n(psrc, currentTransferSize, pdst);
                p_dst += current_transfer_size;
                transfer_size -= current_transfer_size;
                transferred += current_transfer_size;

                DSW *dsw = reinterpret_cast<DSW *>(buffer);
                prepareProto(&dsw->proto, DATA_STATUS_WRAPPER_SIGNATURE, lun_, sequence_id_, sequence_cnt_);
                // sequenceCnt ++;

                dsw->data_transfer_length = transfer_size;
                dsw->status_code = 0;

                transmitted = ::sendto(socket_obj_->socket_, buffer, sizeof(DSW), 0,
                                       (const sockaddr *)&socket_obj_->sockaddr_, sizeof(socket_obj_->sockaddr_));
                if (transmitted == SOCKET_ERROR) {
                    setError("Error: sendto()");
                    break;
                }
                sequence_cnt_++;
            } while (transfer_size > 0);
        } else {  // host -> device
            const char *psrc = reinterpret_cast<const char *>(data);

            do {
                DBW *dbw = reinterpret_cast<DBW *>(buffer);
                char *p_dst = buffer + sizeof(DBW);

                uint32_t current_transfer_size = transfer_size;
                if (current_transfer_size > max_transfer_size) current_transfer_size = max_transfer_size;

                memcpy(p_dst, psrc, current_transfer_size);
                // std::uninitialized_copy_n(psrc, currentTransferSize, pdst);
                psrc += current_transfer_size;

                prepareProto(&dbw->proto, DATA_BLOCK_WRAPPER_SIGNATURE, lun_, sequence_id_, sequence_cnt_);
                dbw->data_transfer_length = current_transfer_size;
                dbw->status_code = 0;

                transmitted = ::sendto(socket_obj_->socket_, buffer, int(sizeof(DSW) + current_transfer_size), 0,
                                       (sockaddr *)&socket_obj_->sockaddr_, sizeof(socket_obj_->sockaddr_));
                if (transmitted == SOCKET_ERROR) {
                    setError("Error: sendto()");
                    break;
                }

                sequence_cnt_++;
                transfer_size -= current_transfer_size;

                received = ::recvfrom(socket_obj_->socket_, buffer, MAX_INPUT_BUFFER_SIZE, 0, &sa_from, &from_len);
                if (received == SOCKET_ERROR) {
                    setError("Error: recvfrom()");
                    break;
                }

                status = validateDSW(buffer, received, lun_, sequence_id_, sequence_cnt_, transfer_size);
                if (status != 0) {
                    setError("Error: validateDSW()");
                    break;
                }

                sequence_cnt_++;
                transferred += current_transfer_size;
            } while (transfer_size > 0);
        }

        if (p_transferred != nullptr) *p_transferred = transferred;
    } while (false);

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::ioRequestSync(uint8_t request, uint8_t flags, void *data, uint32_t transfer_size,
                                 uint32_t *p_transferred, CxW_Params *cbw_params, CxW_Params *csw_params,
                                 int32_t timeout) {
    bool status = ioSyncGen(request, flags, data, transfer_size, p_transferred, cbw_params, csw_params, timeout);

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
    CxW_Params cbw_params{};
    memset(&cbw_params, 0, sizeof(cbw_params));

    cbw_params.u32[0] = mibMakeCommand(request, address, length);

    uint32_t transferred = 0;
    bool status =
        ioRequestSync(Request::MIB_EXT_EXECUTE, 0x00, nullptr, 0, &transferred, &cbw_params, nullptr, TIMEOUT);

    if (status) setError("Error: mibExec()");

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibReadBuffer(void *data, int32_t size, int32_t *p_transferred) {
    uint32_t transferred = 0;
    bool status = ioRequestSync(Request::MIB_EXT_RWBUFFER, 0x80 /*host read*/, data, size, &transferred, nullptr,
                                nullptr, TIMEOUT);

    if (p_transferred != nullptr) *p_transferred = int32_t(transferred);
    else
        status |= static_cast<bool>(size - transferred);

    if (status) setError("Error: mibReadBuffer()");

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibWriteBuffer(void *data, int32_t size, int32_t *p_transferred) {
    uint32_t transferred = 0;
    bool status = ioRequestSync(Request::MIB_EXT_RWBUFFER, 0x00, data, size, &transferred, nullptr, nullptr, TIMEOUT);

    if (p_transferred != nullptr) *p_transferred = int32_t(transferred);
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
    bool status = mibExecRead(Request::MIB_GET_ID, address, sizeof(*mid), mid, sizeof(*mid), nullptr);
    if (status) setError("Error: mibGetId()");
    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibGetStatus(const uint8_t address, ModuleStatus *mst) {
    bool status = mibExecRead(Request::MIB_GET_STATUS, address, sizeof(*mst), mst, sizeof(*mst), nullptr);
    if (status) setError("Error: mibGetStatus()");
    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibGetTask(const uint8_t address, void *p, uint32_t size) {
    bool status = mibExecRead(Request::MIB_GET_TASK, address, size, p, int32_t(size), nullptr);
    if (status) setError("Error: mibGetTask()");
    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::mibSetTask(const uint8_t address, void *p, uint16_t size) {
    bool status = mibWriteExec(Request::MIB_SET_TASK, address, size, p, size, nullptr);
    if (status) setError("Error: mibSetTask()");
    return status;
}

//-------------------------------------------------------------------------
bool KsdConnected::readRecordModulesMap(void *p, uint32_t offset, uint32_t size, uint32_t &transferred) {
    CxW_Params cbw_params{};
    cbw_params.u32[0] = offset;

    bool status = ioRequestSync(Request::READ_RECORD_MAP, 0x80 /*host read*/, p, size, &transferred, &cbw_params,
                                nullptr, TIMEOUT);
    if (status) setError("Error: readRecordModulesMap()");

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::readTaskMemory(void *p, uint32_t offset, uint32_t size, uint32_t &transferred) {
    CxW_Params cbw_params{};
    cbw_params.u32[0] = offset;

    bool status = ioRequestSync(Request::READ_TASK_MEMORY, 0x80 /*host read*/, p, size, &transferred, &cbw_params,
                                nullptr, TIMEOUT);
    if (status) setError("Error: readTaskMemory()");

    return status;
}
//-------------------------------------------------------------------------
bool KsdConnected::ask() {
    bool status = false;
    uint32_t transferred = 0;

    UdpRecordModulesMap record_modules_map{};

    do {
        status = readRecordModulesMap(reinterpret_cast<void *>(&record_modules_map), 0, sizeof(record_modules_map),
                                      transferred);
        if (status) break;

        record_modules_map_.resize(TNMLIB_CFG_MAX_MODULES_ENTRIES);
        // record_modules_map_.fill(0);

        // auto *ksd_rmm = reinterpret_cast<TNMLIB_RECORD_MODULES_MAP *>(record_modules_map_buffer_.data());

        // auto *ksd_rmm = reinterpret_cast<TNMLIB_RECORD_MODULES_MAP *>(record_modules_map_.data());

        for (int j = 0; j < TNMLIB_CFG_MAX_MODULES_ENTRIES; j++) {
            auto *udp_mi = reinterpret_cast<ModuleInfo *>(&record_modules_map.module_info[j]);
            auto ksd_mi = &record_modules_map_[j];
            ksd_mi->module_id = udp_mi->mid.id;
            ksd_mi->serial_number = udp_mi->mid.serial_number;
            ksd_mi->version = udp_mi->mid.version;
            ksd_mi->mib_address = int32_t(udp_mi->mib_addr);
            ksd_mi->status = udp_mi->status;
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

#include <iomanip>
#include <sstream>

std::string KsdConnected::getSource() const {
    auto *s = (sockaddr_in *)&(socket_obj_->sockaddr_);
    auto *ip = (uint8_t *)&(s->sin_addr);
    std::stringstream ss;
#define LZ(X) std::setw(3) << std::setfill('0') << uint16_t(X)

    ss << "udp://" << LZ(ip[0]) << "." << LZ(ip[1]) << "." << LZ(ip[2]) << "." << LZ(ip[3]) << ":"
       << std::to_string(htons(s->sin_port));

    return std::string(ss.str());
}

//-------------------------------------------------------------------------
const std::vector<char> &KsdConnected::getTask() const { return task_buffer_; }

//-------------------------------------------------------------------------
const std::vector<TNMLIB_RECORD_MODULE_INFO> &KsdConnected::getRecordModulesMap() const { return record_modules_map_; }

/*
 *
 *
 *
 */
void KsdConnected::closeSocket() {
    int status = ::closesocket(socket_obj_->socket_);
    if (status) setError("Error: closesocket()");
    socket_obj_->socket_ = INVALID_SOCKET;
}

//-------------------------------------------------------------------------
template <typename T>
std::string format(const char *c, T arg) {
    char str[100];
    sprintf_s(str, c, arg);
    return str;
}

//-------------------------------------------------------------------------
void setError(std::string &error, const std::string &error_msg) { error += error_msg + "\n"; }

//-------------------------------------------------------------------------
std::list<KsdConnected *> devicePing(const EthernetAddress &adapter_address, std::string &error_msg) {
    int32_t status = 0;

    auto ping_socket = INVALID_SOCKET;

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

    sockaddr_in sa_ping;
    memset(&sa_ping, 0, sizeof(sa_ping));

    sa_ping.sin_family = AF_INET;
    sa_ping.sin_addr.s_addr = INADDR_BROADCAST;
    sa_ping.sin_port = htons(DEFAULT_PORT);

    sockaddr_in host_addr;
    host_addr.sin_family = AF_INET;
    host_addr.sin_addr.s_addr = adapter_address.ip;
    host_addr.sin_port = 0;

    bind(ping_socket, (SOCKADDR *)&host_addr, sizeof(host_addr));

    do {
        uint16_t sequence_cnt = 0;
        uint16_t sequence_id = 0;

        CBW ping_cbw{};
        memset(&ping_cbw, 0, sizeof(ping_cbw));

        prepareProto(&ping_cbw.proto, COMMAND_BLOCK_WRAPPER_SIGNATURE, LUN_ANY, sequence_cnt, sequence_id);

        ping_cbw.data_transfer_length = 0x00000000;
        ping_cbw.request_code = Request::PING;
        ping_cbw.flags = 0x00;

        memcpy(buffer, &ping_cbw, sizeof(ping_cbw));

        sockaddr sa_from{};
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

            auto arg = SocketObj{candidate_socket_, candidate_sockaddr_};
            auto *dvc = new KsdConnected((int)devices.size(), candidate_lun_, &arg);

            devices.push_back(dvc);
            dvc->ask();
        } while (devices.size() < MAX_DEVICE_NUMBER);

        if (devices.empty()) {
            setError(error_msg, "Устройств не обнаружено.");
        }
    } while (false);

    status = closesocket(ping_socket);
    if (status) setError(error_msg, format("Error: closesocket() = 0x%04X", status));

    return devices;
}
