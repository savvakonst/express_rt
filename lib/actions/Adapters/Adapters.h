//
// Created by SVK on 14.02.2022.
//

#ifndef EXRT_ADAPTERS_H
#define EXRT_ADAPTERS_H
#include "common/CustomTypes.h"

class AdapterInfo {
   public:
    std::string description_;
    EthernetAddress address_;
};

class Adapters;

Adapters* newAdapters();

class Adapters {
   public:
    virtual AdapterInfo getCurrentAdapter() = 0;
    virtual void setCurrentAdapter(const AdapterInfo& adapter) = 0;
    virtual std::vector<AdapterInfo> getAdapterInfoList() = 0;
};

#endif  // EXRT_ADAPTERS_H
