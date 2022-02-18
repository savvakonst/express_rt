#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QTableView>
//
#include "GUI/WidgetWrappers.h"
#include "common/ExtensionManager.h"
#include "common/IO_ifs.h"
#include "convtemplate/Parameter_ifs.h"
#include "device/DeviceManager.h"
//
#include "DeviceList.h"

DeviceListModel::DeviceListModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUnit("device_manager", "device_manager");
    if (unit && unit->ptr) {
        device_manager_ = (DeviceManager *)unit->ptr;
    }

    class Delegate : public Signal_ifs {
       public:
        explicit Delegate(DeviceListModel *parent) : parent_(parent) {}
        void emit_() override {
            parent_->buildTree();
            parent_->layoutChanged();
        }

       private:
        DeviceListModel *parent_;
    };

    device_manager_->addSignal(new Delegate(this));

    buildTree();
}

DeviceListModel::~DeviceListModel() = default;

QVariant DeviceListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};

    if (role == Qt::DecorationRole) {
        QPixmap pixmap(16, 16);
        pixmap.fill(QColor(255, 0, 0));
        return pixmap;
    }

    if (role == Qt::BackgroundRole) {
        if (index == current_index_) return QBrush(QColor(255 * index.row(), 255 * index.column(), 0));
    }

    if (role != Qt::DisplayRole) return {};

    auto node = (TreeNode *)index.internalPointer();

    auto id = node->path_chunk;  //->object->getID();
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
    return {};
}

QModelIndex DeviceListModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) return {};

    auto node = parent.isValid() ? (TreeNode *)parent.internalPointer() : root_;

    if (node->child_vector.size() > size_t(row)) return createIndex(row, column, node->child_vector[row]);

    return {};
}

QModelIndex DeviceListModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return {};

    auto parent_node = ((TreeNode *)index.internalPointer())->parent;

    if (root_ == parent_node) return {};

    return createIndex(int(parent_node->getIndex()), 0, parent_node);
}

int DeviceListModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) return 0;

    auto node = parent.isValid() ? (TreeNode *)parent.internalPointer() : root_;
    if (node) return int(node->child_vector.size());

    return 0;
}

int DeviceListModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 1;
    return (int)1;
}

