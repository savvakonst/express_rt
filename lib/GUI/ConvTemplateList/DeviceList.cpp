#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>
#include <QTableView>
//
#include "common/ExtensionManager.h"
#include "common/IO_ifs.h"
#include "convtemplate/Parameter_ifs.h"
#include "device/DeviceManager.h"
//
#include "DeviceList.h"

DeviceListModel::DeviceListModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUint("device_manager", "device_manager");
    if (unit && unit->ptr) {
        device_manager_ = (DeviceManager *)unit->ptr;
    }
    buildTree();
}

DeviceListModel::~DeviceListModel() = default;

QVariant DeviceListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    if (role != Qt::DisplayRole) return QVariant();

    auto node = (TreeNode *)index.internalPointer();

    auto id = node->object->getID();
    return id.c_str();
}

Qt::ItemFlags DeviceListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index);
}

QVariant DeviceListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && (section == 0)) {
        return QString(tr("id"));
    }
    return QVariant();
}

QModelIndex DeviceListModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) return QModelIndex();

    auto node = parent.isValid() ? (TreeNode *)parent.internalPointer() : root_;

    if (node->child_vector.size() > size_t(row)) return createIndex(row, column, node->child_vector[row]);

    return QModelIndex();
}

QModelIndex DeviceListModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return QModelIndex();

    auto parent_node = ((TreeNode *)index.internalPointer())->parent;

    if (root_ == parent_node) return QModelIndex();

    return createIndex(int(parent_node->getIndex()), 0, parent_node);
}

int DeviceListModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) return 0;

    auto node = parent.isValid() ? (TreeNode *)parent.internalPointer() : root_;
    if (node) return node->child_vector.size();

    return 0;
}

int DeviceListModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 1;
    return (int)1;
}

void DeviceListModel::buildTree() {
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
 */

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
        view_ = (QAbstractItemView *)manager->getLastVersionExtensionObject("widget", "device_list");
        device_manager_ = (DeviceManager *)manager->getLastVersionExtensionObject("device_manager", "device_manager");
    }
   protected slots:
    void removeEntry() {
        auto index = view_->selectionModel()->currentIndex().row();
        device_manager_->removeDeviceByIndex(index);
    }

   protected:
    QAbstractItemView *view_ = nullptr;
    DeviceManager *device_manager_ = nullptr;
};

/*
 *
 *
 *
 *
 */

QWidget *newDevicesView(QWidget *parent) {
    auto table_view = new QTreeView();

    table_view->setAlternatingRowColors(true);
    table_view->setStyleSheet(
        "QTreeView {background-color: #D2DCDF; alternate-background-color: #f6fafb; show-decoration-selected: 1;}"
        "QHeaderView::section {background-color: #D2DCDF}");
    table_view->setContextMenuPolicy(Qt::ActionsContextMenu);
    // table_view->setSelectionMode( QAbstractItemView::SelectionMode::ExtendedSelection );

    return table_view;
}

int initDevicesView(ExtensionManager *manager) {
    auto view = (QAbstractItemView *)manager->getLastVersionExtensionObject("widget", "device_list");

    view->setModel(new DeviceListModel(manager));

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