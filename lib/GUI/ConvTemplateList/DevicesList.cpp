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
    auto unit = manager->getLastVersionExtensionUint("device_manager", "device_manager");
    if (unit && unit->ptr) {
        device_manager_ = (DeviceManager *)unit->ptr;
        std::cout << "cnv_manager_ = (ConversionTemplateManager *)unit->ptr;\n";
    }
    buildTree();
}

DevicesListModel::~DevicesListModel() = default;

QVariant DevicesListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    if (role != Qt::DisplayRole) return QVariant();

    auto ptr = (Module_ifs *)index.internalPointer();

    if (ptr != nullptr) {
        // TODO: it is too slow, "Module_ifs" need another method
        auto modules = ptr->getSubModules();
        ptr = modules[size_t(index.row())].second;
    } else
        ptr = device_manager_->getConversionTemplateByIndex(index.row());

    auto id = ptr->getID();
    return id.c_str();
}

Qt::ItemFlags DevicesListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index);
}

QVariant DevicesListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && (section == 0)) {
        return QString(tr("id"));
    }
    return QVariant();
}

QModelIndex DevicesListModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) return QModelIndex();

    if (!parent.isValid()) {
        auto module = device_manager_->getConversionTemplateByIndex((size_t)row);
        if (module) return createIndex(row, column, nullptr);
    } else {
        auto module = (Module_ifs *)parent.internalPointer();
        // TODO: this is also slow, "Module_ifs" needs another method, this problem has already been mentioned above.
        module = module->getSubModules()[row].second;
        return createIndex(row, column, module);
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
    return (int)1;
}

void DevicesListModel::buildTree() {
    if (root_)
        delete root_;

    auto list = device_manager_->getConversionTemplateList();
    root_ = new TreeNode(nullptr);
    root_->child_vector.reserve(list.size());
    for (auto i : list) root_->addNodesRecursively(i);
}

/*
 *
 *
 *
 *
 */
