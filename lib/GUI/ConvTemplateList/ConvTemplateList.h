//
// Created by SVK on 27.01.2022.
//

#ifndef EXRT_CONVTEMPLATELIST_H
#define EXRT_CONVTEMPLATELIST_H
#include <QAbstractTableModel>
#include <QTreeView>

#include "common/BaseClass_ifs.h"

class DataSchema_ifs;

class ExtensionManager;
class ConversionTemplateManager;
class BaseSignalController;

class TreeView : public QTreeView {
    Q_OBJECT
   public:
    explicit TreeView(QWidget *parent = nullptr) : QTreeView(parent) {}
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                     const QVector<int> &roles = QVector<int>()) override {
        QTreeView::dataChanged(topLeft, bottomRight, roles);
        repaint();
    }
    // repaint
    BaseSignalController signal_controller_;

   private:
};

/*
 *
 *
 */

class ConvTemplateTreeModel : public QAbstractItemModel {
    Q_OBJECT

   public:
    explicit ConvTemplateTreeModel(ExtensionManager *manager);
    explicit ConvTemplateTreeModel(const QString &data, QObject *parent = nullptr);
    ~ConvTemplateTreeModel() override;

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
    Q_OBJECT
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
class ConversionTemplate;
class QAbstractItemView;

class ParameterTableModel : public QAbstractTableModel {
    Q_OBJECT
   public:
    explicit ParameterTableModel(ExtensionManager *manager);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

   public slots:
    void receiveRow(const QModelIndex &index);

   protected:
    ConversionTemplate *getCurrentConversionTemplate() const;

    std::vector<DataSchema_ifs *> list_of_entries_;
    DataSchema_ifs *schema_ = nullptr;
    QAbstractItemView *a_item_view_ = nullptr;
    ConversionTemplateManager *manager_ = nullptr;
};

#endif  // EXRT_TABLEMODEL_H
