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
class ConversionTemplate;
class Parameter_ifs;

class QTreeWidgetItem;

/*
 *
 *
 */

class TreeEditor : public QTreeWidget {
   public:
    explicit TreeEditor(QWidget *parent = nullptr);

    virtual void setupProperties(Parameter_ifs *parameter);

    void addExtensionUint(ExtensionManager *manager);

   private:
    void addProperty(DataSchema_ifs *ds, size_t dim, QTreeWidgetItem *parent_item = nullptr,
                     const std::string &path = "");

    class UpdateSignal : public Signal_ifs {
       public:
        explicit UpdateSignal(TreeEditor *tree_widget) : tree_widget_(tree_widget) {}

        void emit_() override { tree_widget_->model()->layoutChanged(); }

        TreeEditor *tree_widget_;
    };

   protected:
   private:
    typedef WidgetWrapper_ifs *(*treeWidgetWrapperConstructor)(Parameter_ifs *, DataSchema_ifs *, const std::string &,
                                                               QWidget *);

    typedef std::vector<treeWidgetWrapperConstructor> constructorList_t;
    std::map<std::string, constructorList_t *> constructors_map_;

    UpdateSignal update_signal_;

    std::string error_message_;
    Parameter_ifs *parameter_ = nullptr;

    QAbstractItemView *parent_view_ = nullptr;
    ConversionTemplate *conversion_template__ = nullptr;

    QString item_stylesheet_;

    friend class AddNewItemToListButton;
    // const DataSchema_ifs *data_schema_ = nullptr;
};

template <class T>
WidgetWrapper_ifs *newTreeWidgetWrapper(Parameter_ifs *parameter, DataSchema_ifs *data_schema, const std::string &path,
                                        QWidget *parent = nullptr) {
    // std::cerr << path << "\n";
    return new WidgetWrapper(new T(parameter, data_schema, path, parent));
}

// TreeEditor(parent)
typedef TreeEditor *(*newTreeEditor_t)(ExtensionManager *, QWidget *);

#endif  // MAI