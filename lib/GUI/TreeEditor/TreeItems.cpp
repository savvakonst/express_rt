//
// Created by SVK on 29.07.2022.
//

#define _USE_MATH_DEFINES

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QStringList>
#include <QTreeWidget>
#include <iostream>

#include "GUI/TreeEditor.h"
#include "TreeTextEdit.h"
#include "common/ExtensionManager.h"

/*
 *
 *
 *
 *
 */
#ifdef DEBUG_
#    include <QDebug>
#endif

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
        auto h = parameter_->getProperty(path_);
        if (h && isString(h->getValue().type_)) {
            setText(h->getValue().asString().c_str());
        } else {
            setText("invalid_value");
        }
        setToolTip(data_schema_->help_.c_str());
        initSettings();

        connect(this, &QLineEdit::textEdited, this, &TreeLineEdit::textWasEdited);
    }

    BaseSignalController signal_controller_;

   public slots:
    void textWasEdited(const QString &text) { parameter_->setProperty(path_, Value(text.toStdString())); }

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

class TreeIPEdit : public QLineEdit {
   public:
    explicit TreeIPEdit(Parameter_ifs *parameter, DataSchema_ifs *data_schema, const std::string &path,
                        QWidget *parent = nullptr)
        :  //
          QLineEdit(parent),
          parameter_(parameter),
          data_schema_(data_schema),
          path_(path) {
        initSettings();
        setupValue();
    }

    void setupValue() {
        auto h = parameter_->getProperty(path_);
        if (h && isNum(h->getValue().type_)) {
            union TempUnion {
                u8_t b_[4];
                u32_t dw_;
            };
            TempUnion temp{};
            temp.dw_ = h->getValue().value_.u32;

            auto s = QString("%1.%2.%3.%4")
                         .arg(temp.b_[3], 3, 10, QLatin1Char('0'))
                         .arg(temp.b_[2], 3, 10, QLatin1Char('0'))
                         .arg(temp.b_[1], 3, 10, QLatin1Char('0'))
                         .arg(temp.b_[0], 3, 10, QLatin1Char('0'));
            qDebug() << s;
            setText(s);
        } else {
            setText("invalid_value");
        }
    }
    ~TreeIPEdit() override { delete validator_; };

    BaseSignalController signal_controller_;

   private:
    void initSettings() {
        QRegExp rx(R"(((([01]\d\d)|(2[0-4]\d)|(25[0-5])).){3}(([01]\d\d)|(2[0-4]\d)|(25[0-5])))");
        validator_ = new QRegExpValidator(rx);

        setInputMask("000.000.000.000;0");
        setValidator(validator_);
    }

    QValidator *validator_ = nullptr;

   protected:
    std::string path_;
    Parameter_ifs *parameter_ = nullptr;
    DataSchema_ifs *data_schema_ = nullptr;
};

/*
 *
 *
 */

class TreeNumEdit : public QLineEdit {
   public:
    explicit TreeNumEdit(Parameter_ifs *parameter, DataSchema_ifs *data_schema, const std::string &path,
                         QWidget *parent = nullptr)
        :  //
          QLineEdit(parent),
          parameter_(parameter),
          data_schema_(data_schema),
          path_(path) {
        initSettings();

        auto h = parameter_->getProperty(path_);

        if (h && isNum(h->getValue().type_)) {
            setText(h->getValue().asString().c_str());
            type_ = h->getValue().type_;
        } else {
            setText("invalid_value");
        }
        setToolTip(data_schema_->help_.c_str());

        connect(this, &QLineEdit::textEdited, this, &TreeNumEdit::textWasEdited);
    }

    ~TreeNumEdit() override { delete validator_; };

    BaseSignalController signal_controller_;

   public slots:
    void textWasEdited(const QString &text) { parameter_->setProperty(path_, Value(text.toStdString(), type_)); }

   private:
    void initSettings() {
        QRegExp rx(R"([0-9]*(.[0-9]+)?)");
        validator_ = new QRegExpValidator(rx);
        setValidator(validator_);
    }

    QValidator *validator_ = nullptr;

   protected:
    std::string path_;
    DataType type_ = DataType::none_v;
    Parameter_ifs *parameter_ = nullptr;
    DataSchema_ifs *data_schema_ = nullptr;
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

/*
 *
 *
 */

#ifndef TREE_EDITOR_LIB_NAME
#    error "TREE_EDITOR_LIB_NAME undefined"
#endif

QWidget *newTreeEditor(QWidget *parent) { return new TreeEditor(parent); }

static ExtensionUnit *g_tree_widget_extension_uint;
static ExtensionInfo g_tree_widget_extension_info;

static int initTreeEditor_(ExtensionManager *manager) {
    auto p_unit = search(g_tree_widget_extension_uint, "tree_editor", "tree_editor");

    if (p_unit != manager->getLastVersionExtensionUnit("tree_editor", "tree_editor")) {
        DEBUG_CERR("cant init (name: " << p_unit->name << ", type: " << p_unit->type << ", ver.:" << p_unit->version
                                       << ") unit, since there is a newer unit.\n");
        return 1;
    }

    auto tree = (TreeEditor *)p_unit->ptr;
    if (tree) tree->addExtensionUint(manager);
    return 0;
}

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
        {"number", "tree_widget_wrapper", "return wrapper of TreeLineEdit", (void *)newTreeWidgetWrapper<TreeNumEdit>,
         0x00},
        {"enum", "tree_widget_wrapper", "return wrapper of TreeLineEdit", (void *)newTreeWidgetWrapper<TreeComboBox>,
         0x00},
        {"tree_editor", "tree_editor", "", (void *)newTreeEditor(nullptr), 0x00},
        {"tree_editor", "init", "", (void *)&initTreeEditor_, 0x00},
        {nullptr, nullptr, nullptr, nullptr, 0}};

    g_tree_widget_extension_info = {"tree_widget_extension", 0x01, g_tree_widget_extension_uint};

    return &g_tree_widget_extension_info;
}
