#ifndef BASECLASS_H_
#define BASECLASS_H_

#include <list>
#include <set>
#include <vector>

#include "CustomTypes.h"
#include "ErrorInfo_ifs.h"
#include "Signal_ifs.h"
#include "common\Port.h"

class COMMON_API_ BaseSignalController {
   private:
    std::list<Signal_ifs *> signals_;

   public:
    void emit_() {
        for (auto i : signals_) {
            i->emit_();
        }
    }

    status addSignal(Signal_ifs *signal) {
        signals_.push_back(signal);
        return status::succes;
    }

    status removeSignal(Signal_ifs *signal) {
        signals_.remove(signal);
        return succes;
    }

    status removeAllSignals() {
        signals_.clear();
        return succes;
    }
};

class COMMON_API_ BaseClass_ifs {
   private:
    /* data */

    std::list<Signal_ifs *> signals_;

   protected:
    BaseClass_ifs(/* args */) {}

    virtual ~BaseClass_ifs() = default;

    /*
     *  common voids
     *  TODO: perhaps it will be good to create parent class with following members
     */

    void emit_() {
        for (auto i : signals_) {
            i->emit_();
        }
    }

   public:
    status addSignal(Signal_ifs *signal) {
        signals_.push_back(signal);
        return status::succes;
    }

    virtual status removeSignal(Signal_ifs *signal) {
        signals_.remove(signal);
        return succes;
    }

    virtual status removeAllSignals() {
        signals_.clear();
        return succes;
    }

    // TODO it might be worth to remove this function
    virtual const ErrorInfo_ifs *getErrorInfo() const { return nullptr; };

    virtual bool hasError() const { return !error_message_.empty(); }

    virtual const std::string &getErrorMessage() const { return error_message_; }

    virtual void clearErrorMessage() { error_message_.clear(); }

    // a think, in future it will be transformed in non virtual functions
    virtual const std::list<std::string> &getWarningMessages() const { return warning_messages_; }

    virtual void clearWarningMessages() { warning_messages_.clear(); }

   protected:
    std::string error_message_;
    std::list<std::string> warning_messages_;
};

#endif