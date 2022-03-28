//
// Created by SVK on 07.02.2022.
//

#ifndef EXRT_DEVICESLIST_H
#define EXRT_DEVICESLIST_H

#include <QAbstractTableModel>
#include <QMap>
#include <QTreeView>

#include "common/BaseClass_ifs.h"
#include "device/Module_ifs.h"

class DataSchema_ifs;
class ExtensionManager;
class DeviceManager;
class BaseSignalController;

class TreeEditor;

class DeviceListModel : public QAbstractItemModel {
    Q_OBJECT

   public:
    explicit DeviceListModel(ExtensionManager *manager);

    ~DeviceListModel() override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

    [[nodiscard]] QStringList mimeTypes() const override;
    [[nodiscard]] QMimeData *mimeData(const QModelIndexList &indexes) const override;
    [[nodiscard]] Qt::DropActions supportedDropActions() const override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                      const QModelIndex &parent) override;

    void buildTree();

    struct TreeNode;

    [[nodiscard]] static TreeNode *getNode(const QModelIndex &index);
    [[nodiscard]] QList<QModelIndex> getTreeIndexList(const std::string &source, const std::string &path) const;
    [[nodiscard]] std::list<TreeNode *> getTreeNodeList(const std::string &source, const std::string &path) const;
    [[nodiscard]] TreeNode *getTreeNode(const std::string &source, const std::string &path) const;

    bool setActiveDevice(const std::string &source);
    TreeNode *getActiveDevice() { return active_device_; }

    struct TreeNode {
        // it is better to avoid direct deletion.

        explicit TreeNode() : m_object(nullptr) {}
        explicit TreeNode(Module_ifs *ptr) : m_object(ptr) {}

        [[nodiscard]] bool isDevice() const { return (m_object != nullptr) && (parent->parent == nullptr); }

        ~TreeNode() {
            for (auto i : child_vector) delete i;
        }

        bool removeChild(size_t index) {
            if (index < child_vector.size()) return false;
            delete child_vector[index];

            auto s = child_vector.erase(child_vector.begin() + ptrdiff_t(index));
            return true;
        }

        [[nodiscard]] size_t getIndex() const { return self_index; }

        [[nodiscard]] std::pair<std::string, std::string> getPath() const;
        void addNodesRecursively(Module_ifs *ptr, const std::string &path = "");

        std::string module_type;
        std::string path_chunk;

        TreeNode *parent = nullptr;

        Module_ifs *m_object;

        std::vector<TreeNode *> child_vector;
        size_t self_index = 0;
    };

   private:
    void addItemsToNodeMap(TreeNode *node);

    QMap<QString, QIcon> icons_map_;

    std::map<std::string, TreeNode *> node_map_;
    TreeNode *root_ = nullptr;
    TreeNode *active_device_ = nullptr;
    DeviceManager *device_manager_ = nullptr;
};

#endif
