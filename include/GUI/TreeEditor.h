#ifndef TREE_WIDGET_H
#define TREE_WIDGET_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <iostream>

#include "WidgetWrappers.h"
#include "common/BaseClass_ifs.h"
#include "common/DataSchema_ifs.h"
#include "common/Extension.h"
class ExtensionManager;
class QTreeWidgetItem;

/*
 *
 *
 */

class TreeEditor : public QTreeWidget {
   public:
    explicit TreeEditor(ExtensionManager *manager, QWidget *parent = nullptr);

    virtual void setupProperties(DataSchema_ifs *ds, QTreeWidgetItem *parent_item = nullptr);

   private:
    void addProperty(DataSchema_ifs *ds, QTreeWidgetItem *parent_item = nullptr);

    class UpdateSignal : public Signal_ifs {
       public:
        explicit UpdateSignal(TreeEditor *tree_widget) : tree_widget_(tree_widget) {}

        void emit_() override { tree_widget_->model()->layoutChanged(); }

        TreeEditor *tree_widget_;
    };

   protected:
    void addExtensionUint(ExtensionManager *);

   private:
    typedef WidgetWrapper_ifs *(*treeWidgetWrapperConstructor)(DataSchema_ifs *, QWidget *);

    typedef std::vector<treeWidgetWrapperConstructor> constructorList_t;
    std::map<std::string, constructorList_t *> constructors_map_;

    UpdateSignal update_signal_;

    std::string error_message_;

    DataSchema_ifs *data_schema_ = nullptr;
};

template <class T>
WidgetWrapper_ifs *newTreeWidgetWrapper(DataSchema_ifs *data_schema, QWidget *parent = nullptr) {
    return new TreeWidgetWrapper(new T(data_schema, parent));
}

// TreeEditor(parent)
typedef TreeEditor *(*newTreeEditor_t)(ExtensionManager *, QWidget *);

#endif  // MAI