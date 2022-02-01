//
// Created by SVK on 27.01.2022.
//

#ifndef EXRT_CONVTEMPLATELIST_H
#define EXRT_CONVTEMPLATELIST_H
#include <QAbstractTableModel>

class ExtensionManager;
class ConversionTemplateManager;
class DataSchema_ifs;

class ConvTempateTreeModel : public QAbstractItemModel {
    Q_OBJECT

   public:
    explicit ConvTempateTreeModel(ExtensionManager *manager);
    explicit ConvTempateTreeModel(const QString &data, QObject *parent = nullptr);
    ~ConvTempateTreeModel() override;

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
    ConversionTemplateManager *manager_ = nullptr;
};

/*
 *
 *
 */

class ConvTempateTableModel : public QAbstractTableModel {
   public:
    explicit ConvTempateTableModel(ExtensionManager *manager);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

   protected:
    std::vector<DataSchema_ifs *> list_of_entries_;
    DataSchema_ifs *schema_ = nullptr;
    ConversionTemplateManager *manager_ = nullptr;
};

/*
 *
 *
 */

class ParameterTableModel : public QAbstractTableModel {
   public:
    explicit ParameterTableModel(ExtensionManager *manager);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

   protected:
    std::vector<DataSchema_ifs *> list_of_entries_;
    DataSchema_ifs *schema_ = nullptr;
    ConversionTemplateManager *manager_ = nullptr;
};

#endif  // EXRT_TABLEMODEL_H
