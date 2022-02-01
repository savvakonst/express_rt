
#include <QHeaderView>
#include <QTableView>
//
#include "ConvTemplateList.h"
#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/ConversionTemplateManager.h"

TreeModel::TreeModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "conversion_template");
    if (unit && unit->ptr) {
        schema_ = (DataSchema_ifs *)unit->ptr;
        schema_->init(manager);
        list_of_entries_ = schema_->getMapList();
    }

    unit = manager->getLastVersionExtensionUint("cnv_template_manager", "cnv_template_manager");
    if (unit && unit->ptr) {
        manager_ = (ConversionTemplateManager *)unit->ptr;
        std::cout << "manager_ = (ConversionTemplateManager *)unit->ptr;\n";
    }
}

// TreeModel::TreeModel(const QString &data, QObject *parent) {}
TreeModel::~TreeModel() = default;
QVariant TreeModel::data(const QModelIndex &index, int role) const { return {}; }
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const { return QAbstractItemModel::flags(index); }

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        return QString(list_of_entries_[section]->description_.data());
    }
    return {};
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const { return {}; }
QModelIndex TreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return QModelIndex();

    return createIndex();
}

int TreeModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) return 0;

    if (!parent.isValid()) parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem *>(parent.internalPointer());

    return parentItem->childCount();

    return (int)manager_->getEntriesNumber();
}
int TreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 1;
    return (int)list_of_entries_.size();
}

/*
 *
 *
 *
 *
 */

TableModel::TableModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "conversion_template");
    if (unit && unit->ptr) {
        schema_ = (DataSchema_ifs *)unit->ptr;
        schema_->init(manager);
        list_of_entries_ = schema_->getMapList();
    }

    unit = manager->getLastVersionExtensionUint("cnv_template_manager", "cnv_template_manager");
    if (unit && unit->ptr) {
        manager_ = (ConversionTemplateManager *)unit->ptr;
        std::cout << "manager_ = (ConversionTemplateManager *)unit->ptr;\n";
    }
}

int TableModel::rowCount(const QModelIndex &parent) const { return (int)manager_->getEntriesNumber(); }

int TableModel::columnCount(const QModelIndex &parent) const { return (int)list_of_entries_.size(); }

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        return QString(list_of_entries_[section]->description_.data());
    }
    return {};
}

QVariant TableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        auto conv_template = manager_->getConversionTemplateByIndex(index.row());
        auto name = list_of_entries_[index.column()]->name_;
        auto data = conv_template->getInfo(name);
        //=data->getValue().asString();
        // QString answer =
        //      QString("row = ") + QString::number(index.row()) + "  col = " + QString::number(index.column());

        return {data->getValue().asString().data()};
    }
    return {};
}

/*
 *
 *
 *
 *
 */

#ifndef CONV_TEMPLATE_LIST_LIB_NAME
#    error "CONV_TEMPLATE_LIST_LIB_NAME undefined"
#endif

QWidget *newConvTemplateList(QWidget *parent) {
    auto table_view = new QTableView();

    QHeaderView *vertical_header = table_view->verticalHeader();

    vertical_header->setDefaultSectionSize(vertical_header->minimumSectionSize());  // 20 px height
    vertical_header->sectionResizeMode(QHeaderView::Fixed);
    vertical_header->hide();

    table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_view->setAlternatingRowColors(true);

    table_view->setStyleSheet(
        "QHeaderView::section {background-color: #D2DCDF; alternate-background-color: #f6fafb;};");

    return table_view;
}

static int initConvTemplateListWidget(ExtensionManager *manager);
static ExtensionUnit *g_conv_template_list_extension_uint = nullptr;
static ExtensionInfo g_conv_template_list_extension_info;

#include <QApplication>
InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, CONV_TEMPLATE_LIST_LIB_NAME)(void) {
    if (QCoreApplication::instance() == nullptr) return nullptr;

    g_conv_template_list_extension_uint = new ExtensionUnit[]{
        {"conv_template_list", "widget",
         "returns widget instance, which provides list of available conversion templates",
         (void *)newConvTemplateList(nullptr), 0x00},
        {"conv_template_list", "init", "checks the version and, if it's the latest, initializes the widget.",
         (void *)&initConvTemplateListWidget, 0x00},
        {nullptr, nullptr, nullptr, nullptr, 0}};

    g_conv_template_list_extension_info = {"conv_template_list_extension", 0x01, g_conv_template_list_extension_uint};

    return &g_conv_template_list_extension_info;
}

static int initConvTemplateListWidget(ExtensionManager *manager) {
    auto p_unit = search(g_conv_template_list_extension_uint, "widget", "conv_template_list");

    if (p_unit != manager->getLastVersionExtensionUint("widget", "conv_template_list")) {
        DEBUG_CERR("cant init (name: " << p_unit->name << ", type: " << p_unit->type << ", ver.:" << p_unit->version
                                       << ") unit, since there is a newer unit.\n");
        return 0;
    }

    ((QTableView *)p_unit->ptr)->setModel(new TableModel(manager));
    return 0;
}