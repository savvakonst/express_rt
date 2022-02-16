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

   protected:
    T *widget_;
};

template <class ret_T>
class ListWidgetWrapper_ifs : public WidgetWrapper_ifs {
   public:
    virtual bool setActive(size_t row_index) = 0;
    virtual bool removeFromActive() = 0;

    virtual bool addToSelected(size_t row_index) = 0;
    virtual bool removeFromSelected(size_t row_index) = 0;

    virtual ret_T *getActive() = 0;
    virtual std::vector<ret_T *> getSelected() = 0;
};

class Module_ifs;
class Device;

class DeviceViewWrapper_ifs : public WidgetWrapper_ifs {
   public:
    virtual bool setActive(size_t row_index) = 0;
    virtual bool setActive(const std::string &source, const std::string &path) = 0;
    virtual bool removeFromActive() = 0;

    virtual bool addToSelected(size_t row_index) = 0;
    virtual bool removeFromSelected(size_t row_index) = 0;

    virtual Device *getActiveDevice() = 0;
    virtual Module_ifs *getActiveModule() = 0;

    virtual std::vector<Module_ifs *> getSelected() = 0;
};

class Parameter_ifs;

class ParameterViewWrapper_if : public WidgetWrapper_ifs {
   public:
    virtual bool setActive(size_t row_index) = 0;
    virtual bool setActive(const std::string &name) = 0;
    virtual bool removeFromActive() = 0;

    virtual bool addToSelected(size_t row_index) = 0;
    virtual bool addToSelected(const std::string &name) = 0;
    virtual bool removeFromSelected(size_t row_index) = 0;

    virtual Parameter_ifs *getActive() = 0;
    virtual std::vector<Parameter_ifs *> getSelected() = 0;
};

class ConversionTemplate;

class ConversionTemplateViewWrapper_if : public WidgetWrapper_ifs {
   public:
    virtual bool setActive(size_t row_index) = 0;
    virtual bool setActive(const std::string &source) = 0;
    virtual bool removeFromActive() = 0;

    virtual bool addToSelected(size_t row_index) = 0;
    virtual bool addToSelected(const std::string &name) = 0;
    virtual bool removeFromSelected(size_t row_index) = 0;

    virtual ConversionTemplate *getActive() = 0;
    virtual std::vector<ConversionTemplate *> getSelected() = 0;
};

#endif  // EXRT_WIDGETWRAPPERS_H
