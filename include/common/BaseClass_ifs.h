#ifndef BASECLASS_H_
#define BASECLASS_H_

#include <list>
#include <set>
#include <vector>

#include "CustomTypes.h"
#include "ErrorInfo_ifs.h"
#include "Signal_ifs.h"
#include "common\Port.h"

class COMMON_API_ BaseClass_ifs {
   private:
    /* data */

    std::list<Signal_ifs*> signals_;

   protected:
    BaseClass_ifs(/* args */) {}
    ~BaseClass_ifs() {}

    /*
     *  common voids
     *  perhaps it will be good to create parent class with following members
     */

    void emit() {
        for (auto i : signals_) {
            i->emit();
        }
    }

    status addSignal(Signal_ifs* signal) {
        signals_.push_back(signal);
        return status::succes;
    }

    virtual status removeSignal(Signal_ifs* signal) {
        signals_.remove(signal);
        return succes;
    }

    virtual status removeAllSignals() {
        signals_.clear();
        return succes;
    }

   public:
    virtual const ErrorInfo_ifs* getErrorInfo() const = 0;

    virtual bool hasError() const { return !error_mesadge_.empty(); }
    virtual const std::string& getErrorMessage() const { return error_mesadge_; }
    virtual void clearErrorMessage() { error_mesadge_.clear(); }

    // a think,in future it will be transformed in non virtual functions
    virtual const std::list<std::string>& getWarningMessages() const { return warning_mesadges_; }
    virtual void clearWarningMessages() { warning_mesadges_.clear(); }

   protected:
    std::string error_mesadge_;
    std::list<std::string> warning_mesadges_;
};

#endif