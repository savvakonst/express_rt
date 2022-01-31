
#include <QHeaderView>
#include <QTableView>
//
#include "ConvTemplateList.h"
#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/ConversionTemplateManager.h"

TableModel::TableModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "conversion_template");
    if (unit && unit->ptr) {
        schema_ = (DataSchema_ifs *)unit->ptr;
        schema_->init(manager);
        list_of_entries_ = schema_->getMapList();
    }

    unit = manager->getLastVersionExtensionUint("cnv_template_manager", "cnv_template_manager");
    if (unit && unit->ptr) manager_ = (ConversionTemplateManager *)unit->ptr;
}

int TableModel::rowCount(const QModelIndex &parent) const { return manager_->getEntriesNumber(); }

int TableModel::columnCount(const QModelIndex &parent) const { return list_of_entries_.size(); }

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        return QString(list_of_entries_[section]->description_.data());
    }
    return QVariant();
}

QVariant TableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        auto conv_template = manager_->getConversionTemplateByIndex(index.row());
        auto name = list_of_entries_[index.column()]->name_;
        auto data = conv_template->getInfo(name);
        //=data->getValue().asString();
        // QString answer =
        //      QString("row = ") + QString::number(index.row()) + "  col = " + QString::number(index.column());

        return QVariant(data->getValue().asString().data());
    }
    return QVariant();
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
    auto table_view = new QTableView(parent);

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

static void *g_conv_template_list_singleton = (void *)newConvTemplateList(nullptr);

int initConvTemplateList(ExtensionManager *manager) {
    ((QTableView *)g_conv_template_list_singleton)->setModel(new TableModel(manager));
    return 0;
}

static ExtensionUnit g_conv_template_list_extension_uint[] = {
    {"conv_template_list", "widget", "return table with available conversion templates",
     (void *)g_conv_template_list_singleton, 0x00},
    {"conv_template_list", "widget", "return table with available conversion templates",
     (void *)g_conv_template_list_singleton, 0x00},
    {nullptr, nullptr, nullptr, nullptr, 0}};

static ExtensionInfo g_conv_template_list_extension_info = {"conv_template_list_extension", 0x01,
                                                            g_conv_template_list_extension_uint};

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, CONV_TEMPLATE_LIST_LIB_NAME)(void) {
    return &g_conv_template_list_extension_info;
}