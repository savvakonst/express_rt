#ifndef TREE_WIDGET_H
#define TREE_WIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
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

/*
template<class T>
QWidget *createTreeItemWidget(DataSchema_ifs *data_schema, QWidget *parent = nullptr) {
    return new T(parent);
}
*/

class TreeWidgetWrapper_ifs {
   public:
    virtual ~TreeWidgetWrapper_ifs() {}

    virtual QWidget *getWidget() = 0;

    virtual status addSignal(Signal_ifs *signal) = 0;
};

template <class T>
class TreeWidgetWrapper : public TreeWidgetWrapper_ifs {
   public:
    TreeWidgetWrapper(T *widget) : widget_(widget) {}

    TreeWidgetWrapper(const TreeWidgetWrapper &) = delete;

    TreeWidgetWrapper(const TreeWidgetWrapper &&) = delete;

    ~TreeWidgetWrapper() override {}

    virtual QWidget *getWidget() override { return widget_; }

    virtual status addSignal(Signal_ifs *signal) override { return widget_->signal_controller_.addSignal(signal); }

    virtual void setDataSchema(DataSchema_ifs *ds) { widget_->setDataSchema(ds); }

   private:
    T *widget_;
};

class TreeTextEdit : public QPlainTextEdit {
    Q_OBJECT
   public:
    explicit TreeTextEdit(QWidget *parent = nullptr) : QPlainTextEdit(parent) { initSettings(); }

    explicit TreeTextEdit(const QString &text, QWidget *parent = nullptr)
        :  //
          QPlainTextEdit(parent) {
        insertPlainText(text);
        initSettings();
    }

    explicit TreeTextEdit(DataSchema_ifs *data_schema, QWidget *parent = nullptr)
        :  //
          QPlainTextEdit(parent),
          data_schema_(data_schema) {
        initSettings();
    }

    explicit TreeTextEdit(DataSchema_ifs *data_schema, const QString &text, QWidget *parent = nullptr)
        :  //
          QPlainTextEdit(parent),
          data_schema_(data_schema) {
        insertPlainText(text);
        initSettings();
    }

    void setDataSchema(DataSchema_ifs *data_schema) { data_schema_ = data_schema; }

    BaseSignalController signal_controller_;
   private slots:

    void setHeight() {
        int n_rows = this->document()->blockCount() + 1;

        QFontMetrics m(this->font());
        int row_height = m.lineSpacing();
        this->setFixedHeight(n_rows * row_height);
        signal_controller_.emit_();
    }

   private:
    bool change_lock = true;

    void initSettings() {
        setTabStopWidth(24);
        connect(this, SIGNAL(textChanged()), this, SLOT(setHeight()));
        setHeight();
    }

    DataSchema_ifs *data_schema_ = nullptr;
};

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

/*
 *
 *
 */
#include <QDebug>
#include <QStyledItemDelegate>

class TreeEditor : public QTreeWidget {
   public:
    friend class StyledItemDelegate;

    explicit TreeEditor(QWidget *parent = nullptr);

    explicit TreeEditor(DataSchema_ifs *data_schema, QWidget *parent = nullptr) : TreeEditor(parent) {
        data_schema_ = data_schema;
        // setupProperties(data_schema_, nullptr);
    }

    void setupProperties(DataSchema_ifs *ds, QTreeWidgetItem *parent_item = nullptr);

    void setupProperties() {
        setupProperties(data_schema_);
        expandAll();
    }

   private:
    class UpdateSignal : public Signal_ifs {
       public:
        UpdateSignal(TreeEditor *tree_widget) : tree_widget_(tree_widget) {}

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

    std::string error_message_ = "";

    DataSchema_ifs *data_schema_ = nullptr;
};

template <class T>
TreeWidgetWrapper_ifs *newTreeWW(DataSchema_ifs *data_schema, QWidget *parent = nullptr) {
    return new TreeWidgetWrapper(new T(data_schema, parent));
}

#endif  // MAI