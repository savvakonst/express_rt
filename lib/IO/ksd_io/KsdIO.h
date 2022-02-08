//
// Created by SVK on 07.02.2022.
//

#ifndef EXRT_KSDIO_H
#define EXRT_KSDIO_H

#include "common/IO_ifs.h"

class KsdIO : public IO_ifs {
   public:
    KsdIO();

    ~KsdIO() override;

    bool readDocument(ExtensionManager *manager, const std::string &source_path) override;
    bool saveDocument(const std::string &id, const std::string &dst_path) override;
};

#endif  // EXRT_KSDIO_H
