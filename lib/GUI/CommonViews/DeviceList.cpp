#include <functional>
#include <sstream>
#include <utility>
//
#include <QAction>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QHeaderView>
#include <QMimeData>
#include <QTableView>
//
#include "GUI/WidgetWrappers.h"
#include "common/ExrtAction_ifs.h"
#include "common/ExtensionManager.h"
#include "common/IO_ifs.h"
#include "common/StringProcessingTools.h"
#include "convtemplate/Parameter_ifs.h"
#include "device/DeviceManager.h"
//
#include "DeviceList.h"
#include "GUI/CustomQActions.h"

DeviceListModel::DeviceListModel(ExtensionManager *manager) {
    auto image_dir = QDir(QCoreApplication::applicationDirPath() + "/png/modules");
    auto image_path_list = image_dir.entryInfoList({"*.png", "*.bmp", "*.ico"});

    for (auto &i : image_path_list) icons_map_[i.baseName()] = QIcon(i.absoluteFilePath());

    auto unit = manager->getLastVersionExtensionUnit("device_manager", "device_manager");
    if (unit && unit->ptr) device_manager_ = (DeviceManager *)unit->ptr;

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

    auto node = getNode(index);

    if (role == Qt::DecorationRole) {
        if (node == active_device_) return icons_map_["DEV_CHECK"];
        if (node->m_object) return icons_map_[node->module_type.c_str()];

        return {};
    } else if (role == Qt::TextColorRole) {
        if (node->m_object && !node->m_object->isAvailable()) return QColor(200, 100, 100);
        return {};
    }

    if (role != Qt::DisplayRole) return {};

    return node->path_chunk.c_str();
}

Qt::ItemFlags DeviceListModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (index.isValid()) return flags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    else
        return flags | Qt::ItemIsDropEnabled;

    // if (!index.isValid()) return Qt::NoItemFlags;
    // return QAbstractItemModel::flags(index);
}

QVariant DeviceListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && (section == 0)) return QString(tr("id"));
    return {};
}

QModelIndex DeviceListModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) return {};
    auto node = parent.isValid() ? getNode(parent) : root_node_;
    if (node->child_vector_.size() > size_t(row)) return createIndex(row, column, node->child_vector_[row]);
    return {};
}

QModelIndex DeviceListModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return {};
    auto parent_node = getNode(index)->parent;
    if (parent_node == root_node_) return {};
    return createIndex(int(parent_node->getIndex()), 0, parent_node);
}

int DeviceListModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) return 0;
    auto node = parent.isValid() ? getNode(parent) : root_node_;
    if (node) return int(node->child_vector_.size());
    return 0;
}

int DeviceListModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 1;
    return (int)1;
}

void DeviceListModel::buildTree() {
    beginResetModel();
    delete root_node_;
    node_map_.clear();

    root_node_ = new TreeNode();
    endResetModel();

    auto list = device_manager_->getConversionTemplateList();
    if (list.empty()) {
        emit layoutChanged();
        return;
    }

    root_node_->child_vector_.reserve(list.size());
    for (auto i : list) root_node_->addNodesRecursively(i, i->getSource());
    dataChanged(QModelIndex(), QModelIndex());

    for (auto i : root_node_->child_vector_) addItemsToNodeMap(i);

    emit layoutChanged();
}

DeviceListModel::TreeNode *DeviceListModel::getNode(const QModelIndex &index) {
    return (TreeNode *)index.internalPointer();
}

QList<QModelIndex> DeviceListModel::getTreeIndexList(const std::string &source, const std::string &path) const {
    QList<QModelIndex> ret;
    auto full_path = source + "//" + path;
    for (auto &i : node_map_)
        if (filePatternMatch(full_path, i.first)) ret.push_back(createIndex(i.second->getIndex(), 0, i.second));

    return ret;
}

std::list<DeviceListModel::TreeNode *> DeviceListModel::getTreeNodeList(const std::string &source,
                                                                        const std::string &path) const {
    std::list<DeviceListModel::TreeNode *> ret;
    auto full_path = source + "//" + path;
    for (auto &i : node_map_) {
        if (filePatternMatch(full_path, i.first)) ret.push_back(i.second);
    }
    return ret;
}

