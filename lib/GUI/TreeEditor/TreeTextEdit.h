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
#include "convtemplate/Parameter_ifs.h"

class TreeTextEdit : public QPlainTextEdit {
    Q_OBJECT
   public:
    explicit TreeTextEdit(Parameter_ifs *parameter, DataSchema_ifs *data_schema, const std::string &path,
                          QWidget *parent = nullptr)
        :  //
          QPlainTextEdit(parent),
          parameter_(parameter),
          data_schema_(data_schema),
          path_(path) {
        initSettings();
    }

    BaseSignalController signal_controller_;
   private slots:

    void setHeight() {
        int n_rows = this->document()->blockCount() + 1;

        QFontMetrics m(this->font());
        int row_height = m.lineSpacing();
        this->setFixedHeight(n_rows * row_height);
        signal_controller_.emit_();
    }

   protected:
    void initSettings() {
        setTabStopWidth(24);
        connect(this, SIGNAL(textChanged()), this, SLOT(setHeight()));
        setHeight();
    }

   private:
    [[maybe_unused]] bool change_lock_ = true;

    std::string path_;
    Parameter_ifs *parameter_ = nullptr;
    DataSchema_ifs *data_schema_ = nullptr;
};
#endif  // EXRT_TREETEXTEDIT_H
