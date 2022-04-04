#ifndef EXRT_RECEIVER_H
#define EXRT_RECEIVER_H

//#include <winsock2.h>

#include <atomic>
#include <string>

#include "common/BaseClass_ifs.h"
#include "device/ModuleStream_ifs.h"

std::string getWSALastErrorText();

namespace std {
class thread;
}

struct ReceiverHidden;

class Receiver : public BaseClass_ifs {
   public:
    struct Sockaddr {
        Sockaddr(uint16_t sin_family, uint16_t sin_port, uint32_t sin_addr, uint64_t sin_zero)
            : sin_family(sin_family), sin_port(sin_port), sin_addr(sin_addr), sin_zero(sin_zero) {}

        explicit Sockaddr(EthernetAddress address);

        uint16_t sin_family;
        uint16_t sin_port;
        uint32_t sin_addr;
        uint64_t sin_zero;
    };

    explicit Receiver(ModuleStream_ifs *m_stream_,
                      Sockaddr dst_address /*= {AF_INET, htons(4660), inet_addr("192.168.0.176"), 0}*/,
                      const Sockaddr src_address = Sockaddr{0, 0, 0, 0});

    explicit Receiver(ModuleStream_ifs *m_stream, const EthernetSettings &ethernet_settings);
    ~Receiver();

    void start();

    void stop();

   protected:
    ModuleStream_ifs *m_stream_ = nullptr;

    ReceiverHidden *hidden_;

    bool in_run_ = false;

    const Sockaddr src_address_;
    const Sockaddr dst_address_;

    uint32_t socket_buffer_size_ = (32 << 20);

    char *data_buffer_ = nullptr;
    uint32_t data_buffer_size_ = (2 << 16);

    std::thread *thread_ = nullptr;
    std::atomic_int32_t cmd_;
};

#endif