DeviceListModel::TreeNode *DeviceListModel::getTreeNode(const std::string &source, const std::string &path) const {
    if (path.empty()) {
        TreeNode *node = nullptr;
        for (auto i : root_node_->child_vector_)
            if (source == ((Device_ifs *)i->m_object)->getSource()) {
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
    auto &vector = node->child_vector_;
    for (TreeNode *i : vector) addItemsToNodeMap(i);
}

QStringList DeviceListModel::mimeTypes() const {
    return {QAbstractItemModel::mimeTypes().at(0), "text/module", "text/uri-list"};
}

QMimeData *DeviceListModel::mimeData(const QModelIndexList &indexes) const {
    auto mime_data = new QMimeData();
    std::stringstream modules_s_stream;
    std::stringstream devices_s_stream;

    for (auto &i : indexes) {
        auto c = getNode(i)->getPath();

        modules_s_stream << c.first << "//" << c.second << "\n";
        if (getNode(i)->isDevice()) devices_s_stream << c.first << "//" << c.second << "\n";
    }
    auto modules_s = modules_s_stream.str();
    mime_data->setData("text/module", QByteArray::fromStdString(modules_s));

    auto devices_s = devices_s_stream.str();
    if (!devices_s.empty()) mime_data->setData("text/device", QByteArray::fromStdString(devices_s));

    return mime_data;
}

Qt::DropActions DeviceListModel::supportedDropActions() const { return QAbstractItemModel::supportedDropActions(); }

bool DeviceListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                   const QModelIndex &parent) {
    // qDebug() << data->data("module").data();
    return QAbstractItemModel::dropMimeData(data, action, row, column, parent);
}

void DeviceListModel::TreeNode::addNodesRecursively(Module_ifs *ptr, const std::string &path) {
    auto node = new TreeNode(ptr);
    node->parent = this;

    node->self_index = child_vector_.size();
    child_vector_.push_back(node);

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
    if (isDevice()) return {((Device_ifs *)m_object)->getSource(), ""};

    std::string path = path_chunk;
    auto ptr = parent;
    while (!ptr->isDevice()) {
        path = ptr->path_chunk + "/" + path;
        ptr = ptr->parent;
    }
    auto dev = (Device_ifs *)ptr->m_object;
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
        connect(this, &QAction::triggered, this, &DeviceOpenAction::openFile);
    }

   protected slots:
    void openFile() {
        auto f_name = QFileDialog::getOpenFileName(nullptr, QString(), QString(), io_->filename_pattern_.c_str(),
                                                   nullptr, QFileDialog::Options());
        io_->readDocument(manager_, QDir(QDir::current()).relativeFilePath(f_name).toStdString());
    }

   protected:
    ExtensionManager *manager_ = nullptr;
    IO_ifs *io_;
};

/*
 *
 *
 *
 */
class DeviceViewWrapper : public DeviceViewWrapper_ifs {
   public:
    friend class DeviceViewWrapperDelegate;
    DeviceViewWrapper() : tree_view_(new QTreeView()) {
        tree_view_->setAlternatingRowColors(true);
        tree_view_->setStyleSheet(
            "QTreeView {background-color: #D2DCDF;  show-decoration-selected: 1;}"
            "QHeaderView::section {background-color: #D2DCDF}");
        tree_view_->setContextMenuPolicy(Qt::ActionsContextMenu);
        tree_view_->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

        // widget_->setSelectionMode(QAbstractItemView::SingleSelection);
        tree_view_->setDragDropMode(QAbstractItemView::DragDrop);
        tree_view_->setDragEnabled(true);
        tree_view_->setDropIndicatorShown(true);
        tree_view_->setAcceptDrops(true);
    }

