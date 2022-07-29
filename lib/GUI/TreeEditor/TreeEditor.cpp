
#define _USE_MATH_DEFINES

#include "GUI/TreeEditor.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>
#include <QTreeWidget>
#include <iostream>
#include <utility>

#include "TreeTextEdit.h"
#include "common/ExtensionManager.h"

TreeEditor::TreeEditor(QWidget *parent) : QTreeWidget(parent), update_signal_(this) {
    item_stylesheet_ =
        "QWidget {"
        "   background-color: rgba(0,0,0,0);"
        "   selection-background-color: black;"
        "   selection-color: yellow; "
        "   border-style: outset; "
        "   border-width: 0px;"
        "}"
        "QWidget    :pressed {"
        "    background-color: rgb(224, 0, 0);"
        "    border-style: inset;"
        "}"
        "QWidget::focus {"
        "   color: white; "
        "}";

    this->setStyleSheet(
        "QHeaderView::section {"
        "   background-color: #D2DCDF;"
        "   alternate-background-color: #f6fafb;"
        //"   selection-color: yellow; "
        "}"
        "QTreeView::item:selected {"
        "   selection-color: white; "
        "}"

    );

    this->setAutoFillBackground(true);
    this->setBackgroundRole(QPalette().Base);
    auto p = this->palette();
    p.setColor(backgroundRole(), QColor("#D2DCDF"));
    this->setPalette(p);

    this->setHeaderItem(new QTreeWidgetItem({"", "свойства"}, 4));

    this->setAlternatingRowColors(true);
}

void TreeEditor::setupProperties(Parameter_ifs *parameter) {
    clear();
    const DataSchema_ifs *ds = parameter->getPropertySchema();
    parameter->getProperty("");
    parameter_ = parameter;

    if (ds->isMap()) {
        auto map_list = ds->getMapList();
        for (auto i : map_list) {
            addProperty(i, 0, nullptr, i->name_);
        }
    }
    expandAll();
}

class AddNewItemToListButton : public QPushButton {
   public:
    AddNewItemToListButton(TreeEditor *tree_editor, DataSchema_ifs *ds, size_t dim, QTreeWidgetItem *parent_item,
                           std::string path)
        : QPushButton("+"),
          tree_editor_(tree_editor),
          ds_(ds),
          dim_(dim),
          parent_item_(parent_item),
          path_(std::move(path)) {
        auto child_cnt = parent_item_->childCount();
    }
    void mousePressEvent(QMouseEvent *event) override {
        QPushButton::mousePressEvent(event);
        auto child_cnt = parent_item_->childCount();
        tree_editor_->addProperty(ds_, dim_ + 1, parent_item_, path_ + "/" + std::to_string(child_cnt));
    }

    TreeEditor *tree_editor_;
    const std::string path_;
    size_t dim_;
    DataSchema_ifs *ds_;
    QTreeWidgetItem *parent_item_;
};

void TreeEditor::addProperty(DataSchema_ifs *ds, size_t dim, QTreeWidgetItem *parent_item,
                             const std::string &path) {  // NOLINT(misc-no-recursion)
    auto item = parent_item ? new QTreeWidgetItem(parent_item) : new QTreeWidgetItem(this);

    auto name = ds->description_.c_str();
    item->setText(0, dim ? "" : name);

    if (ds->isArray()) {
        auto dims = ds->getDims();
        if (dim < dims.size()) {
            auto list = parameter_->getProperty(path);
            int index = 0;

            if (dims[0] == 0) {
                // TODO: to implement this branch of logic
                //auto add_new_button_item = new QTreeWidgetItem(item);
                auto w = new AddNewItemToListButton(this, ds, dim, item, path);
                w->setStyleSheet(item_stylesheet_);
                this->setItemWidget(item, 1, w);
            }

            if (list) {
                for (auto i : list->getArray()) addProperty(ds, dim + 1, item, path + "/" + std::to_string(index++));
            }


            return;
        }
    }

    if (ds->isMap()) {
        auto map_list = ds->getMapList();
        for (auto i : map_list) {
            addProperty(i, 0, item, path + "/" + i->name_);
        }
    } else {
        auto type = ds->getRepresentationType();
        auto cm = constructors_map_.find(type);

        if (cm != constructors_map_.end()) {
            WidgetWrapper_ifs *wrapper;
            auto list = *(*cm).second;

            // search for appropriate constructor
            for (treeWidgetWrapperConstructor wrapper_constructor : list) {
                wrapper = wrapper_constructor(parameter_, ds, path, nullptr);
                if (wrapper) break;
            }

            if (wrapper) {
                // if (!wrapper->getWidget()->metaObject()->inherits(&QComboBox::staticMetaObject))
                wrapper->getWidget()->setStyleSheet(item_stylesheet_);

                this->setItemWidget(item, 1, wrapper->getWidget());
                wrapper->addSignal(&update_signal_);

                delete wrapper;
            } else {
                auto error = "no compatible constructor for type: \"" + type + "\"\n";
                error_message_ += error;
                item->setText(1, QString::fromStdString(error));
            }
        } else {
            auto error = "unknown type: \"" + type + "\"\n";
            error_message_ += error;
            item->setText(1, QString::fromStdString(error));
        }
    }
}

void TreeEditor::addExtensionUint(ExtensionManager *manager) {
    auto unit_list = manager->getLastVersionExtensionUnitsByType("tree_widget_wrapper");

    for (auto uint : unit_list) {
        QString str = QString::fromStdString(uint->name);
        QStringList str_list = str.split('|');
        for (auto &i : str_list) {
            auto type = std::string(i.toStdString());

            auto constructor_ptr = (treeWidgetWrapperConstructor)uint->ptr;

            auto cm = constructors_map_.find(type);
            if (cm == constructors_map_.end()) {
                constructors_map_[type] = new constructorList_t{constructor_ptr};
            } else {
                (*cm).second->push_back(constructor_ptr);
            }
        }
    }
}
