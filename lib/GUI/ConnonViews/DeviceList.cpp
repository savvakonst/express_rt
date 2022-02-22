#include <QAction>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QHeaderView>
#include <QTableView>
//
#include "GUI/WidgetWrappers.h"
#include "common/ExtensionManager.h"
#include "common/IO_ifs.h"
#include "common/StringProcessingTools.h"
#include "convtemplate/Parameter_ifs.h"
#include "device/DeviceManager.h"
//
#include "DeviceList.h"

DeviceListModel::DeviceListModel(ExtensionManager *manager) {
    auto image_dir = QDir(QCoreApplication::applicationDirPath() + "/png/modules");
    auto image_path_list = image_dir.entryInfoList({"*.png", "*.bmp", "*.ico"});

    for (auto &i : image_path_list) {
        icons_map_[i.baseName()] = QIcon(i.absoluteFilePath());
    }

    auto unit = manager->getLastVersionExtensionUnit("device_manager", "device_manager");
    if (unit && unit->ptr) {
        device_manager_ = (DeviceManager *)unit->ptr;
    }

    class Delegate : public Signal_ifs {
       public:
        explicit Delegate(DeviceListModel *parent) : parent_(parent) {}
        void emit_() override { parent_->buildTree(); }

       private:
        DeviceListModel *parent_;
    };

    device_manager_->addSignal(new Delegate(this));

    buildTree();
}

DeviceListModel::~DeviceListModel() = default;

QVariant DeviceListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};

    auto node = (TreeNode *)index.internalPointer();

    if (role == Qt::DecorationRole) {
        if (node == active_device_) return icons_map_["DEV_CHECK"];
        if (node->m_object) return icons_map_[node->module_type.c_str()];

        return {};
    }

    if (role != Qt::DisplayRole) return {};

    return node->path_chunk.c_str();
}

Qt::ItemFlags DeviceListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index);
}

QVariant DeviceListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && (section == 0)) return QString(tr("id"));

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

    if (parent_node == root_) return {};

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
    node_map_.clear();

    root_ = new TreeNode();
    endResetModel();

    auto list = device_manager_->getConversionTemplateList();
    if (list.empty()) {
        emit layoutChanged();
        return;
    }

    root_->child_vector.reserve(list.size());
    for (auto i : list) root_->addNodesRecursively(i, i->getSource());
    dataChanged(QModelIndex(), QModelIndex());

    for (auto i : root_->child_vector) addItemsToNodeMap(i);

    emit layoutChanged();
}

QList<QModelIndex> DeviceListModel::getTreeIndexList(const std::string &source, const std::string &path) {
    QList<QModelIndex> ret;
    auto full_path = source + "//" + path;
    for (auto &i : node_map_) {
        if (filePatternMatch(full_path, i.first)) ret.push_back(createIndex(i.second->getIndex(), 0, i.second));
    }
    return ret;
}

std::list<DeviceListModel::TreeNode *> DeviceListModel::getTreeNodeList(const std::string &source,
                                                                        const std::string &path) {
    std::list<DeviceListModel::TreeNode *> ret;
    auto full_path = source + "//" + path;
    for (auto &i : node_map_) {
        if (filePatternMatch(full_path, i.first)) ret.push_back(i.second);
    }
    return ret;
}

DeviceListModel::TreeNode *DeviceListModel::getTreeNode(const std::string &source, const std::string &path) {
    if (path.empty()) {
        TreeNode *node = nullptr;
        for (auto i : root_->child_vector)
            if (source == ((Device *)i->m_object)->getSource()) {
                node = i;
                break;
            }
        return node;
    }

    auto item = node_map_.find(source + "//" + path);
    if (item != node_map_.end()) return item->second;
    return nullptr;
}

bool DeviceListModel::setActiveDevice(const std::string &source) {
    auto i = getTreeNode(source, "");
    if (i) {
        TreeNode *old_active_device = active_device_;
        active_device_ = i;
        if (old_active_device) {
            auto active_index = createIndex(int(old_active_device->getIndex()), 0, old_active_device);
            dataChanged(active_index, active_index, {Qt::DecorationRole});
        }
        auto new_active_index = createIndex(int(active_device_->getIndex()), 0, active_device_);
        dataChanged(new_active_index, new_active_index, {Qt::DecorationRole});
        return true;
    }
    return false;
}

