//
// Created by SVK on 06.02.2022.
//

#ifndef EXRT_PARAMETERTABLE_H
#define EXRT_PARAMETERTABLE_H

#include <QAbstractTableModel>
#include <QTreeView>

#include "common/BaseClass_ifs.h"

class Parameter_ifs;
class DataSchema_ifs;
class ExtensionManager;
class ConversionTemplate;
class ConversionTemplateManager;

class TreeEditor;

class QAbstractItemView;

class ParameterTableModel : public QAbstractItemModel /*QAbstractTableModel*/ {
    Q_OBJECT
   public:
    explicit ParameterTableModel(ExtensionManager *manager);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] QModelIndex getIndex(const std::string &name) const;

    //////////////

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QModelIndex parent(const QModelIndex &) const override;

    [[nodiscard]] QModelIndex sibling(int row, int column, const QModelIndex &) const override;

    [[nodiscard]] bool hasChildren(const QModelIndex &parent) const override;

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

   protected:
    [[nodiscard]] Parameter_ifs *getParameter(const QModelIndex &index) const;
    [[nodiscard]] ConversionTemplate *getCurrentConversionTemplate() const;

   public slots:
    void receiveRow(const QModelIndex &index);
    void selectParameter(const QModelIndex &index);

   protected:
    DataSchema_ifs *schema_ = nullptr;

    std::vector<DataSchema_ifs *> list_of_entries_;
    QAbstractItemView *parent_view_ = nullptr;
    ConversionTemplateManager *cnv_manager_ = nullptr;

    TreeEditor *child_view_ = nullptr;
};

/*
 *
 *
 */

class ParameterTreeView : public QTreeView {
    Q_OBJECT
   public:
    explicit ParameterTreeView(QWidget *parent = nullptr) : QTreeView(parent) {}

    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

   public:
   signals:
    void currentChangedSignal(const QModelIndex &current);

   private:
};

#endif  // EXRT_PARAMETERTABLE_H
