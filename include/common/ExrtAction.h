#ifndef EXRT_EXRTACTION_H
#define EXRT_EXRTACTION_H

class ExrtAction {
   public:
    virtual std::string run() = 0;

    virtual std::string getDescription() = 0;
    virtual std::string getInfo() = 0;
};

#endif  // EXRT_ACTION_H
