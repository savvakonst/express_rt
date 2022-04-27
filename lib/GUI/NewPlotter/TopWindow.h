//
// Created by SVK on 27.03.2022.
//

#ifndef EXRT_TOPWINDOW_H
#define EXRT_TOPWINDOW_H

#include <QAbstractTableModel>
#include <QMainWindow>
//
#include "common/ExtensionManager.h"

class DeviceManager;
class DeviceViewWrapper_ifs;
class QFormScreen;

class TopWindow : public QMainWindow {
    Q_OBJECT
   public:
    explicit TopWindow(QWidget *parent = nullptr);

    void init(ExtensionManager *manager);

    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void addPlotter(const std::string &name, QFormScreen *form_screen);

    bool block_ = false;
   private slots:
    void onRemoveDocWidget(QWidget *widget);

   private:
    DeviceManager *device_manager_ = nullptr;
    DeviceViewWrapper_ifs *device_view_wrapper_ = nullptr;
    ExtensionManager *manager_ = nullptr;
};

class Parameter_ifs;
class Device_ifs;
class DataSchema_ifs;
class ParameterViewWrapper_ifs;
class ParameterBufferModel;

class QTreeView;

class ParametersToPlot : public QWidget {
    Q_OBJECT
   public:
    explicit ParametersToPlot(TopWindow *plotter_main_window, Device_ifs *device, ExtensionManager *manager,
                              QWidget *parent = nullptr);
    ~ParametersToPlot() override;
   public slots:

    void runAndClose();

   private:
    QTreeView *tree_view_ = nullptr;
    TopWindow *plotter_main_window_ = nullptr;
    ParameterBufferModel *model_ = nullptr;
    ExtensionManager *manager_ = nullptr;
    Device_ifs *device_ = nullptr;

    QTimer *timer_ = nullptr;

   signals:
    void toRemove(QWidget *widget);
};

class ParameterBufferModel : public QAbstractItemModel {
    Q_OBJECT
   public:
    explicit ParameterBufferModel(Device_ifs *device, ExtensionManager *manager);

    ~ParameterBufferModel() override { delete root_node_; }

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        if (!hasIndex(row, column, parent)) return {};
        auto node = parent.isValid() ? getNode(parent) : root_node_;
        if (node->rowsCount() > size_t(row)) return createIndex(row, column, node->getChild(row));
        return {};
    }

    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override {
        if (!index.isValid()) return {};
        auto parent_node = getNode(index)->parent_;
        if (parent_node == root_node_) return {};
        return createIndex(int(parent_node->getIndex()), 0, parent_node);
    }

    [[nodiscard]] QModelIndex sibling(int row, int column, const QModelIndex &model_index) const override {
        return index(row, column, model_index.parent());
    }

    [[nodiscard]] bool hasChildren(const QModelIndex &parent) const override {
        if (!parent.isValid()) return rowCount(parent) > 0 && columnCount(parent) > 0;
        return false;
    }

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override {
        if (parent.column() > 0) return 0;
        auto node = parent.isValid() ? getNode(parent) : root_node_;
        if (node) return int(node->rowsCount());
        return 0;
    }

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override { return (int)list_of_entries_.size(); }

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    //////////////
    [[nodiscard]] QStringList mimeTypes() const override {
        return {QAbstractItemModel::mimeTypes().at(0), "text/parameter", "int/parameters_to_plot"};
    }

    [[nodiscard]] QMimeData *mimeData(const QModelIndexList &indexes) const override;

    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                         const QModelIndex &parent) const override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                      const QModelIndex &parent) override;

    const QList<Parameter_ifs *> &getParameters() {
        parameters_ = root_node_->getParametersList();
        return parameters_;
    }

    void addGroup(const QList<QModelIndex> &index_list) {
        // for (auto i : index_list)
        //      if (i.row() == 0)
        //          model_->
        //              return true;
    }

   protected:
    class TreeNode {
       public:
        explicit TreeNode(Parameter_ifs *parameter, TreeNode *parent = nullptr) {
            parameter_ = parameter;
            parent_ = parent;
        }

        explicit TreeNode(const QString &name) { data_ = name; }

        [[nodiscard]] bool isParameter() const { return parameter_ != nullptr; }

        bool addSubNode(Parameter_ifs *parameter) {
            if (isParameter()) return false;
            child_vector_.push_back(new TreeNode(parameter, this));
            return true;
        }

        bool insertNodes(int start_index, const std::list<TreeNode *> &nodes) {
            if (start_index > child_vector_.size()) return false;
            child_vector_.insert(child_vector_.begin() + start_index, nodes.begin(), nodes.end());
            return true;
        }

        std::list<TreeNode *> removeSubNodes(const std::list<TreeNode *> &nodes) {
            if (start_index >= child_vector_.size()) return {};
            auto end_index = start_index + len;
            auto last = end_index < child_vector_.size() ? child_vector_.begin() + end_index : child_vector_.end();

            std::list<TreeNode *> ret;
            std::copy(child_vector_.begin() + end_index, last, std::back_inserter(ret));
            child_vector_.erase(child_vector_.begin() + start_index, last);
            return ret;
        }

        std::list<TreeNode *> removeSubNodes(int start_index, int len = 1) {
            if (start_index >= child_vector_.size()) return {};
            auto end_index = start_index + len;
            auto last = end_index < child_vector_.size() ? child_vector_.begin() + end_index : child_vector_.end();

            std::list<TreeNode *> ret;
            std::copy(child_vector_.begin() + end_index, last, std::back_inserter(ret));
            child_vector_.erase(child_vector_.begin() + start_index, last);
            return ret;
        }

        [[nodiscard]] int getIndex() const {
            if (parent_ == nullptr) return -1;
            int cnt = 0;
            for (auto i : parent_->child_vector_) {
                if (i == this) return cnt;
                cnt++;
            }
            return -1;
        }

        QString getData(const std::string &name);

        /// TreeNode* getParent(){return parent_;}

        [[nodiscard]] bool isRoot() const { return parent_ == nullptr; }

        bool contains(Parameter_ifs *parameter) const {
            if (parameter == parameter_) return true;
            if (!isParameter())
                for (auto i : child_vector_)
                    if (i->contains(parameter)) return true;
            return false;
        }

        [[nodiscard]] int rowsCount() const { return int(child_vector_.size()); }

        QList<Parameter_ifs *> getParametersList() {
            QList<Parameter_ifs *> ret;
            for (auto i : child_vector_)
                if (i->parameter_) {
                    ret.push_back(i->parameter_);
                }

            return ret;
        }

        TreeNode *getChild(int i) { return child_vector_[i]; }

        TreeNode *parent_ = nullptr;

       private:
        std::vector<TreeNode *> child_vector_;
        Parameter_ifs *parameter_ = nullptr;
        QString data_ = "";
    };

    static TreeNode *getNode(const QModelIndex &index) { return (TreeNode *)index.internalPointer(); }

    std::vector<DataSchema_ifs *> list_of_entries_;

    QList<Parameter_ifs *> parameters_;
    TreeNode *root_node_ = nullptr;

    Device_ifs *device_ = nullptr;
    DeviceViewWrapper_ifs *device_view_wrapper_ = nullptr;
    ParameterViewWrapper_ifs *parameter_view_wrapper_ = nullptr;
};

#endif  // EXRT_TOPWINDOW_H
