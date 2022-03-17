#ifndef QFORMSCREENSETTINGS_H
#define QFORMSCREENSETTINGS_H

#include <QDialog>
#include <QString>
#include <QCloseEvent>

#include "Express_Online_Screen.h"

#include "qttreepropertybrowser.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qtvariantproperty.h"

namespace Ui {
class QFormScreenSettings;
}

class QFormScreenSettings : public QDialog
{
    Q_OBJECT

    const QStringList LINE_TYPES        = QStringList() << tr("Сплошная") << tr("Пунктирная") << tr("Штриховая") << tr("Штрихпунктирная");
    const QStringList SCALE_DATA_TYPES  = QStringList() << tr("Вещественное")
                                                        << tr("Целое (16)")
                                                       << tr("Целое (10)")
                                                       << tr("Целое (8)")
                                                       << tr("Целое (2)")
                                                       << tr("Угол (° ' \" из °)")
                                                       << tr("Время (ч:м:с из с)");
public:
    explicit QFormScreenSettings(QWidget *parent = nullptr);
    ~QFormScreenSettings();

    void setCommon(const QString &title, const SettingsCommon &stx);
    void setScales();

    void setActiveTab(const int &index = 0);

private slots:
    void on_confirm();
    void on_close();

    void on_scaleTreeClicked(QTreeWidgetItem *item, int column);
    void on_scaleTreeSelected();
    void on_scaleTreeCurrentChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_scaleColor(QtProperty *_t1, const QVariant &_t2);
    void on_scaleMinAuto(QtProperty *_t1, const QVariant &_t2);
    void on_scaleMinValue(QtProperty *_t1, const QVariant &_t2);
    void on_scaleMaxAuto(QtProperty *_t1, const QVariant &_t2);
    void on_scaleMaxValue(QtProperty *_t1, const QVariant &_t2);
    void on_scaleDataType(QtProperty *_t1, const QVariant &_t2);
    void on_scalePrec(QtProperty *_t1, const QVariant &_t2);
    void on_scaleLineWeight(QtProperty *_t1, const QVariant &_t2);
    void on_scaleDotWeight(QtProperty *_t1, const QVariant &_t2);
    void on_scaleInterpol(QtProperty *_t1, const QVariant &_t2);

private:
    void fill(const int &src = -1);

    Ui::QFormScreenSettings     *ui;

    // Tool Browser Common
    // line
    QtVariantPropertyManager    *mrCmnDotWeight;
    QtVariantProperty           *prCmnDotWeight;

    QtVariantPropertyManager    *mrCmnLineWeight;
    QtVariantProperty           *prCmnLineWeight;

    QtVariantPropertyManager    *mrCmnInterpol;
    QtVariantProperty           *prCmnInterpol;

    QtGroupPropertyManager      *grCmnLineDisplay;
    QtProperty                  *prCmnLineDisplay;

    // text
    QtVariantPropertyManager    *mrFontSize;
    QtVariantProperty           *prFontSize;

    QtGroupPropertyManager      *grTxt;
    QtProperty                  *prTxt;

    // time
    QtVariantPropertyManager    *mrTimeWidth;
    QtVariantProperty           *prTimeWidth;
    QtVariantPropertyManager    *mrTimeStep;
    QtVariantProperty           *prTimeStep;

    QtGroupPropertyManager      *grTime;
    QtProperty                  *prTime;

    // margins
    QtVariantPropertyManager    *mrMarginLeft;
    QtVariantProperty           *prMarginLeft;

    QtVariantPropertyManager    *mrMarginRight;
    QtVariantProperty           *prMarginRight;

    QtVariantPropertyManager    *mrMarginTop;
    QtVariantProperty           *prMarginTop;

    QtVariantPropertyManager    *mrMarginBottom;
    QtVariantProperty           *prMarginBottom;

    QtGroupPropertyManager      *grMargins;
    QtProperty                  *prMargins;

    // image saving
    QtVariantPropertyManager    *mrImageSaveAuto;
    QtVariantProperty           *prImageSaveAuto;

    //QtVariantPropertyManager    *mrImageSavePath;
    //QtVariantProperty           *prImageSavePAth;

    QtGroupPropertyManager      *grImageSave;
    QtProperty                  *prImageSave;

    // browser
    QtVariantEditorFactory      *factory;
    QtSpinBoxFactory            *factorySpinBox;
    QtColorEditorFactory        *factoryColor;
    QtTreePropertyBrowser       *brCommon;

    // Tool Browser Scales
    // line
    QtColorPropertyManager      *mrSclColor;
    QtProperty                  *prSclColor;

    QtVariantPropertyManager    *mrSclDotWeight;
    QtVariantProperty           *prSclDotWeight;

    QtVariantPropertyManager    *mrSclLineWeight;
    QtVariantProperty           *prSclLineWeight;

    QtVariantPropertyManager    *mrSclInterpol;
    QtVariantProperty           *prSclInterpol;

    QtGroupPropertyManager      *grSclLineDisplay;
    QtProperty                  *prSclLineDisplay;

    // extremums
    QtVariantPropertyManager    *mrMaxAuto;
    QtVariantProperty           *prMaxAuto;

    QtVariantPropertyManager    *mrMaxValue;
    QtVariantProperty           *prMaxValue;

    QtGroupPropertyManager      *grMax;
    QtProperty                  *prMax;

    QtVariantPropertyManager    *mrMinAuto;
    QtVariantProperty           *prMinAuto;

    QtVariantPropertyManager    *mrMinValue;
    QtVariantProperty           *prMinValue;

    QtGroupPropertyManager      *grMin;
    QtProperty                  *prMin;

    // Format
    QtVariantPropertyManager    *mrDataType;
    QtVariantProperty           *prDataType;

    QtVariantPropertyManager    *mrPrec;
    QtVariantProperty           *prPrec;

    QtGroupPropertyManager      *grFmt;
    QtProperty                  *prFmt;

    QStringList                 list_precisions_;

    // greed ?


    QtTreePropertyBrowser       *brScales;
    //

    QString                     title_;
    SettingsCommon              stx_common_;
    //QList<>                     stx_scales_;

    //SettingsLineDisplay         line_;
    //SettingsMarginPosition      margin_;

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void    to_confirmed(const QString title, const SettingsCommon stx_cmn);
    void    to_confirm_scale();
};

#endif // QFORMSCREENSETTINGS_H
