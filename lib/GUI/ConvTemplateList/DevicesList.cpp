#include <QDebug>
#include <QHeaderView>
#include <QTableView>
//
#include "common/ExtensionManager.h"
#include "convtemplate/Parameter_ifs.h"
#include "device/DeviceManager.h"
//
#include "DevicesList.h"

DevicesListModel::DevicesListModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUint("device_manager", "device_manager");
    if (unit && unit->ptr) {
        device_manager_ = (DeviceManager *)unit->ptr;
    }
    buildTree();
}

DevicesListModel::~DevicesListModel() = default;

QVariant DevicesListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    if (role != Qt::DisplayRole) return QVariant();

    auto node = (TreeNode *)index.internalPointer();

    auto id = node->object->getID();
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

    auto node = parent.isValid() ? (TreeNode *)parent.internalPointer() : root_;

    if (node->child_vector.size() > size_t(row)) return createIndex(row, column, node->child_vector[row]);

    return QModelIndex();
}

QModelIndex DevicesListModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return QModelIndex();

    auto parent_node = ((TreeNode *)index.internalPointer())->parent;

    if (root_ == parent_node) return QModelIndex();

    return createIndex(int(parent_node->getIndex()), 0, parent_node);
}

int DevicesListModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) return 0;

    auto node = parent.isValid() ? (TreeNode *)parent.internalPointer() : root_;
    if (node) return node->child_vector.size();

    return 0;
}

int DevicesListModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 1;
    return (int)1;
}

void DevicesListModel::buildTree() {
    if (root_) delete root_;

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
