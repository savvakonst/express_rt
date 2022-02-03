
#include <QHeaderView>
#include <QTableView>
//
#include "ConvTemplateList.h"
#include "GUI/WidgetWrappers.h"
#include "QDebug"
#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "convtemplate/Parameter_ifs.h"

ConvTemplateTreeModel::ConvTemplateTreeModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "conversion_template");
    if (unit && unit->ptr) {
        schema_ = (DataSchema_ifs *)unit->ptr;
        schema_->init(manager);
        list_of_entries_ = schema_->getMapList();
    }

    unit = manager->getLastVersionExtensionUint("cnv_template_manager", "cnv_template_manager");
    if (unit && unit->ptr) {
        manager_ = (ConversionTemplateManager *)unit->ptr;
        std::cout << "manager_ = (ConversionTemplateManager *)unit->ptr;\n";
    }
}

// TreeModel::TreeModel(const QString &data, QObject *parent) {}
ConvTemplateTreeModel::~ConvTemplateTreeModel() = default;

QVariant ConvTemplateTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();

    if (role != Qt::DisplayRole) return QVariant();

    auto ptr = (ConversionTemplate *)index.internalPointer();
    if (ptr != nullptr) {
        auto &prm_map = ptr->getAllParameters();

        auto it = prm_map.begin();
        std::advance(it, index.row());
        return QVariant(it->first.data());
    }

    auto conv_template = manager_->getConversionTemplateByIndex(index.row());
    auto name = list_of_entries_[index.column()]->name_;
    auto data = conv_template->getInfo(name);
    return {data->getValue().asString().data()};
}

Qt::ItemFlags ConvTemplateTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index);
}

QVariant ConvTemplateTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        return QString(list_of_entries_[section]->description_.data());
    }
    return {};
}

QModelIndex ConvTemplateTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) return QModelIndex();

    if (!parent.isValid()) {
        auto conv_tmp = manager_->getConversionTemplateByIndex((size_t)row);
        if (conv_tmp) return createIndex(row, column, nullptr);
    } else {
        auto conv_tmp = manager_->getConversionTemplateByIndex((size_t)parent.row());
        if (row < conv_tmp->getAllParameters().size()) return createIndex(row, column, conv_tmp);
    }

    return QModelIndex();
}

QModelIndex ConvTemplateTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return QModelIndex();

    auto ptr = (ConversionTemplate *)index.internalPointer();
    if (ptr) return createIndex(manager_->getIndex(ptr), 0, nullptr);

    return QModelIndex();
}

int ConvTemplateTreeModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) return 0;

    if (!parent.isValid()) return (int)manager_->getEntriesNumber();

    if (!parent.internalPointer()) {
        auto p = manager_->getConversionTemplateByIndex(parent.row());

        int count = (int)p->getAllParameters().size();
        return count;
    }
    return 0;
}

int ConvTemplateTreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 1;
    return (int)list_of_entries_.size();
}

/*
 *
 *
 *
 *
 */

ConvTempateTableModel::ConvTempateTableModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "conversion_template");
    if (unit && unit->ptr) {
        schema_ = (DataSchema_ifs *)unit->ptr;
        schema_->init(manager);
        list_of_entries_ = schema_->getMapList();
    }

    unit = manager->getLastVersionExtensionUint("cnv_template_manager", "cnv_template_manager");
    if (unit && unit->ptr) {
        manager_ = (ConversionTemplateManager *)unit->ptr;
        if (manager_ == nullptr) std::cerr << "ConvTempateTableModel::manager_ == nullptr;\n";
    }
}

int ConvTempateTableModel::rowCount(const QModelIndex &parent) const { return (int)manager_->getEntriesNumber(); }

int ConvTempateTableModel::columnCount(const QModelIndex &parent) const { return (int)list_of_entries_.size(); }

QVariant ConvTempateTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        return QString(list_of_entries_[section]->description_.data());
    }
    return {};
}

