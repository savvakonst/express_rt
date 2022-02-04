//
// Created by SVK on 04.02.2022.
//

#ifndef EXRT_IO_IFS_H
#define EXRT_IO_IFS_H

#include "common/BaseClass_ifs.h"

class ExtensionManager;

class IO_ifs : public BaseClass_ifs {
   public:
    IO_ifs(const std::string &filename_pattern, const std::string &file_type, const std::string &file_info)
        : filename_pattern_(filename_pattern), file_type_(file_type), file_info_() {}

    ~IO_ifs() override = default;

    virtual bool readDocument(ExtensionManager *manager, const std::string &source_path) = 0;
    virtual bool saveDocument(const std::string &id, const std::string &dst_path) = 0;

    [[maybe_unused]] const std::string filename_pattern_ = 0;
    [[maybe_unused]] const std::string file_type_ = 0;
    [[maybe_unused]] const std::string file_info_ = 0;
};

#endif  // EXRT_IO_IFS_H
