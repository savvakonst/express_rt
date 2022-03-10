//
// Created by SVK on 09.03.2022.
//

#ifndef EXRT_CUSTOMQACTIONS_H
#define EXRT_CUSTOMQACTIONS_H

#include <functional>
#include <utility>
#include <QAction>

class FuncProxyQAction : public QAction {
   public:

    using function_t = std::function<bool()>;
    FuncProxyQAction(function_t function, QObject *parent = nullptr)
        : QAction(parent), function_(std::move(function)) {
        connect(this, &FuncProxyQAction::triggered, this, &FuncProxyQAction::run);
    }
    FuncProxyQAction(void * object_ = nullptr, QObject *parent = nullptr)
        : QAction(parent) {
        connect(this, &FuncProxyQAction::triggered, this, &FuncProxyQAction::run);
    }

    void setFunction(function_t function){
        function_ = std::move(function);
    }

    void *object_ = nullptr;
   public slots:
    void run() { function_(); };

   private:

    function_t function_;
};

class ProxyQAction : public QAction {
   public:
    ProxyQAction(ExrtAction_ifs *action, QObject *parent = nullptr) : QAction(parent), action_(action) {
        setText(action->getDescription().c_str());
        connect(this, &ProxyQAction::triggered, this, &ProxyQAction::run);
    }
   public slots:
    void run() { action_->run(); };
    ExrtAction_ifs *action_;
};


#endif  // EXRT_CUSTOMQACTIONS_H