    void init(ExtensionManager *manager) {
        device_manager_ = (DeviceManager *)manager->getLastVersionExtensionObject("device_manager", "device_manager");

        model_ = new DeviceListModel(manager);
        tree_view_->setModel(model_);

        auto io_units = manager->getLastVersionExtensionUnitsByType("io");
        for (auto i : io_units) {
            if (i) {
                auto io = (IO_ifs *)i->ptr;
                if (io && io->filename_pattern_ == "*.ksd") {
                    auto new_base = new DeviceOpenAction(manager, (IO_ifs *)i->ptr, tree_view_);

                    new_base->setText(QObject::tr("&Open"));
                    new_base->setStatusTip(QObject::tr("&Open a new file"));
                    new_base->setShortcutContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
                    new_base->setShortcut(Qt::CTRL + Qt::Key_O);
                    tree_view_->addAction(new_base);
                }
            }
        }

        /*
         *
         */
        auto remove_action = new FuncProxyQAction(
            [&]() {
                auto index = tree_view_->selectionModel()->currentIndex().row();
                device_manager_->removeDeviceByIndex(index);
                return true;
            },
            tree_view_);

        remove_action->setShortcutContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
        remove_action->setShortcut(Qt::Key_Delete);
        remove_action->setText("&Delete");
        tree_view_->addAction(remove_action);

        /*
         *
         */
        auto *set_active_action = new FuncProxyQAction(
            [&]() {
                auto node = model_->getNode(tree_view_->selectionModel()->currentIndex());
                while (!node->isDevice()) node = node->parent;
                setActive(node->getPath().first, "");
                return true;
            },
            tree_view_);

        set_active_action->setShortcut(Qt::Key_Return);
        set_active_action->setText("&Set active device");
        tree_view_->addAction(set_active_action);
    }

    ~DeviceViewWrapper() override { delete tree_view_; }

    status addSignal(Signal_ifs *signal) override {
        signals_.push_back(signal);
        return status::succes;
    }

    QWidget *getWidget() override { return tree_view_; }

    bool setActive(const std::string &source, const std::string &path) override {
        emit_();
        if (path.empty()) model_->setActiveDevice(source);
        return true;
    }

    bool removeFromActive() override {
        tree_view_->setCurrentIndex(QModelIndex());
        return true;
    }

    bool addToSelected(const std::string &source, const std::string &path) override {
        auto s_model = tree_view_->selectionModel();
        auto list = model_->getTreeIndexList(source, path);
        for (auto i : list) s_model->select(i, QItemSelectionModel::Select);
        return !list.empty();
    }

    bool removeFromSelected(const std::string &source, const std::string &path) override {
        // TODO: remove code repeat
        auto s_model = tree_view_->selectionModel();
        auto list = model_->getTreeIndexList(source, path);
        for (auto i : list) s_model->select(i, QItemSelectionModel::Deselect);
        return !list.empty();
    }

    Device_ifs *getActiveDevice() override {
        auto node = model_->getActiveDevice();
        return node ? (Device_ifs *)node->m_object : nullptr;
    };

    std::string getActiveDeviceSource() override { return getActiveDevice()->getSource(); }

    Module_ifs *getActiveModule() override {
        auto index = tree_view_->selectionModel()->currentIndex();
        if (!index.isValid()) return nullptr;
        auto node = model_->getNode(index);
        return node->isDevice() ? nullptr : node->m_object;
    };

    std::pair<std::string, std::string> getActiveModulePath() override {
        auto index = tree_view_->selectionModel()->currentIndex();
        if (!index.isValid()) return {};
        auto node = model_->getNode(index);
        return node->getPath();
    }

    std::vector<Module_ifs *> getSelected() override {
        std::vector<Module_ifs *> vector;
        auto indexes = tree_view_->selectionModel()->selectedIndexes();
        for (auto &i : indexes) {
            auto node = model_->getNode(i);
            vector.push_back(node->m_object);
        }
        return vector;
    };

    std::vector<std::pair<std::string, std::string>> getSelectedPath() override {
        std::vector<std::pair<std::string, std::string>> vector;
        auto indexes = tree_view_->selectionModel()->selectedIndexes();
        for (auto &i : indexes) {
            auto node = model_->getNode(i);
            vector.push_back(node->getPath());
        }
        return vector;
    }

    void addAction(ExrtAction_ifs *action) override {
        auto q_action = new ProxyQAction(action, tree_view_);
        tree_view_->addAction(q_action);
    }

   protected:
    void emit_() {
        for (auto i : signals_) {
            i->emit_();
        }
    }

    std::list<Signal_ifs *> signals_;
    QTreeView *tree_view_ = nullptr;
    DeviceListModel *model_ = nullptr;
    DeviceManager *device_manager_ = nullptr;
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
