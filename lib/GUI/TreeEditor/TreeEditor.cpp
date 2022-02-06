
#define _USE_MATH_DEFINES

#include "GUI/TreeEditor.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QStringList>
#include <QTreeWidget>
#include <iostream>

#include "TreeTextEdit.h"
#include "common/ExtensionManager.h"

TreeEditor::TreeEditor(ExtensionManager *manager, QWidget *parent) : QTreeWidget(parent), update_signal_(this) {



    this->setAutoFillBackground(true);
    this->setBackgroundRole(QPalette().Base);
    auto p = this->palette();
    p.setColor(backgroundRole(), QColor("#D2DCDF"));
    this->setPalette(p);

    this->setHeaderItem(new QTreeWidgetItem({"", "свойства"}, 4));

    this->setStyleSheet(
        "QHeaderView::section {background-color: #D2DCDF; alternate-background-color: #f6fafb;};"
        //"alternate-background-color: #f6fafb;background-color: #D2DCDF; border-style: outset; border-width: 0px;"
    );

    this->setAlternatingRowColors(true);

    addExtensionUint(manager);
}

void TreeEditor::setupProperties(Parameter_ifs *parameter) {
    const DataSchema_ifs *ds = parameter->getPropertySchema();
    parameter_ = parameter;

    if (ds->isMap()) {
        auto map_list = ds->getMapList();
        for (auto i : map_list) {
            addProperty(i, nullptr, i->name_);
        }
    }
    expandAll();
}

void TreeEditor::addProperty(DataSchema_ifs *ds, QTreeWidgetItem *parent_item, const std::string &path) {
    auto item = parent_item ? new QTreeWidgetItem(parent_item) : new QTreeWidgetItem(this);

    auto name = ds->description_.c_str();
    item->setText(0, name);

    if (ds->isMap()) {
        auto map_list = ds->getMapList();
        for (auto i : map_list) {
            addProperty(i, item, path + "/" + i->name_);
        }
    } else if (ds->isArray()) {
        // TODO: realise this branch of logic
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
                wrapper->getWidget()->setStyleSheet(
                    "QWidget {background-color: rgba(0,0,0,0);border-style: outset; border-width: 0px;}"
                    "QWidget QAbstractItemView {\n"
                    "    background: white;"
                    "}");

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
    auto unit_list = manager->getLastVersionExtensionUintsByType("tree_widget_wrapper");

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

/*
 *
 *
 *
 *
 */
class TreeCheckBox : public QCheckBox {
   public:
    explicit TreeCheckBox(Parameter_ifs *parameter, DataSchema_ifs *data_schema, const std::string &path,
                          QWidget *parent = nullptr)
        :  //
          QCheckBox(parent),
          parameter_(parameter),
          data_schema_(data_schema),
          path_(path) {
        setToolTip(data_schema_->help_.c_str());
        initSettings();
    }

    BaseSignalController signal_controller_;

   private:
    void initSettings() {}

   protected:
    std::string path_;
    Parameter_ifs *parameter_ = nullptr;
    DataSchema_ifs *data_schema_ = nullptr;
};

/*
 *
 *
 */

class TreeLineEdit : public QLineEdit {
   public:
    explicit TreeLineEdit(Parameter_ifs *parameter, DataSchema_ifs *data_schema, const std::string &path,
                          QWidget *parent = nullptr)
        :  //
          QLineEdit(parent),
          parameter_(parameter),
          data_schema_(data_schema),
          path_(path) {
        setToolTip(data_schema_->help_.c_str());
        initSettings();
    }

    BaseSignalController signal_controller_;

   private:
    void initSettings() {}

   protected:
    std::string path_;
    Parameter_ifs *parameter_ = nullptr;
    DataSchema_ifs *data_schema_ = nullptr;
};

/*
 *
 *
 */

class TreeIPEdit : public TreeLineEdit {
   public:
    explicit TreeIPEdit(Parameter_ifs *parameter, DataSchema_ifs *data_schema, const std::string &path,
                        QWidget *parent = nullptr)
        :  //
          TreeLineEdit(parameter, data_schema, path, parent) {
        initSettings();
    }

    ~TreeIPEdit() override { delete validator_; };

   private:
    void initSettings() {
        QRegExp rx(R"(((([01]\d\d)|(2[0-4]\d)|(25[0-5])).){3}(([01]\d\d)|(2[0-4]\d)|(25[0-5])))");
        validator_ = new QRegExpValidator(rx);

        setInputMask("000.000.000.000;0");
        setValidator(validator_);
    }

    QValidator *validator_ = nullptr;
};

/*
 *
 *
 */

class TreeComboBox : public QComboBox {
   public:
    explicit TreeComboBox(Parameter_ifs *parameter, DataSchema_ifs *data_schema, const std::string &path,
                          QWidget *parent = nullptr)
        : QComboBox(parent), parameter_(parameter), data_schema_(data_schema), path_(path) {
        setToolTip(data_schema_->help_.c_str());
        for (auto i : data_schema_->enums_) addItem(i.first.c_str());
    }

    BaseSignalController signal_controller_;

   protected:
    std::string path_;
    Parameter_ifs *parameter_ = nullptr;
    DataSchema_ifs *data_schema_ = nullptr;
};

#ifndef TREE_EDITOR_LIB_NAME
#    error "TREE_EDITOR_LIB_NAME undefined"
#endif

QWidget *newTreeEditor(ExtensionManager *manager, QWidget *parent) { return new TreeEditor(manager, parent); }

static ExtensionUnit *g_tree_widget_extension_uint;
static ExtensionInfo g_tree_widget_extension_info;

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, TREE_EDITOR_LIB_NAME)(void) {
    if (QCoreApplication::instance() == nullptr) return nullptr;

    g_tree_widget_extension_uint = new ExtensionUnit[]{
        {"large_text", "tree_widget_wrapper", "return wrapper of TreeTextEdit",
         (void *)newTreeWidgetWrapper<TreeTextEdit>, 0x00},
        {"bool", "tree_widget_wrapper", "return wrapper of TreeCheckBox", (void *)newTreeWidgetWrapper<TreeCheckBox>,
         0x00},
        {"text", "tree_widget_wrapper", "return wrapper of TreeLineEdit", (void *)newTreeWidgetWrapper<TreeLineEdit>,
         0x00},
        {"ip", "tree_widget_wrapper", "return wrapper of TreeLineEdit", (void *)newTreeWidgetWrapper<TreeIPEdit>, 0x00},
        {"number", "tree_widget_wrapper", "return wrapper of TreeLineEdit", (void *)newTreeWidgetWrapper<TreeLineEdit>,
         0x00},
        {"enum", "tree_widget_wrapper", "return wrapper of TreeLineEdit", (void *)newTreeWidgetWrapper<TreeComboBox>,
         0x00},
        {"tree_editor", "tree_editor", "", (void *)newTreeEditor, 0x00},
        {nullptr, nullptr, nullptr, nullptr, 0}};

    g_tree_widget_extension_info = {"tree_widget_extension", 0x01, g_tree_widget_extension_uint};

    return &g_tree_widget_extension_info;
}
