//
// Created by SVK on 17.11.2021.
//
#include <winsock2.h>
//
#include "Receiver.h"
//
#include <atomic>
#include <iostream>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

std::string getWSALastErrorText() {
    auto error_code = WSAGetLastError();
    switch (error_code) {
    case WSANOTINITIALISED:
        return "A successful WSAStartup call must occur before using this function.";
    case WSAENETDOWN:
        return "The network subsystem or the associated service provider has failed.";
    case WSAEAFNOSUPPORT:
        return "The specified address family is not supported. For example, an application tried to create a socket "
               "for the AF_IRDA address family but an infrared adapter and device driver is not installed on the local "
               "computer.";
    case WSAEINPROGRESS:
        return "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a "
               "callback "
               "function.";
    case WSAEMFILE:
        return "No more socket descriptors are available.";
    case WSAEINVAL:
        return "An invalid argument was supplied. This error is returned if the af parameter is set to AF_UNSPEC and "
               "the "
               "type and protocol parameter are unspecified.";
    case WSAEINVALIDPROVIDER:
        return "The service provider returned a version other than 2.2.";
    case WSAEINVALIDPROCTABLE:
        return "The service provider returned an invalid or incomplete procedure table to the WSPStartup.";
    case WSAENOBUFS:
        return "No buffer space is available. The socket cannot be created.";
    case WSAEPROTONOSUPPORT:
        return "The specified protocol is not supported.";
    case WSAEPROTOTYPE:
        return "The specified protocol is the wrong type for this socket.";
    case WSAEPROVIDERFAILEDINIT:
        return "The service provider failed to initialize. This error is returned if a layered service provider (LSP) "
               "or "
               "namespace provider was improperly installed or the provider fails to operate correctly.";
    case WSAESOCKTNOSUPPORT:
        return "The specified socket type is not supported in this address family.";
    }
    return "unknown. error_code: " + std::to_string(error_code);
}
// 2952898752

struct ReceiverHidden {
    SOCKET sock_;
};

Receiver::Receiver(ModuleStream_ifs *m_stream, Sockaddr dst_address, Sockaddr src_address)
    : hidden_(new ReceiverHidden{INVALID_SOCKET}),
      dst_address_(dst_address),
      src_address_(src_address),
      m_stream_(m_stream) {
    WSADATA wsa;
    auto status = ::WSAStartup(MAKEWORD(2, 2), &wsa);
    if (status != 0) {
        this->error_message_ = "usage initialization of the Winsock DLL by a process has failed.";
        return;
    }

    hidden_->sock_ = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (hidden_->sock_ == INVALID_SOCKET) {
        this->error_message_ = "socket: " + getWSALastErrorText();
        return;
    }

    status =
        ::setsockopt(hidden_->sock_, SOL_SOCKET, SO_RCVBUF, (char *)&socket_buffer_size_, sizeof(socket_buffer_size_));
    if (status != 0) {
        this->error_message_ = "SO_RCVBUF: " + getWSALastErrorText();
        return;
    }

    bool val = true;
    status = ::setsockopt(hidden_->sock_, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val));
    if (status != 0) {
        this->error_message_ = "SO_REUSEADDR: " + getWSALastErrorText();
        return;
    }

    uint32_t timeout = 500;
    status = ::setsockopt(hidden_->sock_, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    if (status != 0) {
        this->error_message_ = "SO_RCVTIMEO: " + getWSALastErrorText();
        return;
    }

    status = ::bind(hidden_->sock_, (const sockaddr *)&dst_address_, sizeof(dst_address_));
    if (status != 0) {
        this->error_message_ = getWSALastErrorText();
        return;
    }

    data_buffer_ = new char[data_buffer_size_];
}

Receiver::Receiver(ModuleStream_ifs *m_stream, const EthernetSettings &ethernet_settings)
    : Receiver(m_stream, {AF_INET, ethernet_settings.dst_.port, ethernet_settings.dst_.ip, 0}, Sockaddr{0, 0, 0, 0}) {}
//{0, htons(*((uint16_t *)ethernet_settings.src_.port)), *((uint32_t *)ethernet_settings.src_.ip_b), 0}) {}

Receiver::~Receiver() {
    if (thread_) {
        stop();
        delete thread_;
    }
}

struct Frame {
    uint16_t marker;
    uint32_t time;

    size_t getCntListLen() const {
        // 0x5A3x
        return size_t(0x000F & marker);
    }

    uint16_t *getCntList() { return (uint16_t *)((char *)this + sizeof(Frame)); }

    char *getDataPtr() {
        const size_t c_sum_size = 2;
        return (char *)((char *)this + sizeof(Frame) + (marker & 0x000F) + c_sum_size);
    }
};

void unpack() {
    void *ptr = nullptr;
    auto *frame = (Frame *)ptr;
    ModuleStream_ifs **m_stream_ifs = nullptr;

    size_t len = frame->getCntListLen();
    uint16_t *cnt = frame->getCntList();

    char *data_frame_list = frame->getDataPtr();

    for (size_t n = 0; n < len; n++) {
        size_t size = cnt[n];
        m_stream_ifs[n]->readFramePeace(nullptr, data_frame_list, size);
        data_frame_list += size;
    }
}

void receiverThread(ModuleStream_ifs *m_stream, SOCKET sock_, char *data_buffer_, int32_t data_buffer_size_,
                    Receiver::Sockaddr source_address_, std::atomic_int32_t &cmd_) {
    Receiver::Sockaddr source_address = {0, 0, 0, 0};
    int32_t source_address_length = sizeof(source_address);

    while (1) {
        if (cmd_) return;

        auto length =
            recvfrom(sock_, data_buffer_, data_buffer_size_, 0, (sockaddr *)&source_address, &source_address_length);

        if (length == SOCKET_ERROR) {
            auto error_code = WSAGetLastError();
            if (WSAETIMEDOUT == error_code) continue;

            std::cout << "length == SOCKET_ERROR: " + getWSALastErrorText() << "\n";
            return;
        }

        m_stream->readFramePeace(nullptr, data_buffer_, length);
    }
}

void Receiver::start() {
    if (in_run_) return;
    in_run_ = true;
    cmd_ = 0;
    thread_ = new std::thread(receiverThread, m_stream_, hidden_->sock_, data_buffer_, data_buffer_size_, src_address_,
                              std::ref(cmd_));
    // thread_->join();
}

void Receiver::stop() {
    if (!in_run_) return;
    in_run_ = false;
    cmd_ = 1;
    thread_->join();
}

Receiver::Sockaddr::Sockaddr(EthernetAddress address) {
    sin_family = AF_INET;

    sin_port = htons(address.port);
    sin_addr = address.ip;
    sin_zero = 0;
}
