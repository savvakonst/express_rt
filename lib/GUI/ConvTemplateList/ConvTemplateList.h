//
// Created by SVK on 27.01.2022.
//

#ifndef EXRT_CONVTEMPLATELIST_H
#define EXRT_CONVTEMPLATELIST_H
#include <QAbstractTableModel>

class ExtensionManager;
class ConversionTemplateManager;
class DataSchema_ifs;

class TreeModel : public QAbstractItemModel {
    Q_OBJECT

   public:
    explicit TreeModel(ExtensionManager *manager);
    explicit TreeModel(const QString &data, QObject *parent = nullptr);
    ~TreeModel() override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role /*= Qt::DisplayRole*/) const override;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent /*= QModelIndex()*/) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent /*= QModelIndex()*/) const override;

   private:
    std::vector<DataSchema_ifs *> list_of_entries_;
    DataSchema_ifs *schema_ = nullptr;
    ConversionTemplateManager *manager_ = nullptr;
};

class TableModel : public QAbstractTableModel {
   public:
    explicit TableModel(ExtensionManager *manager);

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
