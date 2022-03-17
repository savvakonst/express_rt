#include "qformscreensettings.h"
#include "ui_qformscreensettings.h"

//-------------------------------------------------------------------------
QFormScreenSettings::QFormScreenSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFormScreenSettings)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    Qt::WindowFlags flags = Qt::Window;
    flags |= Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    setWindowTitle(tr("Настройки"));
    setWindowIcon(QIcon("://settings"));

    list_precisions_.push_back(tr("Авто"));
    for(int j = 0; j < 21; j ++){
        list_precisions_.push_back(QString::number(j));
    }

    // Tool Browser Common
    // time
    mrTimeWidth = new QtVariantPropertyManager;
    prTimeWidth = mrTimeWidth->addProperty(QVariant::Int, tr("Длина оси времени, с"));
    prTimeWidth->setToolTip(tr("Длина оси времени, с"));
    prTimeWidth->setAttribute("minimum", 1);
    prTimeWidth->setAttribute("maximum", 60);
    prTimeWidth->setValue(30);

    mrTimeStep = new QtVariantPropertyManager;
    prTimeStep = mrTimeStep->addProperty(QVariant::Double, tr("Период обновления, с"));
    prTimeStep->setToolTip(tr("Период обновления, "));
    prTimeStep->setAttribute("minimum", 0.1);
    prTimeStep->setAttribute("maximum", 2);
    prTimeStep->setAttribute("singleStep", 0.1);
    prTimeStep->setValue(0.5);

    grTime = new QtGroupPropertyManager;
    prTime = grTime->addProperty(tr("Временные параметры"));
    prTime->addSubProperty(prTimeWidth);
    prTime->addSubProperty(prTimeStep);

    // margins
    mrMarginLeft = new QtVariantPropertyManager;
    prMarginLeft = mrMarginLeft->addProperty(QVariant::Int, tr("Слева"));
    prMarginLeft->setToolTip(tr("Размер шрифта"));
    prMarginLeft->setAttribute("minimum", static_cast<int>(DIAGRAM_OFFSET_LEFT));
    prMarginLeft->setAttribute("maximum", 600);
    prMarginLeft->setValue(static_cast<int>(DIAGRAM_OFFSET_LEFT));

    mrMarginRight = new QtVariantPropertyManager;
    prMarginRight = mrMarginRight->addProperty(QVariant::Int, tr("Справа"));
    prMarginRight->setToolTip(tr("Размер шрифта"));
    prMarginRight->setAttribute("minimum", static_cast<int>(DIAGRAM_OFFSET_LEFT));
    prMarginRight->setAttribute("maximum", 600);
    prMarginRight->setValue(static_cast<int>(DIAGRAM_OFFSET_LEFT));

    mrMarginTop = new QtVariantPropertyManager;
    prMarginTop = mrMarginTop->addProperty(QVariant::Int, tr("Сверху"));
    prMarginTop->setToolTip(tr("Размер шрифта"));
    prMarginTop->setAttribute("minimum", static_cast<int>(SCREEN_OFFSET));
    prMarginTop->setAttribute("maximum", 300);
    prMarginTop->setValue(static_cast<int>(SCREEN_OFFSET));

    mrMarginBottom = new QtVariantPropertyManager;
    prMarginBottom = mrMarginBottom->addProperty(QVariant::Int, tr("Снизу"));
    prMarginBottom->setToolTip(tr("Размер шрифта"));
    prMarginBottom->setAttribute("minimum", static_cast<int>(SCREEN_OFFSET));
    prMarginBottom->setAttribute("maximum", 300);
    prMarginBottom->setValue(static_cast<int>(SCREEN_OFFSET));

    grMargins = new QtGroupPropertyManager;
    prMargins = grMargins->addProperty(tr("Отступы области графиков, пикс"));
    prMargins->addSubProperty(prMarginLeft);
    prMargins->addSubProperty(prMarginRight);
    prMargins->addSubProperty(prMarginTop);
    prMargins->addSubProperty(prMarginBottom);

    // line display
    mrCmnDotWeight = new QtVariantPropertyManager;
    prCmnDotWeight = mrCmnDotWeight->addProperty(QVariant::Int, tr("Толщина точек"));
    prCmnDotWeight->setToolTip(tr("Толщина точек"));
    prCmnDotWeight->setAttribute("minimum", 1);
    prCmnDotWeight->setAttribute("maximum", 5);
    prCmnDotWeight->setValue(3);

    mrCmnLineWeight = new QtVariantPropertyManager;
    prCmnLineWeight = mrCmnLineWeight->addProperty(QVariant::Int, tr("Толщина линий"));
    prCmnLineWeight->setToolTip(tr("Толщина линий, соединяющиех точки"));
    prCmnLineWeight->setAttribute("minimum", 1);
    prCmnLineWeight->setAttribute("maximum", 5);
    prCmnLineWeight->setValue(1);

    mrCmnInterpol = new QtVariantPropertyManager;
    prCmnInterpol = mrCmnInterpol->addProperty(QtVariantPropertyManager::enumTypeId(), tr("Интерполяция"));
    prCmnInterpol->setToolTip(tr("Способ соединения точек между собой"));
    prCmnInterpol->setAttribute("enumNames", QStringList() << tr("Нет (точки)") << tr("Ступенчатая") << tr("Линейная"));
    prCmnInterpol->setValue(2);

    grCmnLineDisplay = new QtGroupPropertyManager;
    prCmnLineDisplay = grCmnLineDisplay->addProperty(tr("Отрисовка графика"));

    prCmnLineDisplay->addSubProperty(prCmnDotWeight);
    prCmnLineDisplay->addSubProperty(prCmnLineWeight);
    prCmnLineDisplay->addSubProperty(prCmnInterpol);

    // text
    mrFontSize = new QtVariantPropertyManager;
    prFontSize = mrFontSize->addProperty(QVariant::Int, tr("Размер шрифта"));
    prFontSize->setToolTip(tr("Размер шрифта"));
    prFontSize->setAttribute("minimum", 7);
    prFontSize->setAttribute("maximum", 12);
    prFontSize->setValue(9);

    grTxt = new QtGroupPropertyManager;
    prTxt = grTxt->addProperty(tr("Текст"));
    prTxt->addSubProperty(prFontSize);

    // image saving
    mrImageSaveAuto = new QtVariantPropertyManager;
    prImageSaveAuto = mrImageSaveAuto->addProperty(QVariant::Bool, tr("Автоматически"));
    prImageSaveAuto->setToolTip(tr("Автоматическое сохранение изображения экрана"));
    prImageSaveAuto->setValue(false);

    grImageSave = new QtGroupPropertyManager;
    prImageSave = grImageSave->addProperty(tr("Сохранение изображения"));
    prImageSave->addSubProperty(prImageSaveAuto);

    factory = new QtVariantEditorFactory;
    brCommon  = new QtTreePropertyBrowser;

    brCommon->setFactoryForManager(mrTimeWidth, factory);
    brCommon->setFactoryForManager(mrTimeStep, factory);
    brCommon->addProperty(prTime);

    brCommon->setFactoryForManager(mrMarginLeft, factory);
    brCommon->setFactoryForManager(mrMarginRight, factory);
    brCommon->setFactoryForManager(mrMarginTop, factory);
    brCommon->setFactoryForManager(mrMarginBottom, factory);
    brCommon->addProperty(prMargins);

    brCommon->setFactoryForManager(mrFontSize, factory);
    brCommon->addProperty(prTxt);

    brCommon->setFactoryForManager(mrCmnDotWeight, factory);
    brCommon->setFactoryForManager(mrCmnLineWeight, factory);
    brCommon->setFactoryForManager(mrCmnInterpol, factory);
    //brCommon->addProperty(prCmnLineDisplay);

    brCommon->setFactoryForManager(mrImageSaveAuto, factory);
    brCommon->addProperty(prImageSave);

    brCommon->setHeaderVisible(false);
    brCommon->setMinimumHeight(500);

    ui->layoutCommon->insertWidget(1, brCommon);
    ui->layoutCommon->setStretch(1, 1);

    // Tools Browser Scales
    // color
    mrSclColor = new QtColorPropertyManager;
    prSclColor = mrSclColor->addProperty(tr("Цвет"));
    /*for(auto j = prSclColor->subProperties().begin(); j != prSclColor->subProperties().end(); j ++){

    }*/

    prSclColor->setToolTip(tr("Цвет отображения шкалы"));
    mrSclColor->setValue(prSclColor, QRgb(0xAA0AA0));

    // max
    mrMaxAuto = new QtVariantPropertyManager;
    prMaxAuto = mrMaxAuto->addProperty(QVariant::Bool, tr("Автоматически"));
    prMaxAuto->setToolTip(tr("Автоматический выбор максимума шкалы"));
    prMaxAuto->setValue(true);

    connect(mrMaxAuto, &QtVariantPropertyManager::valueChanged, this, &QFormScreenSettings::on_scaleMaxAuto);

    mrMaxValue = new QtVariantPropertyManager;
    prMaxValue = mrMaxValue->addProperty(QVariant::Double, tr("Значение"));
    prMaxValue->setToolTip(tr("Значение максимума шкалы"));
    prMaxValue->setAttribute("minimum", -999999);
    prMaxValue->setAttribute("maximum", 999999);
    prMaxValue->setValue(100);
    prMaxValue->setEnabled(false);

    grMax = new QtGroupPropertyManager;
    prMax = grMax->addProperty(tr("Максимум"));
    prMax->addSubProperty(prMaxAuto);
    prMax->addSubProperty(prMaxValue);

    // min
    mrMinAuto = new QtVariantPropertyManager;
    prMinAuto = mrMinAuto->addProperty(QVariant::Bool, tr("Автоматически"));
    prMinAuto->setToolTip(tr("Автоматический выбор минимума шкалы"));
    prMinAuto->setValue(true);

    connect(mrMinAuto, &QtVariantPropertyManager::valueChanged, this, &QFormScreenSettings::on_scaleMinAuto);

    mrMinValue = new QtVariantPropertyManager;
    prMinValue = mrMinValue->addProperty(QVariant::Double, tr("Значение"));
    prMinValue->setToolTip(tr("Значение минимума шкалы"));
    prMinValue->setAttribute("minimum", -999999);
    prMinValue->setAttribute("maximum", 999999);
    prMinValue->setValue(0);
    prMinValue->setEnabled(false);

    grMin = new QtGroupPropertyManager;
    prMin = grMin->addProperty(tr("Минимум"));
    prMin->addSubProperty(prMinAuto);
    prMin->addSubProperty(prMinValue);

    // format
    mrDataType = new QtVariantPropertyManager;
    prDataType = mrDataType->addProperty(QtVariantPropertyManager::enumTypeId(), tr("Тип"));
    prDataType->setToolTip(tr("Тип представления данных"));
    prDataType->setAttribute("enumNames", SCALE_DATA_TYPES);
    prDataType->setValue(0);

    connect(mrDataType, &QtVariantPropertyManager::valueChanged, this, &QFormScreenSettings::on_scaleDataType);

    mrPrec = new QtVariantPropertyManager;
    prPrec = mrPrec->addProperty(QtVariantPropertyManager::enumTypeId(), tr("Точность"));
    prPrec->setToolTip(tr("Точность представления чисел с плавающей запятой"));
    prPrec->setAttribute("enumNames", list_precisions_);
    prPrec->setValue(0);

    grFmt = new QtGroupPropertyManager;
    prFmt = grFmt->addProperty(tr("Формат данных"));
    prFmt->addSubProperty(prDataType);
    prFmt->addSubProperty(prPrec);

    // line display
    mrSclDotWeight = new QtVariantPropertyManager;
    prSclDotWeight = mrSclDotWeight->addProperty(QVariant::Int, tr("Толщина точек"));
    prSclDotWeight->setToolTip(tr("Толщина точек"));
    prSclDotWeight->setAttribute("minimum", 1);
    prSclDotWeight->setAttribute("maximum", 5);
    prSclDotWeight->setValue(3);

    mrSclLineWeight = new QtVariantPropertyManager;
    prSclLineWeight = mrSclLineWeight->addProperty(QVariant::Int, tr("Толщина линий"));
    prSclLineWeight->setToolTip(tr("Толщина линий, соединяющиех точки"));
    prSclLineWeight->setAttribute("minimum", 1);
    prSclLineWeight->setAttribute("maximum", 5);
    prSclLineWeight->setValue(1);

    mrSclInterpol = new QtVariantPropertyManager;
    prSclInterpol = mrSclInterpol->addProperty(QtVariantPropertyManager::enumTypeId(), tr("Интерполяция"));
    prSclInterpol->setToolTip(tr("Способ соединения точек между собой"));
    prSclInterpol->setAttribute("enumNames", QStringList() << tr("Нет (точки)") << tr("Ступенчатая") << tr("Линейная"));
    prSclInterpol->setValue(2);

    grSclLineDisplay = new QtGroupPropertyManager;
    prSclLineDisplay = grSclLineDisplay->addProperty(tr("Отрисовка графика"));
    prSclLineDisplay->addSubProperty(prCmnDotWeight);
    prSclLineDisplay->addSubProperty(prCmnLineWeight);
    prSclLineDisplay->addSubProperty(prCmnInterpol);

    // browser
    brScales = new QtTreePropertyBrowser;
    brScales->setHeaderVisible(false);

    factoryColor = new QtColorEditorFactory;
    brScales->setFactoryForManager(mrSclColor, factoryColor);
    //factorySpinBox = new QtSpinBoxFactory;
    //brScales->setFactoryForManager(mrSclColor->subIntPropertyManager(), factorySpinBox);
    brScales->addProperty(prSclColor);

    brScales->setFactoryForManager(mrMaxAuto, factory);
    brScales->setFactoryForManager(mrMaxValue, factory);
    brScales->addProperty(prMax);

    brScales->setFactoryForManager(mrMinAuto, factory);
    brScales->setFactoryForManager(mrMinValue, factory);
    brScales->addProperty(prMin);

    brScales->setFactoryForManager(mrDataType, factory);
    brScales->setFactoryForManager(mrPrec, factory);
    brScales->addProperty(prFmt);

    brScales->setFactoryForManager(mrSclDotWeight, factory);
    brScales->setFactoryForManager(mrSclLineWeight, factory);
    brScales->setFactoryForManager(mrSclInterpol, factory);
    brScales->addProperty(prSclLineDisplay);

    ui->layoutScale->insertWidget(0, brScales);
    ui->layoutScale->setStretch(0, 1);

    // TODO: other

    //ui->groupScale->setEnabled(false);
    //  Single
    ui->treeWidget->setColumnCount(3);
    ui->treeWidget->setColumnWidth(0, 50);
    ui->treeWidget->setColumnWidth(1, 16);
    ui->treeWidget->setColumnWidth(2, 100);
    ui->treeWidget->header()->hide();

    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &QFormScreenSettings::on_scaleTreeClicked);
    connect(ui->treeWidget, &QTreeWidget::itemSelectionChanged, this, &QFormScreenSettings::on_scaleTreeSelected);
    connect(ui->treeWidget, &QTreeWidget::currentItemChanged, this, &QFormScreenSettings::on_scaleTreeCurrentChanged);

    // Buttons
    connect(ui->btnOk, &QPushButton::clicked, this, &QFormScreenSettings::on_confirm);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QFormScreenSettings::on_close);
}
//-------------------------------------------------------------------------
QFormScreenSettings::~QFormScreenSettings()
{
    delete ui;
}
//-------------------------------------------------------------------------
void QFormScreenSettings::setCommon(const QString &title, const SettingsCommon &stx)
{
    title_ = title;

    stx_common_ = stx;

    fill();
}
//-------------------------------------------------------------------------
void QFormScreenSettings::setScales()
{
    //

    fill(SOURCE_SCALE);
}
//-------------------------------------------------------------------------
void QFormScreenSettings::setActiveTab(const int &index)
{
    ui->tabWidget->setCurrentIndex(index);
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_confirm()
{
    bool ok = false;
    SettingsCommon  stx;

    stx.timing.width.fromInt(prTimeWidth->value().toInt(&ok));
    stx.timing.step.fromDouble(prTimeStep->value().toDouble(&ok));

    stx.lining.font_size    = prFontSize->value().toInt(&ok);

    stx.saving.autosave     = prImageSaveAuto->value().toBool();

    stx.lining.line_weight    = prCmnLineWeight->value().toInt(&ok);
    stx.lining.dot_weight     = prCmnDotWeight->value().toInt(&ok);
    stx.lining.interpolation  = prCmnInterpol->value().toInt(&ok);

    stx.margin.left         = prMarginLeft->value().toInt(&ok);
    stx.margin.right        = prMarginRight->value().toInt(&ok);
    stx.margin.top          = prMarginTop->value().toInt(&ok);
    stx.margin.bottom       = prMarginBottom->value().toInt(&ok);

    emit to_confirmed(ui->editTitle->text(), stx);

    SettingsScale   stx_scale;

    stx_scale.color         = mrSclColor->value(prSclColor).rgb();

    stx_scale.fmt.type      = prDataType->value().toInt(&ok);
    stx_scale.fmt.prec      = prPrec->value().toInt(&ok) - 1;

    stx_scale.line.line_weight   = prSclLineWeight->value().toInt(&ok);
    stx_scale.line.dot_weight    = prSclDotWeight->value().toInt(&ok);
    stx_scale.line.interpolation = prSclInterpol->value().toInt(&ok);

    emit to_confirm_scale();

    close();
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_close()
{
    close();
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleTreeClicked(QTreeWidgetItem *item, int column)
{

}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleTreeSelected()
{

}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleTreeCurrentChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();

    ui->groupScale->setEnabled(!list.isEmpty());

    if(list.isEmpty()){
        ui->groupScale->setTitle(tr("Шкала не выбрана..."));
        return;
    }

    fill(SOURCE_SCALE);
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleColor(QtProperty *_t1, const QVariant &_t2)
{
    Q_UNUSED(_t1)
    Q_UNUSED(_t2)

    uint32_t color = mrSclColor->value(prSclColor).rgb();

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleMinAuto(QtProperty *_t1, const QVariant &_t2)
{
    Q_UNUSED(_t1)
    Q_UNUSED(_t2)

    bool    ok = false;
    bool    enabled = !prMinAuto->value().toBool();

    prMinValue->setEnabled(enabled);

    int val = prMinAuto->value().toInt(&ok);

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleMinValue(QtProperty *_t1, const QVariant &_t2)
{
    Q_UNUSED(_t1)
    Q_UNUSED(_t2)

    bool    ok  = false;
    int     val = prMinValue->value().toInt(&ok) - 1;

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleMaxAuto(QtProperty *_t1, const QVariant &_t2)
{
    Q_UNUSED(_t1)
    Q_UNUSED(_t2)

    bool    ok = false;
    bool    enabled = !prMaxAuto->value().toBool();

    prMaxValue->setEnabled(enabled);

    int val = prMaxAuto->value().toInt(&ok);

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleMaxValue(QtProperty *_t1, const QVariant &_t2)
{
    Q_UNUSED(_t1)
    Q_UNUSED(_t2)

    bool    ok  = false;
    int     val = prMaxValue->value().toInt(&ok) - 1;

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleDataType(QtProperty *_t1, const QVariant &_t2)
{
    Q_UNUSED(_t1)
    Q_UNUSED(_t2)

    bool    ok = false;
    int     index = prDataType->value().toInt(&ok);

    switch(index){
    case DATA_OUTPUT_FORMAT_INT_16:
    case DATA_OUTPUT_FORMAT_INT_10:
    case DATA_OUTPUT_FORMAT_INT_8:
    case DATA_OUTPUT_FORMAT_INT_2:
        prPrec->setEnabled(false);
        break;
    default:
        prPrec->setEnabled(true);
    }

    int     val = prDataType->value().toInt(&ok);

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scalePrec(QtProperty *_t1, const QVariant &_t2)
{
    Q_UNUSED(_t1)
    Q_UNUSED(_t2)

    bool    ok  = false;
    int     val = prPrec->value().toInt(&ok);

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleLineWeight(QtProperty *_t1, const QVariant &_t2)
{
    Q_UNUSED(_t1)
    Q_UNUSED(_t2)

    bool    ok  = false;
    int     val = prSclLineWeight->value().toInt(&ok);

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleDotWeight(QtProperty *_t1, const QVariant &_t2)
{
    Q_UNUSED(_t1)
    Q_UNUSED(_t2)

    bool    ok  = false;
    int     val = prSclDotWeight->value().toInt(&ok);

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;
}
//-------------------------------------------------------------------------
void QFormScreenSettings::on_scaleInterpol(QtProperty *_t1, const QVariant &_t2)
{
    Q_UNUSED(_t1)
    Q_UNUSED(_t2)

    bool    ok  = false;
    int     val = prSclInterpol->value().toInt(&ok);

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;
}
//-------------------------------------------------------------------------
void QFormScreenSettings::fill(const int &src)
{
    switch(src){
    case SOURCE_SCALE:{
        ui->treeWidget->clear();
    }
    break;
    default:
        ui->editTitle->setText(title_);

        mrTimeWidth->setValue(prTimeWidth, stx_common_.timing.width.toInt());
        mrTimeStep->setValue(prTimeStep, stx_common_.timing.step.toDouble());

        mrMarginLeft->setValue(prMarginLeft, stx_common_.margin.left);
        mrMarginRight->setValue(prMarginRight, stx_common_.margin.right);
        mrMarginTop->setValue(prMarginTop, stx_common_.margin.top);
        mrMarginBottom->setValue(prMarginBottom, stx_common_.margin.bottom);

        mrFontSize->setValue(prFontSize, stx_common_.lining.font_size);

        mrImageSaveAuto->setValue(prImageSaveAuto, stx_common_.saving.autosave);

        mrCmnLineWeight->setValue(prCmnLineWeight, stx_common_.lining.line_weight);
        mrCmnDotWeight->setValue(prCmnDotWeight, stx_common_.lining.dot_weight);
        mrCmnInterpol->setValue(prCmnInterpol, stx_common_.lining.interpolation);
    }
}
//-------------------------------------------------------------------------
void QFormScreenSettings::closeEvent(QCloseEvent *event)
{
    event->accept();
}
//-------------------------------------------------------------------------
