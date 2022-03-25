#include "qformlevel.h"
#include "ui_qformlevel.h"

//-------------------------------------------------------------------------
QFormLevel::QFormLevel(const ControlLevel &lvl, const bool &flag, const int &index, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFormLevel)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    mode_       = flag;
    lvlIndex_   = index;
    //lvlStore_   = lvl;

    Qt::WindowFlags flags = Qt::Sheet;
    //flags   |= Qt::WindowMinMaxButtonsHint;
    flags   |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    setWindowTitle(tr("Добавить уровень"));
    setWindowIcon(QIcon("://chart_dot"));

    vld = new QDoubleValidator(this);
    ui->editValue->setValidator(vld);

    ui->editValue->setText(QString::number(lvl.value));

    ui->comboCross->clear();
    ui->comboCross->addItem(tr("Ниже"), false);
    ui->comboCross->addItem(tr("Выше"), true);
    ui->comboCross->setCurrentIndex(static_cast<int>(lvl.cross));
}
//-------------------------------------------------------------------------
QFormLevel::~QFormLevel()
{
    delete ui;
}
//-------------------------------------------------------------------------
void QFormLevel::on_btnOk_clicked()
{
    ControlLevel lvl;

    lvl.value   = ui->editValue->text().replace(QLocale::system().decimalPoint(), QLocale(QLocale::English).decimalPoint()).toDouble();
    lvl.cross   = ui->comboCross->currentData(Qt::UserRole).toBool();

    emit to_confirmed(lvl, mode_, lvlIndex_);

    close();
}
//-------------------------------------------------------------------------
void QFormLevel::on_btnCancel_clicked()
{
    close();
}
//-------------------------------------------------------------------------
void QFormLevel::closeEvent(QCloseEvent *event)
{
    event->accept();
}
//-------------------------------------------------------------------------
