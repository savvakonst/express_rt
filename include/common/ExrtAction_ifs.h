#ifndef EXRT_EXRTACTION_IFS_H
#define EXRT_EXRTACTION_IFS_H

#include "common/BaseClass_ifs.h"

class ExrtAction_ifs : public BaseClass_ifs {
   public:

    ~ExrtAction_ifs() = default;

    virtual bool run() = 0;

    virtual std::string getDescription() = 0;
    virtual std::string getInfo() = 0;
};

#endif  // EXRT_ACTION_H