QVariant ConvTempateTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        auto conv_template = manager_->getConversionTemplateByIndex(index.row());
        auto name = list_of_entries_[index.column()]->name_;
        auto data = conv_template->getInfo(name);
        return {data->getValue().asString().data()};
    }
    return {};
}

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

    unit = manager->getLastVersionExtensionUint("widget", "conv_template_list");
    if (unit && unit->ptr) {
        a_item_view_ = (QAbstractItemView *)unit->ptr;
        if (a_item_view_ == nullptr) std::cerr << "QAbstractItemView::manager_ == nullptr;\n";
    } else {
        std::cerr << "can't find conv_template_list;\n";
    }

    unit = manager->getLastVersionExtensionUint("cnv_template_manager", "cnv_template_manager");
    if (unit && unit->ptr) {
        manager_ = (ConversionTemplateManager *)unit->ptr;
        if (manager_ == nullptr) std::cerr << "ConvTempateTableModel::manager_ == nullptr;\n";
    } else {
        std::cerr << "can't find cnv_template_manager;\n";
    }

    QObject::connect(a_item_view_, &QAbstractItemView::activated, this, &ParameterTableModel::receiveRow);
    // QObject::connect(a_item_view_, &QAbstractItemView::pressed, this, &ParameterTableModel::receiveRow);
    // QObject::connect(a_item_view_, &QAbstractItemView::clicked, this, &ParameterTableModel::receiveRow);
    // QObject::connect(a_item_view_, &QAbstractItemView::entered, this, &ParameterTableModel::receiveRow);
    //  void pressed(const QModelIndex &index);
    //  void clicked(const QModelIndex &index);
    //  void doubleClicked(const QModelIndex &index);

    // void activated(const QModelIndex &index);
    // void entered(const QModelIndex &index);
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
        auto conv_template = getCurrentConversionTemplate();

        auto &prm_map = conv_template->getAllParameters();
        auto it = prm_map.begin();
        std::advance(it, index.row());

        auto name = list_of_entries_[index.column()]->name_;
        auto h_data = it->second->getProperty("common/" + name);

        return QVariant(h_data->getValue().asString().data());
    }
    return {};
}

ConversionTemplate *ParameterTableModel::getCurrentConversionTemplate() const {
    auto index = a_item_view_->currentIndex();

    if (!index.isValid()) return nullptr;

    while (index.parent().isValid()) index = index.parent();

    return manager_->getConversionTemplateByIndex((size_t)index.row());
}

void ParameterTableModel::receiveRow(const QModelIndex &index) {
    layoutChanged();
    // emit dataChanged(QModelIndex(), QModelIndex());
}

/*
 *
 *
 *
 */

#ifndef CONV_TEMPLATE_LIST_LIB_NAME
#    error "CONV_TEMPLATE_LIST_LIB_NAME undefined"
#endif

QWidget *newTreeView(QWidget *parent) {
    auto table_view = new QTreeView();
    /*
        QHeaderView *vertical_header = table_view->verticalHeader();

        vertical_header->setDefaultSectionSize(vertical_header->minimumSectionSize());  // 20 px height
        vertical_header->sectionResizeMode(QHeaderView::Fixed);
        vertical_header->hide();
    */
    table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_view->setAlternatingRowColors(true);

    // table_view->setStyleSheet(
    //     "QHeaderView::section {background-color: #D2DCDF; alternate-background-color: #f6fafb;};");

    table_view->setAutoFillBackground(true);
    table_view->setBackgroundRole(QPalette().Base);
    auto p = table_view->palette();
    p.setColor(table_view->backgroundRole(), QColor("#D2DCDF"));
    table_view->setPalette(p);

    table_view->setStyleSheet(
        "QHeaderView::section {background-color: #D2DCDF; alternate-background-color: #f6fafb;};");
    return table_view;
}

static int initConvTemplateListWidget(ExtensionManager *manager);
static ExtensionUnit *g_conv_template_list_extension_uint = nullptr;
static ExtensionInfo g_conv_template_list_extension_info;

#include <qfile.h>

#include <QApplication>
InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, CONV_TEMPLATE_LIST_LIB_NAME)(void) {
    if (QCoreApplication::instance() == nullptr) return nullptr;

    g_conv_template_list_extension_uint = new ExtensionUnit[]{
        {"conv_template_list", "widget",
         "returns widget instance, which provides list of available conversion templates", (newTreeView(nullptr)),
         0x00},
        {"parameter_list", "widget", "returns widget instance, which provides list of available parameters",
         (newTreeView(nullptr)), 0x00},
        {"conv_template_list", "init", "checks the version and, if it's the latest, initializes the widget.",
         (void *)&initConvTemplateListWidget, 0x00},
        {nullptr, nullptr, nullptr, nullptr, 0}};

    g_conv_template_list_extension_info = {"conv_template_list_extension", 0x01, g_conv_template_list_extension_uint};

    return &g_conv_template_list_extension_info;
}

static int initConvTemplateListWidget(ExtensionManager *manager) {
    auto p_unit = search(g_conv_template_list_extension_uint, "widget", "conv_template_list");

    if (p_unit != manager->getLastVersionExtensionUint("widget", "conv_template_list")) {
        DEBUG_CERR("cant init (name: " << p_unit->name << ", type: " << p_unit->type << ", ver.:" << p_unit->version
                                       << ") unit, since there is a newer unit.\n");
    } else
        ((QAbstractItemView *)p_unit->ptr)->setModel(new ConvTempateTableModel(manager));

    /*
     *
     *
     */

    p_unit = search(g_conv_template_list_extension_uint, "widget", "parameter_list");

    if (p_unit != manager->getLastVersionExtensionUint("widget", "parameter_list")) {
        DEBUG_CERR("cant init (name: " << p_unit->name << ", type: " << p_unit->type << ", ver.:" << p_unit->version
                                       << ") unit, since there is a newer unit.\n");

    } else
        ((QAbstractItemView *)p_unit->ptr)->setModel(new ParameterTableModel(manager));

    return 0;
}