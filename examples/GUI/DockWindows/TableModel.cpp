
#include "TableModel.h"

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