#ifndef EXRT_EXRTACTION_H
#define EXRT_EXRTACTION_H

#include "common/BaseClass_ifs.h"

class ExrtAction : public BaseClass_ifs {
   public:
    virtual bool run() = 0;

    virtual std::string getDescription() = 0;
    virtual std::string getInfo() = 0;
};

#endif  // EXRT_ACTION_H