void DeviceListModel::buildTree() {
    delete root_;
    root_ = nullptr;

    auto list = device_manager_->getConversionTemplateList();
    if (list.empty()) return;
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

class DeviceRemoveAction : public QAction {
   public:
    explicit DeviceRemoveAction(ExtensionManager *manager, QObject *parent = nullptr) : QAction(parent) {
        setText(QObject::tr("&remove"));
        connect(this, &QAction::triggered, this, &DeviceRemoveAction::removeEntry);
        auto view_wrapper =
            (WidgetWrapper_ifs *)manager->getLastVersionExtensionObject("widget_wrapper", "device_view_wrapper");

        view_ = (QAbstractItemView *)view_wrapper->getWidget();
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
class DeviceViewWrapper;

class DeviceViewWrapper : public DeviceViewWrapper_ifs {
   public:
    class DeviceViewWrapperDelegate : public QObject {
       public:
        explicit DeviceViewWrapperDelegate(DeviceViewWrapper *wrapper) : wrapper_(wrapper) {}

        void init() {
            auto s_model = wrapper_->widget_->selectionModel();
            connect(s_model, &QItemSelectionModel::currentChanged, this,
                    &DeviceViewWrapperDelegate::currentChangedSlot);
        }

       public slots:

        void selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected) {}

        void currentChangedSlot(const QModelIndex &current, const QModelIndex &previous) {
            wrapper_->device_list_model_->current_index_ = current;
            // wrapper_->current_index_;
            qDebug() << "void currentChangedSlot(const QModelIndex &current, const QModelIndex &previous)";
        }

       private:
        DeviceViewWrapper *wrapper_ = nullptr;
    };

   public:
    friend class DeviceViewWrapperDelegate;
    DeviceViewWrapper() : widget_(new QTreeView()), delegate_(this) {}

    void init(ExtensionManager *manager) {
        device_list_model_ = new DeviceListModel(manager);
        widget_->setModel(device_list_model_);
        delegate_.init();

        auto io_units = manager->getLastVersionExtensionUnitsByType("io");
        for (auto i : io_units) {
            if (i && i->ptr && (((IO_ifs *)i->ptr)->filename_pattern_ == "*.ksd")) {
                auto new_base = new OpenAction(manager, (IO_ifs *)i->ptr, widget_);
                new_base->setStatusTip(QObject::tr("&Create a new file"));
                new_base->setShortcutContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
                new_base->setShortcut(Qt::CTRL + Qt::Key_O);

                widget_->addAction(new_base);
            }
        }

        auto remove_action = new DeviceRemoveAction(manager, widget_);
        remove_action->setShortcutContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
        remove_action->setShortcut(Qt::Key_Delete);

        widget_->addAction(remove_action);
    }

    ~DeviceViewWrapper() override { delete widget_; }

    status addSignal(Signal_ifs *signal) override { return status::failure; }

    QWidget *getWidget() override { return widget_; }

    QModelIndex searchForDevice(const std::string &source, const QModelIndex &index) {
        const int cnt = widget_->model()->rowCount(index);

        for (int i = 0; i < cnt; i++) {
            auto next_index = widget_->model()->index(i, 0, index);
            auto node = (DeviceListModel::TreeNode *)next_index.internalPointer();

            if (node->object == nullptr) {
                auto ret_index = searchForDevice(source, next_index);
                if (ret_index.isValid()) return next_index;

            } else {
                auto device = (Device *)node->object;
                if (device && (device->getSource() == source)) return next_index;
            }
        }
        return {};
    }

    bool setActive(const std::string &source, const std::string &path) override {
        auto device_index = searchForDevice(source, QModelIndex());
        if (device_index.isValid()) {
            widget_->selectionModel()->setCurrentIndex(device_index, QItemSelectionModel ::NoUpdate);
            return true;
        }
        return false;
    }

    bool setActive(size_t row_index) override {
        auto model = widget_->model();
        if (row_index < size_t(model->rowCount())) {
            auto top_index = model->index(int(row_index), 0);
            widget_->selectionModel()->setCurrentIndex(top_index, QItemSelectionModel ::NoUpdate);
            return true;
        }
        return false;
    }

    bool removeFromActive() override {
        widget_->setCurrentIndex(QModelIndex());
        return true;
    }

    bool addToSelected(size_t row_index) override { return false; }

    bool removeFromSelected(size_t row_index) override { return false; };

    Device *getActiveDevice() override {
        auto index = widget_->selectionModel()->currentIndex();
        if (!index.isValid()) return nullptr;
        auto node = (DeviceListModel::TreeNode *)index.internalPointer();

        while (node->parent->object) node = node->parent;

        return (Device *)node->object;
    };

    Module_ifs *getActiveModule() override {
        auto index = widget_->selectionModel()->currentIndex();
        if (!index.isValid()) return nullptr;
        return ((DeviceListModel::TreeNode *)index.internalPointer())->object;
    };

    std::vector<Module_ifs *> getSelected() override {
        std::vector<Module_ifs *> vector;
        auto indexes = widget_->selectionModel()->selectedIndexes();
        for (auto &i : indexes) {
            auto node = (DeviceListModel::TreeNode *)i.internalPointer();
            if (node->parent->parent == nullptr) vector.push_back(node->object);
        }
        return vector;
    };

   protected:
    DeviceViewWrapperDelegate delegate_;
    QTreeView *widget_;
    DeviceListModel *device_list_model_ = nullptr;
};

WidgetWrapper_ifs *newDeviceViewWrapper() {
    auto wrapper = new DeviceViewWrapper();
    auto table_view = (QAbstractItemView *)wrapper->getWidget();

    table_view->setAlternatingRowColors(true);
    table_view->setStyleSheet(
        "QTreeView {background-color: #D2DCDF; alternate-background-color: #f6fafb; show-decoration-selected: 1;}"
        "QHeaderView::section {background-color: #D2DCDF}"
        //"QTreeView::item:selected{background-color: #bedcf0;} "
        //"QTreeView::item:hover:selected{background-color: #94c8ea;} "
        //"QTreeView::item:hover:!selected{background-color: #e6e6e6;}"
    );
    table_view->setContextMenuPolicy(Qt::ActionsContextMenu);
    table_view->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

    return wrapper;
}

int initDeviceView(ExtensionManager *manager) {
    auto view_wrapper =
        (DeviceViewWrapper *)manager->getLastVersionExtensionObject("widget_wrapper", "device_view_wrapper");

    view_wrapper->init(manager);
    return 0;
}