void DeviceListModel::addItemsToNodeMap(TreeNode *node) {
    auto ph = node->getPath();
    auto s = ph.first + "//" + ph.second;
    node_map_[s] = node;
    auto &vector = node->child_vector;
    for (TreeNode *i : vector) addItemsToNodeMap(i);
}

void DeviceListModel::TreeNode::addNodesRecursively(Module_ifs *ptr, const std::string &path) {
    auto node = new TreeNode(ptr);
    node->parent = this;

    node->self_index = child_vector.size();
    child_vector.push_back(node);

    node->path_chunk = (path.empty() ? "" : path + "/") + (ptr ? ptr->getID() : "");

    if (ptr == nullptr) return;
    {
        auto id = ptr->getID();
        size_t end_pos = 0;
        for (auto i : id)
            if (('A' <= i) && (i <= 'z') && ((i <= 'Z') || ('a' <= i))) end_pos++;
            else
                break;
        node->module_type = id.substr(0, end_pos);
    }

    auto list = ptr->getSubModules();
    for (const auto &i : list) {
        node->addNodesRecursively(i.second, i.first);
    }
}

std::pair<std::string, std::string> DeviceListModel::TreeNode::getPath() const {
    if (parent == nullptr) return {};
    if (isDevice()) return {((Device *)m_object)->getSource(), ""};

    std::string path = path_chunk;
    auto ptr = parent;
    while (!ptr->isDevice()) {
        path = ptr->path_chunk + "/" + path;
        ptr = ptr->parent;
    }
    auto dev = (Device *)ptr->m_object;
    return {dev->getSource(), dev->getID() + "/" + path};
}

/*
 *
 *
 *
 */

class DeviceOpenAction : public QAction {
   public:
    explicit DeviceOpenAction(ExtensionManager *manager, IO_ifs *io, QObject *parent = nullptr)
        : QAction(parent), manager_(manager), io_(io) {
        setText(QObject::tr("&Open"));
        //+ " " + io->file_type_.c_str()
        connect(this, &QAction::triggered, this, &DeviceOpenAction::openFile);
    }

