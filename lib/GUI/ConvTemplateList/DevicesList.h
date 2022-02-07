//
// Created by SVK on 07.02.2022.
//

#ifndef EXRT_DEVICESLIST_H
#define EXRT_DEVICESLIST_H

#include <QAbstractTableModel>
#include <QTreeView>

#include "common/BaseClass_ifs.h"

class DataSchema_ifs;
class ExtensionManager;
class DeviceManager;
class BaseSignalController;

class TreeEditor;

class DevicesListModel : public QAbstractItemModel {
    Q_OBJECT

   public:
    explicit DevicesListModel(ExtensionManager *manager);
    explicit DevicesListModel(const QString &data, QObject *parent = nullptr);
    ~DevicesListModel() override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

   private:
    std::vector<DataSchema_ifs *> list_of_entries_;
    DataSchema_ifs *schema_ = nullptr;
    DeviceManager *device_manager_ = nullptr;
};

class DevicesList {};

#endif  // EXRT_DEVICESLIST_H
