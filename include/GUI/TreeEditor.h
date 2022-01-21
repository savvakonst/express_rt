#ifndef TREE_WIDGET_H
#define TREE_WIDGET_H


#include <QPainter>
#include <QPalette>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <iostream>

#include "common/BaseClass_ifs.h"
#include "common/Extension.h"
#include "common/FieldInfo.h"



class TreeWidgetWrapper_ifs {
   public:
    virtual ~TreeWidgetWrapper_ifs() = default;

    virtual QWidget *getWidget() = 0;

    virtual status addSignal(Signal_ifs *signal) = 0;
};

template <class T>
class TreeWidgetWrapper : public TreeWidgetWrapper_ifs {
   public:
    explicit TreeWidgetWrapper(T *widget) : widget_(widget) {}

    TreeWidgetWrapper(const TreeWidgetWrapper &) = delete;

    TreeWidgetWrapper(const TreeWidgetWrapper &&) = delete;

    ~TreeWidgetWrapper() override = default;

    QWidget *getWidget() override { return widget_; }

    status addSignal(Signal_ifs *signal) override { return widget_->signal_controller_.addSignal(signal); }

    virtual void setDataSchema(DataSchema_ifs *ds) { widget_->setDataSchema(ds); }

   private:
    T *widget_;
};



/*
 *
 *
 */



class TreeEditor : public QTreeWidget {
   public:
    explicit TreeEditor(QWidget *parent = nullptr);

    explicit TreeEditor(DataSchema_ifs *data_schema, QWidget *parent = nullptr);

    void setupProperties(DataSchema_ifs *ds, QTreeWidgetItem *parent_item = nullptr);

    void setupProperties();

   private:
    class UpdateSignal : public Signal_ifs {
       public:
        explicit UpdateSignal(TreeEditor *tree_widget) : tree_widget_(tree_widget) {}

        void emit_() override { tree_widget_->model()->layoutChanged(); }

        TreeEditor *tree_widget_;
    };

   protected:
   public:
    void addExtensionUint(ExtensionUnit *uint);

   private:
    typedef TreeWidgetWrapper_ifs *(*treeWidgetWrapperConstructor)(DataSchema_ifs *, QWidget *);

    typedef std::vector<treeWidgetWrapperConstructor> constructorList_t;
    std::map<std::string, constructorList_t *> constructors_map_;

    UpdateSignal update_signal_;

    std::string error_message_;

    DataSchema_ifs *data_schema_ = nullptr;
};

template <class T>
TreeWidgetWrapper_ifs *newTreeWidgetWrapper(DataSchema_ifs *data_schema, QWidget *parent = nullptr) {
    return new TreeWidgetWrapper(new T(data_schema, parent));
}

// TreeEditor(parent)
typedef TreeEditor * (*newTreeEditor_t)(QWidget* );

#endif  // MAI