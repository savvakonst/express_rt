
#define _USE_MATH_DEFINES

#include "GUI/TreeEditor.h"

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


void TreeEditor::setupProperties(DataSchema_ifs *ds, QTreeWidgetItem *parent_item) {
    data_schema_ = ds;

    if (ds->isMap()) {
        auto map_list = ds->getMapList();
        for (auto i : map_list) {
            addProperty(i, nullptr);
        }
    }
    expandAll();
}

void TreeEditor::addProperty(DataSchema_ifs *ds, QTreeWidgetItem *parent_item) {

    auto item = parent_item ? new QTreeWidgetItem(parent_item) : new QTreeWidgetItem(this);

    auto name = ds->description_.c_str();
    item->setText(0, name);

    if (ds->isMap()) {
        auto map_list = ds->getMapList();
        for (auto i : map_list) {
            addProperty(i, item);
        }
    } else if (ds->isArray()) {
        // TODO: realise this branch of logic
    } else {
        auto type = ds->getRepresentationType();
        auto cm = constructors_map_.find(type);

        if (cm != constructors_map_.end()) {
            TreeWidgetWrapper_ifs *w;
            auto list = *(*cm).second;
            for (treeWidgetWrapperConstructor wrapperConstructor : list)
                if (w = wrapperConstructor(ds, nullptr)) break;

            if (w) {
                // if (!w->getWidget()->metaObject()->inherits(&QComboBox::staticMetaObject))
                w->getWidget()->setStyleSheet(
                    "QWidget {background-color: rgba(0,0,0,0);border-style: outset; border-width: 0px;}"
                    "QWidget QAbstractItemView {\n"
                    "    background: white;"
                    "}");

                this->setItemWidget(item, 1, w->getWidget());
                w->addSignal(&update_signal_);

                delete w;
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
 */

class TreeCheckBox : public QCheckBox {
   public:
    explicit TreeCheckBox(QWidget *parent = nullptr) : QCheckBox(parent) {}

    explicit TreeCheckBox(const QString &text, QWidget *parent = nullptr)
        :  //
          QCheckBox(text, parent) {}

    explicit TreeCheckBox(DataSchema_ifs *data_schema, QWidget *parent = nullptr)
        :  //
          QCheckBox(parent),
          data_schema_(data_schema) {}

    explicit TreeCheckBox(DataSchema_ifs *data_schema, const QString &str, QWidget *parent = nullptr)
        :  //
          QCheckBox(str, parent),
          data_schema_(data_schema) {}

    void setDataSchema(DataSchema_ifs *data_schema) { data_schema_ = data_schema; }

    BaseSignalController signal_controller_;

   private:
    DataSchema_ifs *data_schema_ = nullptr;
};

/*
 *
 *
 */

class TreeLineEdit : public QLineEdit {
   public:
    explicit TreeLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) { initSettings(); }

    explicit TreeLineEdit(const QString &str, QWidget *parent = nullptr)
        :  //
          QLineEdit(str, parent) {
        initSettings();
    }

    explicit TreeLineEdit(DataSchema_ifs *data_schema, QWidget *parent = nullptr)
        :  //
          QLineEdit(parent),
          data_schema_(data_schema) {
        initSettings();
    }

    explicit TreeLineEdit(DataSchema_ifs *data_schema, const QString &str, QWidget *parent = nullptr)
        :  //
          QLineEdit(str, parent),
          data_schema_(data_schema) {
        initSettings();
    }

    void setDataSchema(DataSchema_ifs *data_schema) { data_schema_ = data_schema; }

    BaseSignalController signal_controller_;

   private:
    void initSettings() {}

    DataSchema_ifs *data_schema_ = nullptr;
};

/*
 *
 *
 */

class TreeIPEdit : public TreeLineEdit {
   public:
    explicit TreeIPEdit(QWidget *parent = nullptr) : TreeLineEdit(parent) { initSettings(); }

    explicit TreeIPEdit(const QString &str, QWidget *parent = nullptr) : TreeLineEdit(str, parent) { initSettings(); }

    explicit TreeIPEdit(DataSchema_ifs *data_schema, QWidget *parent = nullptr) : TreeLineEdit(data_schema, parent) {
        initSettings();
    }

    explicit TreeIPEdit(DataSchema_ifs *data_schema, const QString &str, QWidget *parent = nullptr)
        : TreeLineEdit(data_schema, str, parent) {
        initSettings();
    }

    ~TreeIPEdit() override { delete validator_; };

    void setDataSchema(DataSchema_ifs *data_schema) { data_schema_ = data_schema; }

   private:
    void initSettings() {
        QRegExp rx("((([01]\\d\\d)|(2[0-4]\\d)|(25[0-5])).){3}(([01]\\d\\d)|(2[0-4]\\d)|(25[0-5]))");
        validator_ = new QRegExpValidator(rx);

        setInputMask("000.000.000.000;0");
        setValidator(validator_);
    }
    QValidator *validator_ = nullptr;
    DataSchema_ifs *data_schema_ = nullptr;
};

/*
 *
 *
 */

class TreeComboBox : public QComboBox {
   public:
    explicit TreeComboBox(QWidget *parent = nullptr)
        :  //
          QComboBox(parent) {}

    explicit TreeComboBox(DataSchema_ifs *data_schema, QWidget *parent = nullptr)
        :  //
          QComboBox(parent),
          data_schema_(data_schema) {
        for (auto i : data_schema_->enums_) addItem(i.first.c_str());
    }

    void setDataSchema(DataSchema_ifs *data_schema) {
        data_schema_ = data_schema;
        this->clear();
        for (auto i : data_schema_->enums_) addItem(i.first.c_str());
    }

    BaseSignalController signal_controller_;

   private:
    DataSchema_ifs *data_schema_ = nullptr;
};

#ifndef TREE_EDITOR_LIB_NAME
#    error "TREE_EDITOR_LIB_NAME undefined"
#endif

QWidget *newTreeEditor(ExtensionManager *manager, QWidget *parent) { return new TreeEditor(manager, parent); }

static ExtensionUnit *g_tree_widget_extension_uint;
static ExtensionInfo g_tree_widget_extension_info;

#include <QApplication>

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
