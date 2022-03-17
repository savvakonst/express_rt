#ifndef QFORMLEVEL_H
#define QFORMLEVEL_H

#include <QDialog>
#include <QCloseEvent>
#include <QDoubleValidator>

#include "Express_Online_Screen.h"

namespace Ui {
class QFormLevel;
}

class QFormLevel : public QDialog
{
    Q_OBJECT

public:
    explicit QFormLevel(const ControlLevel &lvl, const bool &flag, const int &index = -1, QWidget *parent = nullptr);
    ~QFormLevel();

private:
    Ui::QFormLevel *ui;

    QDoubleValidator                *vld;

    bool                            mode_;
    int                             lvlIndex_; // @ levels list
    //ControlLevel                    lvlStore_;

private slots:
    void                            on_btnOk_clicked();
    void                            on_btnCancel_clicked();

    // QWidget interface
protected:
    void                            closeEvent(QCloseEvent *event) override;

signals:
    void                            to_confirmed(const ControlLevel &lvl, const bool &flag, const int &index);
};

#endif // QFORMLEVEL_H
