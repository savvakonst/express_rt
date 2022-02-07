//
// Created by SVK on 03.02.2022.
//

#ifndef EXRT_WIDGETWRAPPERS_H
#define EXRT_WIDGETWRAPPERS_H

#include "common/BaseClass_ifs.h"
#include "common/DataSchema_ifs.h"
#include "common/Extension.h"

class QWidget;

class WidgetWrapper_ifs {
   public:
    virtual ~WidgetWrapper_ifs() = default;

    virtual QWidget *getWidget() = 0;

    virtual status addSignal(Signal_ifs *signal) = 0;
};


template <class T>
class WidgetWrapper : public WidgetWrapper_ifs {
   public:
    explicit WidgetWrapper(T *widget) : widget_(widget) {}

    WidgetWrapper(const WidgetWrapper &) = delete;

    WidgetWrapper(const WidgetWrapper &&) = delete;

    ~WidgetWrapper() override = default;

    QWidget *getWidget() override { return widget_; }

    status addSignal(Signal_ifs *signal) override { return widget_->signal_controller_.addSignal(signal); }

   private:
    T *widget_;
};

template <class T,class ret_T>
class ListWidgetWrapper : public WidgetWrapper_ifs {
   public:
    explicit ListWidgetWrapper(T *widget) : widget_(widget) {}

    ListWidgetWrapper(const ListWidgetWrapper &) = delete;

    ListWidgetWrapper(const ListWidgetWrapper &&) = delete;

    ~ListWidgetWrapper() override = default;

    QWidget *getWidget() override { return widget_; }

    status addSignal(Signal_ifs *signal) override { return widget_->signal_controller_.addSignal(signal); }

    virtual bool setActive(size_t row_index) = 0 ;
    virtual bool removeFromActive()  = 0 ;

    virtual bool addToSelected(size_t row_index) = 0 ;
    virtual bool removeFromSelected(size_t row_index) = 0 ;

    virtual ret_T* getActive() = 0;
    virtual ret_T* getSelected() = 0;

   private:
    T *widget_;
};


#endif  // EXRT_WIDGETWRAPPERS_H
