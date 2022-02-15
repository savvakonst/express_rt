#include <winsock2.h>
//
#include <iphlpapi.h>
#include <ws2tcpip.h>
//
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
//
#include "Adapters.h"
#include "common/CustomTypes.h"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

// this code was obtained from https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersinfo
#include "iostream"
class WinAdapters : public Adapters {
   public:
    WinAdapters() {
        auto list = getAdapterInfoList();
        current_adapter_ = list.front();
    }

    AdapterInfo getCurrentAdapter() override {
        { return current_adapter_; }
    }

    void setCurrentAdapter(const AdapterInfo &adapter) override { current_adapter_ = adapter; }
    std::vector<AdapterInfo> getAdapterInfoList() override;

   private:
    AdapterInfo current_adapter_;
};

bool initSocket() {
    WSADATA wsa;
    int32_t status = ::WSAStartup(MAKEWORD(2, 2), &wsa);
    if (status) return false;
    return true;
}

Adapters *newAdapters() {
    static bool g_already_inited = false;
    if (g_already_inited == false) {
        if (initSocket() == false) return nullptr;
        g_already_inited = false;
    }
    return new WinAdapters();
}

std::vector<AdapterInfo> WinAdapters::getAdapterInfoList() {
    std::vector<AdapterInfo> adapters;

    PIP_ADAPTER_INFO p_adapter_info;
    PIP_ADAPTER_INFO p_adapter = NULL;

    uint32_t dw_ret_val = 0;
    UINT i;

    /* variables used to print DHCP time info */

    ULONG ul_out_buf_len = sizeof(IP_ADAPTER_INFO);
    p_adapter_info = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (p_adapter_info == nullptr) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return {};
    }
    // Make an initial call to GetAdaptersInfo to get
    // the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(p_adapter_info, &ul_out_buf_len) == ERROR_BUFFER_OVERFLOW) {
        FREE(p_adapter_info);
        p_adapter_info = (IP_ADAPTER_INFO *)MALLOC(ul_out_buf_len);
        if (p_adapter_info == nullptr) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return {};
        }
    }

    if ((dw_ret_val = GetAdaptersInfo(p_adapter_info, &ul_out_buf_len)) == NO_ERROR) {
        p_adapter = p_adapter_info;
        while (p_adapter) {
            struct in_addr ip_address_ {};
            AdapterInfo adapter_info;

            adapter_info.description_ = p_adapter->Description;

            inet_pton(AF_INET, p_adapter->IpAddressList.IpAddress.String, &ip_address_);

            adapter_info.address_.ip = ip_address_.S_un.S_addr;
            adapter_info.address_.port = 0;
            if (p_adapter->AddressLength == 6) memcpy(adapter_info.address_.mac, p_adapter->Address, 6);

            adapters.push_back(adapter_info);
            // printf("\tAdapter Desc: \t%s\n", p_adapter->Description);
            /*
            printf("\tAdapter Addr: \t");
            for (i = 0; i < p_adapter->AddressLength; i++) {
                if (i == (p_adapter->AddressLength - 1)) printf("%.2X\n", (int)p_adapter->Address[i]);
                else
                    printf("%.2X-", (int)p_adapter->Address[i]);
            }
             */

            // printf("\tIP Address: \t%s\n", p_adapter->IpAddressList.IpAddress.String);

            p_adapter = p_adapter->Next;
            // printf("\n");
        }
    } else {
        printf("GetAdaptersInfo failed with error: %d\n", dw_ret_val);
    }
    if (p_adapter_info) FREE(p_adapter_info);

    return adapters;
}
