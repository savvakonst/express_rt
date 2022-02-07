

#include <QHeaderView>
#include <QTableView>

#include "QDebug"
//
#include "GUI/TreeEditor.h"
#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "convtemplate/Parameter_ifs.h"
//
#include "ParameterTable.h"

/*
 *
 *
 *
 *
 */

ParameterTableModel::ParameterTableModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "common");
    if (unit && unit->ptr) {
        schema_ = (DataSchema_ifs *)unit->ptr;
        schema_->init(manager);
        list_of_entries_ = schema_->getMapList();
    } else {
        std::cerr << "can't find data_schema;\n";
    }

    parent_view_ = (QAbstractItemView *)manager->getLastVersionExtensionObject("widget", "conv_template_list");
    if (parent_view_ == nullptr) std::cerr << "can't find conv_template_list;\n";

    cnv_manager_ = (ConversionTemplateManager *)manager->getLastVersionExtensionObject("cnv_template_manager",
                                                                                       "cnv_template_manager");
    if (cnv_manager_ == nullptr) std::cerr << "can't find cnv_template_manager;\n";

    child_view_ = (TreeEditor *)manager->getLastVersionExtensionObject("tree_editor", "tree_editor");
    if (child_view_ == nullptr) std::cerr << "can't find cnv_template_manager;\n";

    connect(parent_view_, &QAbstractItemView::activated, this, &ParameterTableModel::receiveRow);
}

int ParameterTableModel::rowCount(const QModelIndex &parent) const {
    auto conv_temp = getCurrentConversionTemplate();

    auto ret = conv_temp ? (int)conv_temp->getAllParameters().size() : 0;
    return ret;
}

int ParameterTableModel::columnCount(const QModelIndex &parent) const {
    //
    return (int)list_of_entries_.size();
}

QVariant ParameterTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((role == Qt::DisplayRole)) {
        return QString(list_of_entries_[section]->description_.data());
    }
    return {};
}

QVariant ParameterTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        auto prm = getParameter(index);
        auto name = list_of_entries_[index.column()]->name_;
        auto h_data = prm->getProperty("common/" + name);
        return QVariant(h_data->getValue().asString().data());
    }
    return {};
}

Parameter_ifs *ParameterTableModel::getParameter(const QModelIndex &index) const {
    if (!index.isValid()) return nullptr;

    auto conv_template = getCurrentConversionTemplate();

    auto &prm_map = conv_template->getAllParameters();
    auto it = prm_map.begin();
    std::advance(it, index.row());

    return it->second;
}

ConversionTemplate *ParameterTableModel::getCurrentConversionTemplate() const {
    auto index = parent_view_->currentIndex();

    if (!index.isValid()) return nullptr;

    while (index.parent().isValid()) index = index.parent();

    return cnv_manager_->getConversionTemplateByIndex((size_t)index.row());
}

void ParameterTableModel::receiveRow(const QModelIndex &index) { layoutChanged(); }

void ParameterTableModel::selectParameter(const QModelIndex &index) {
    auto prm = getParameter(index);

    if (prm) child_view_->setupProperties(prm);
}

/*
 *
 */

void ParameterTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    emit currentChangedSignal(current);
    // static int cnt = 0;
    // qDebug() << "ParameterTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous): " <<
    // cnt++;
}

QWidget *newParameterTreeView(QWidget *parent) {
    auto prm_view = new ParameterTreeView();
    prm_view->setAlternatingRowColors(true);

    prm_view->setStyleSheet(
        "QTreeView {background-color: #D2DCDF; alternate-background-color: #f6fafb; show-decoration-selected: 1;}"
        "QHeaderView::section {background-color: #D2DCDF}");

    return prm_view;
}

int initParameterTreeView(ExtensionManager *manager) {
    auto tree_view = (ParameterTreeView *)manager->getLastVersionExtensionObject("widget", "parameter_list");

    auto model = new ParameterTableModel(manager);
    QObject::connect(tree_view, &ParameterTreeView::currentChangedSignal, model, &ParameterTableModel::selectParameter);
    tree_view->setModel(model);
    return 0;
}
