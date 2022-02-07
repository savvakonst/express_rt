#include <QDebug>
#include <QHeaderView>
#include <QTableView>
//
#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "convtemplate/Parameter_ifs.h"
#include "device/DeviceManager.h"
//
#include "DevicesList.h"

DevicesListModel::DevicesListModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "conversion_template");
    if (unit && unit->ptr) {
        schema_ = (DataSchema_ifs *)unit->ptr;
        schema_->init(manager);
        list_of_entries_ = schema_->getMapList();
    }

    unit = manager->getLastVersionExtensionUint("device_manager", "device_manager");
    if (unit && unit->ptr) {
        device_manager_ = (DeviceManager *)unit->ptr;
        std::cout << "cnv_manager_ = (ConversionTemplateManager *)unit->ptr;\n";
    }
}

DevicesListModel::~DevicesListModel() = default;

QVariant DevicesListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();

    if (role != Qt::DisplayRole) return QVariant();

    auto ptr = (ConversionTemplate *)index.internalPointer();
    if (ptr != nullptr) {
        auto &prm_map = ptr->getAllParameters();

        auto it = prm_map.begin();
        std::advance(it, index.row());
        return QVariant(it->first.data());
    }

    auto conv_template = device_manager_->getConversionTemplateByIndex(index.row());
    auto name = list_of_entries_[index.column()]->name_;
    auto data = conv_template->getInfo(name);
    return {data->getValue().asString().data()};
}

Qt::ItemFlags DevicesListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index);
}

QVariant DevicesListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        return QString(list_of_entries_[section]->description_.data());
    }
    return {};
}

QModelIndex DevicesListModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) return QModelIndex();

    if (!parent.isValid()) {
        auto conv_tmp = device_manager_->getConversionTemplateByIndex((size_t)row);
        if (conv_tmp) return createIndex(row, column, nullptr);
    } else {
        auto conv_tmp = device_manager_->getConversionTemplateByIndex((size_t)parent.row());
        if (row < conv_tmp->getAllParameters().size()) return createIndex(row, column, conv_tmp);
    }

    return QModelIndex();
}

QModelIndex DevicesListModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return QModelIndex();

    auto ptr = (ConversionTemplate *)index.internalPointer();
    if (ptr) return createIndex(device_manager_->getIndex(ptr), 0, nullptr);

    return QModelIndex();
}

int DevicesListModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) return 0;

    if (!parent.isValid()) return (int)device_manager_->getEntriesNumber();

    if (!parent.internalPointer()) {
        auto p = device_manager_->getConversionTemplateByIndex(parent.row());

        int count = (int)p->getAllParameters().size();
        return count;
    }
    return 0;
}

int DevicesListModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 1;
    return (int)list_of_entries_.size();
}

/*
 *
 *
 *
 *
 */