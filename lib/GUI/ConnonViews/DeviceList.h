//
// Created by SVK on 07.02.2022.
//

#ifndef EXRT_DEVICESLIST_H
#define EXRT_DEVICESLIST_H

#include <QAbstractTableModel>
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

    [[nodiscard]] void buildTree();

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

    struct TreeNode {
        // it is better to avoid direct deletion.

        explicit TreeNode(Module_ifs *ptr) : object(ptr) {}

        ~TreeNode() {
            for (auto i : child_vector) delete i;
        }

        bool removeChild(size_t index) {
            if (index < child_vector.size()) return false;
            delete child_vector[index];

            auto s = child_vector.erase(child_vector.begin() + ptrdiff_t(index));
            return true;
        }

        size_t getIndex() {
            // TODO: implement this
            return self_index;
        }

        void addNodesRecursively(Module_ifs *ptr, const std::string &path = "") {
            auto node = new TreeNode(ptr);
            node->parent = this;

            // auto slot = ptr->getPropertyAsTxt("header/slot");
            // auto sub_slot = ptr->getPropertyAsTxt("header/sub");
            // auto path = slot + ((sub_slot == "0" || sub_slot == "") ? "" : "." + sub_slot);

            // node->path_chunk_ = path + ": " + ptr->getID();

            node->self_index = child_vector.size();
            child_vector.push_back(node);

            node->path_chunk = path + ": " + (ptr ? ptr->getID() : "");
            if (ptr == nullptr) return;
            auto list = ptr->getSubModules();
            for (const auto &i : list) {
                node->addNodesRecursively(i.second, i.first);
            }
        }

        size_t self_index = 0;
        std::string path_chunk;
        TreeNode *parent = nullptr;
        Module_ifs *object = nullptr;
        std::vector<TreeNode *> child_vector;
    };
    
   private:
    TreeNode *root_ = nullptr;
    std::vector<DataSchema_ifs *> list_of_entries_;
    DeviceManager *device_manager_ = nullptr;
};

class DeviceList {};

#endif  // EXRT_DEVICELIST_H
