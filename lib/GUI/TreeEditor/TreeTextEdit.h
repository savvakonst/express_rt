//
// Created by SVK on 21.01.2022.
//

#ifndef EXRT_TREETEXTEDIT_H
#define EXRT_TREETEXTEDIT_H

#include <QPainter>
#include <QPalette>
#include <QPlainTextEdit>
#include <QStringList>

#include "common/BaseClass_ifs.h"
#include "common/DataSchema_ifs.h"
#include "common/Extension.h"

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
    [[maybe_unused]] bool change_lock_ = true;

    void initSettings() {
        setTabStopWidth(24);
        connect(this, SIGNAL(textChanged()), this, SLOT(setHeight()));
        setHeight();
    }

    DataSchema_ifs *data_schema_ = nullptr;
};
#endif  // EXRT_TREETEXTEDIT_H
