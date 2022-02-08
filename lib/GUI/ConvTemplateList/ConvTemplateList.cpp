#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>
#include <QMenu>
#include <QTableView>
//
#include "common/ExtensionManager.h"
#include "common/IO_ifs.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "convtemplate/Parameter_ifs.h"
//
#include "ConvTemplateList.h"

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
        std::cout << "cnv_manager_ = (ConversionTemplateManager *)unit->ptr;\n";
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

ConvTemplateTableModel::ConvTemplateTableModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "conversion_template");
    if (unit && unit->ptr) {
        schema_ = (DataSchema_ifs *)unit->ptr;
        schema_->init(manager);
        list_of_entries_ = schema_->getMapList();
    }

    unit = manager->getLastVersionExtensionUint("cnv_template_manager", "cnv_template_manager");
    if (unit && unit->ptr) {
        cnv_manager_ = (ConversionTemplateManager *)unit->ptr;
        if (cnv_manager_ == nullptr) std::cerr << "ConvTempateTableModel::cnv_manager_ == nullptr;\n";
    }

    class Delegate : public Signal_ifs {
       public:
        explicit Delegate(ConvTemplateTableModel *parent) : parent_(parent) {}
        void emit_() override { parent_->layoutChanged(); }

       private:
        ConvTemplateTableModel *parent_;
    };

    cnv_manager_->addSignal(new Delegate(this));
}

int ConvTemplateTableModel::rowCount(const QModelIndex &parent) const { return (int)cnv_manager_->getEntriesNumber(); }

int ConvTemplateTableModel::columnCount(const QModelIndex &parent) const { return (int)list_of_entries_.size(); }

QVariant ConvTemplateTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        return QString(list_of_entries_[section]->description_.data());
    }
    return {};
}

QVariant ConvTemplateTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        auto conv_template = cnv_manager_->getConversionTemplateByIndex(index.row());
        auto name = list_of_entries_[index.column()]->name_;
        auto data = conv_template->getInfo(name);
        return {data->getValue().asString().data()};
    }
    return {};
}

/*
bool ConvTemplateTableModel::removeRows(int row, int count, const QModelIndex &parent ) {
    qDebug()<<"bool ConvTemplateTableModel::removeRows(int row, int count, const QModelIndex &parent ) ";
    return false;
}
*/
/*
Qt::ItemFlags ConvTemplateTableModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);  // Qt::ItemIsEditable |
}
*/
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

    table_view->setAlternatingRowColors(true);
    table_view->setStyleSheet(
        "QTreeView {background-color: #D2DCDF; alternate-background-color: #f6fafb; show-decoration-selected: 1;}"
        "QHeaderView::section {background-color: #D2DCDF}");
    table_view->setContextMenuPolicy(Qt::ActionsContextMenu);
    // table_view->setSelectionMode( QAbstractItemView::SelectionMode::ExtendedSelection );

    return table_view;
}

class OpenAction : public QAction {
   public:
    explicit OpenAction(ExtensionManager *manager, IO_ifs *io, QObject *parent = nullptr)
        : QAction(parent), manager_(manager), io_(io) {
        setText(QObject::tr("&Open"));
        //+ " " + io->file_type_.c_str()
        connect(this, &QAction::triggered, this, &OpenAction::openFile);
    }

   protected slots:
    void openFile() {
        auto f_name =
            QFileDialog::getOpenFileName(nullptr, /*caption*/ QString(), /*dir */ QString(),
                                         /*filter*/ io_->filename_pattern_.c_str(), /*selectedFilter*/ nullptr,
                                         /*options*/ QFileDialog::Options());
        io_->readDocument(manager_, f_name.toStdString());
    }

   protected:
    ExtensionManager *manager_ = nullptr;
    IO_ifs *io_;
};

class RemoveAction : public QAction {
   public:
    explicit RemoveAction(ExtensionManager *manager, QObject *parent = nullptr) : QAction(parent) {
        setText(QObject::tr("&remove"));
        connect(this, &QAction::triggered, this, &RemoveAction::removeEntry);
        view_ = (QAbstractItemView *)manager->getLastVersionExtensionObject("widget", "conv_template_list");
        cnv_manager_ = (ConversionTemplateManager *)manager->getLastVersionExtensionObject("cnv_template_manager",
                                                                                           "cnv_template_manager");
    }
   protected slots:
    void removeEntry() {
        auto index = view_->selectionModel()->currentIndex().row();
        cnv_manager_->removeConversionTemplateByIndex(index);
    }

   protected:
    QAbstractItemView *view_ = nullptr;
    ConversionTemplateManager *cnv_manager_ = nullptr;
};

/*
 *
 *
 */

QWidget *newParameterTreeView(QWidget *parent);
int initParameterTreeView(ExtensionManager *manager);

/*
 *
 *
 */

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
         (newParameterTreeView(nullptr)), 0x00},
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
    } else {
        auto view = (QAbstractItemView *)p_unit->ptr;
        view->setModel(new ConvTemplateTableModel(manager));

        auto io_units = manager->getLastVersionExtensionUintsByType("io");
        for (auto i : io_units) {
            if (i && i->ptr) {
                auto new_base = new OpenAction(manager, (IO_ifs *)i->ptr, view);
                new_base->setStatusTip(QObject::tr("&Create a new file"));
                new_base->setShortcutContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
                new_base->setShortcut(Qt::CTRL + Qt::Key_O);

                view->addAction(new_base);
            }
        }

        auto remove_action = new RemoveAction(manager, view);
        remove_action->setShortcutContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
        remove_action->setShortcut(Qt::Key_Delete);

        view->addAction(remove_action);
    }

    /*
     *
     *
     */

    p_unit = search(g_conv_template_list_extension_uint, "widget", "parameter_list");

    if (p_unit != manager->getLastVersionExtensionUint("widget", "parameter_list")) {
        DEBUG_CERR("cant init (name: " << p_unit->name << ", type: " << p_unit->type << ", ver.:" << p_unit->version
                                       << ") unit, since there is a newer unit.\n");
    } else
        initParameterTreeView(manager);

    return 0;
}
