//
// Created by SVK on 09.03.2022.
//

#ifndef EXRT_CUSTOMQACTIONS_H
#define EXRT_CUSTOMQACTIONS_H

#include <QAction>
#include <functional>
#include <utility>

#include "common/ExrtAction_ifs.h"

class FuncProxyQAction : public QAction {
   public:
    using function_t = std::function<bool()>;
    explicit FuncProxyQAction(function_t function, QObject *parent = nullptr)
        : QAction(parent), function_(std::move(function)) {
        connect(this, &FuncProxyQAction::triggered, this, &FuncProxyQAction::run);
    }

    FuncProxyQAction(const QIcon &icon, const QString &text, function_t function, QObject *parent = nullptr)
        : QAction(icon, text, parent), function_(std::move(function)) {
        connect(this, &FuncProxyQAction::triggered, this, &FuncProxyQAction::run);
    }

    explicit FuncProxyQAction(void *object = nullptr, QObject *parent = nullptr) : QAction(parent) {
        connect(this, &FuncProxyQAction::triggered, this, &FuncProxyQAction::run);
    }

    void setFunction(function_t function) { function_ = std::move(function); }

    void *object_ = nullptr;
   public slots:
    void run() { function_(); };

   private:
    function_t function_;
};

class ProxyQAction : public QAction {
   public:
    explicit ProxyQAction(ExrtAction_ifs *action, QObject *parent = nullptr) : QAction(parent), action_(action) {
        setText(action->getDescription().c_str());
        connect(this, &ProxyQAction::triggered, this, &ProxyQAction::run);
    }
   public slots:
    void run() const { action_->run(); };
    ExrtAction_ifs *action_;
};

#endif  // EXRT_CUSTOMQACTIONS_H