   protected slots:
    void openFile() {
        auto f_name =
            QFileDialog::getOpenFileName(nullptr, /*caption*/ QString(), /*dir */ QString(),
                                         /*filter*/ io_->filename_pattern_.c_str(), /*selectedFilter*/ nullptr,
                                         /*options*/ QFileDialog::Options());

        auto dir = QDir(QDir::current());
        qDebug() << "//" << QDir::current();
        // dir.relativeFilePath(f_name)
        io_->readDocument(manager_, dir.relativeFilePath(f_name).toStdString());
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
 */

#include <functional>
#include <utility>

#include "common/ExrtAction_ifs.h"
class DeviceViewWrapper : public DeviceViewWrapper_ifs {
   private:
    class FuncProxyQAction : public QAction {
       public:
        using function_t = std::function<bool()>;
        FuncProxyQAction(function_t function, QObject *parent = nullptr)
            : QAction(parent), function_(std::move(function)) {
            connect(this, &FuncProxyQAction::triggered, this, &FuncProxyQAction::run);
        }
       public slots:
        void run() { function_(); };

       private:
        function_t function_;
    };

    class ProxyQAction : public QAction {
       public:
        ProxyQAction(ExrtAction_ifs *action, QObject *parent = nullptr) : QAction(parent), action_(action) {
            setText(action->getDescription().c_str());
            connect(this, &ProxyQAction::triggered, this, &ProxyQAction::run);
        }
       public slots:
        void run() { action_->run(); };
        ExrtAction_ifs *action_;
    };

   public:
    friend class DeviceViewWrapperDelegate;
    DeviceViewWrapper() : widget_(new QTreeView()) {
        widget_->setAlternatingRowColors(true);
        widget_->setStyleSheet(
            "QTreeView {background-color: #D2DCDF;  show-decoration-selected: 1;}"
            "QHeaderView::section {background-color: #D2DCDF}");
        widget_->setContextMenuPolicy(Qt::ActionsContextMenu);
        widget_->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
    }

    void init(ExtensionManager *manager) {
        device_list_model_ = new DeviceListModel(manager);
        widget_->setModel(device_list_model_);

        auto io_units = manager->getLastVersionExtensionUnitsByType("io");
        for (auto i : io_units) {
            if (i && i->ptr && (((IO_ifs *)i->ptr)->filename_pattern_ == "*.ksd")) {
                auto new_base = new DeviceOpenAction(manager, (IO_ifs *)i->ptr, widget_);
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

        FuncProxyQAction *action = new FuncProxyQAction(
            [&]() {
                auto node = (DeviceListModel::TreeNode *)(widget_->selectionModel()->currentIndex().internalPointer());
                while (!node->isDevice()) node = node->parent;
                device_list_model_->setActiveDevice(node->getPath().first);
                return true;
            },
            widget_);
        action->setShortcut(Qt::Key_Return);
        action->setText("&Set active device");
        widget_->addAction(action);
    }

    ~DeviceViewWrapper() override { delete widget_; }

    status addSignal(Signal_ifs *signal) override { return status::failure; }

    QWidget *getWidget() override { return widget_; }

    bool setActive(const std::string &source, const std::string &path) override {
        if (path.empty()) device_list_model_->setActiveDevice(source);
        return true;
    }

    bool removeFromActive() override {
        widget_->setCurrentIndex(QModelIndex());
        return true;
    }

    bool addToSelected(const std::string &source, const std::string &path) override {
        auto s_model = widget_->selectionModel();
        auto list = device_list_model_->getTreeIndexList(source, path);
        for (auto i : list) s_model->select(i, QItemSelectionModel::Select);
        return !list.empty();
    }

    bool removeFromSelected(const std::string &source, const std::string &path) override {
        // TODO: remove code repeat
        auto s_model = widget_->selectionModel();
        auto list = device_list_model_->getTreeIndexList(source, path);
        for (auto i : list) s_model->select(i, QItemSelectionModel::Deselect);
        return !list.empty();
    }

    Device *getActiveDevice() override { return (Device *)device_list_model_->getActiveDevice()->m_object; };

    std::string getActiveDeviceSource() override { return getActiveDevice()->getSource(); }

    Module_ifs *getActiveModule() override {
        auto index = widget_->selectionModel()->currentIndex();
        if (!index.isValid()) return nullptr;
        auto node = ((DeviceListModel::TreeNode *)index.internalPointer());
        return node->isDevice()? nullptr : node->m_object;
    };

    std::pair<std::string, std::string> getActiveModulePath() override {
        auto index = widget_->selectionModel()->currentIndex();
        if (!index.isValid()) return {};
        auto node = ((DeviceListModel::TreeNode *)index.internalPointer());
        return node->getPath();
    }

    std::vector<Module_ifs *> getSelected() override {
        std::vector<Module_ifs *> vector;
        auto indexes = widget_->selectionModel()->selectedIndexes();
        for (auto &i : indexes) {
            auto node = (DeviceListModel::TreeNode *)i.internalPointer();
            vector.push_back(node->m_object);
        }
        return vector;
    };

    std::vector<std::pair<std::string, std::string>> getSelectedPath() override {
        std::vector<std::pair<std::string, std::string>> vector;
        auto indexes = widget_->selectionModel()->selectedIndexes();
        for (auto &i : indexes) {
            auto node = (DeviceListModel::TreeNode *)i.internalPointer();
            vector.push_back(node->getPath());
        }
        return vector;
    }

    void addAction(ExrtAction_ifs *action) override {
        auto q_action = new ProxyQAction(action, widget_);
        widget_->addAction(q_action);
    }

   protected:
    QTreeView *widget_;
    DeviceListModel *device_list_model_ = nullptr;
};

/*
 *
 *
 *
 *
 *
 */

WidgetWrapper_ifs *newDeviceViewWrapper() {
    auto wrapper = new DeviceViewWrapper();
    return wrapper;
}

int initDeviceView(ExtensionManager *manager) {
    auto view_wrapper =
        (DeviceViewWrapper *)manager->getLastVersionExtensionObject("widget_wrapper", "device_view_wrapper");

    view_wrapper->init(manager);
    return 0;
}