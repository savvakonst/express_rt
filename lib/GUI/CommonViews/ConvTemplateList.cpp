#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>
#include <QMenu>
#include <QTableView>
//
#include "GUI/WidgetWrappers.h"
#include "common/ExtensionManager.h"
#include "common/IO_ifs.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "convtemplate/Parameter_ifs.h"
//
#include "ConvTemplateList.h"

ConvTemplateTreeModel::ConvTemplateTreeModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUnit("data_schema", "conversion_template");
    if (unit && unit->ptr) {
        schema_ = (DataSchema_ifs *)unit->ptr;
        schema_->init(manager);
        list_of_entries_ = schema_->getMapList();
    }

    unit = manager->getLastVersionExtensionUnit("cnv_template_manager", "cnv_template_manager");
    if (unit && unit->ptr) {
        manager_ = (ConversionTemplateManager *)unit->ptr;
        std::cout << "cnv_manager_ = (ConversionTemplateManager *)unit->ptr;\n";
    }
}

ConvTemplateTreeModel::~ConvTemplateTreeModel() = default;

QVariant ConvTemplateTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};

    if (role != Qt::DisplayRole) return {};

    auto ptr = (ConversionTemplate *)index.internalPointer();
    if (ptr != nullptr) {
        auto &prm_map = ptr->getAllParameters();

        auto it = prm_map.begin();
        std::advance(it, index.row());
        return it->first.data();
    }

    auto conv_template = manager_->getConversionTemplateByIndex(index.row());

    auto name = list_of_entries_[index.column()]->name_;
    auto data = conv_template->getProperty(name);
    if (data->isValue()) return {data->getValue().asString().data()};

    // TODO: use getSize()
    if (name != "changes") return "";

    auto array = data->getArray();

    switch (array.size()) {
    case 0:
        return {"no date"};
    case 1:
        return {array[0]->getMapUnit("time")->getValue().asString().data()};
    default:
        auto first = array[0]->getMapUnit("time")->getValue().asString();
        auto end = array[array.size() - 1]->getMapUnit("time")->getValue().asString();
        auto s = first + " / " + end;
        return {s.data()};
    }
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
    if (!hasIndex(row, column, parent)) return {};

    if (!parent.isValid()) {
        auto conv_tmp = manager_->getConversionTemplateByIndex((size_t)row);
        if (conv_tmp) return createIndex(row, column, nullptr);
    } else {
        auto conv_tmp = manager_->getConversionTemplateByIndex((size_t)parent.row());
        if (row < conv_tmp->getAllParameters().size()) return createIndex(row, column, conv_tmp);
    }

    return {};
}

QModelIndex ConvTemplateTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return {};

    auto ptr = (ConversionTemplate *)index.internalPointer();
    if (ptr) return createIndex(int(manager_->getIndex(ptr)), 0, nullptr);

    return {};
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
    auto unit = manager->getLastVersionExtensionUnit("data_schema", "conversion_template");
    if (unit && unit->ptr) {
        schema_ = (DataSchema_ifs *)unit->ptr;
        schema_->init(manager);
        list_of_entries_ = schema_->getMapList();
    }

    unit = manager->getLastVersionExtensionUnit("cnv_template_manager", "cnv_template_manager");
    if (unit && unit->ptr) {
        cnv_manager_ = (ConversionTemplateManager *)unit->ptr;
        if (cnv_manager_ == nullptr) std::cerr << "ConvTemplateTableModel::cnv_manager_ == nullptr;\n";
    }

    class [[maybe_unused]] Delegate : public Signal_ifs {
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
#include "convtemplate/ParameterFieldTree.h"
QVariant ConvTemplateTableModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole) return {};

    auto conv_template = cnv_manager_->getConversionTemplateByIndex(index.row());
    auto name = list_of_entries_[index.column()]->name_;
    auto data = conv_template->getProperty(name);

    if (data->isValue()) return {data->getValue().asString().data()};

    // TODO: use getSize()
    if (name != "changes") return "";

    auto array = data->getArray();

    auto time = array[0]->getMapUnit("time");

    switch (array.size()) {
    case 0:
        return {"no date"};
    case 1:
        return {array[0]->getMapUnit("time")->getValue().asString().data()};
    default:
        auto first = array[0]->getMapUnit("time")->getValue().asString();
        auto end = array[array.size() - 1]->getMapUnit("time")->getValue().asString();
        auto s = first + " / " + end;
        return {s.data()};
    }
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
 *
 */

class ConversionTemplateViewWrapper : public ConversionTemplateViewWrapper_if {
   public:
    ConversionTemplateViewWrapper() : widget_(new QTreeView()) {}

    size_t rowCount() { return size_t(widget_->model()->rowCount()); }

    status addSignal(Signal_ifs *signal) override { return status::failure; }

    QWidget *getWidget() override { return widget_; }

    bool setActive(size_t row_index) override {
        auto cnt = rowCount();
        for (int i = 0; i < cnt; i++) {
            auto index = widget_->model()->index(i, 0);
        }
        // TODO: implement method
        return false;
    }

    bool setActive(const std::string &name) override {
        // TODO: implement method
        return false;
    }

    bool removeFromActive() override {
        // TODO: implement method
        return false;
    }

    bool addToSelected(size_t row_index) override {
        // TODO: implement method
        return false;
    }

    bool addToSelected(const std::string &name) override {
        // TODO: implement method
        return false;
    }

    bool removeFromSelected(size_t row_index) override {
        // TODO: implement method
        return false;
    }

    ConversionTemplate *getActive() override {
        // TODO: implement method
        return nullptr;
    }

    std::vector<ConversionTemplate *> getSelected() override {
        // TODO: implement method
        return {false};
    }

   protected:
    QTreeView *widget_;
};

/*
 *
 *
 *
 */

QWidget *newConversionTemplateView(QWidget *parent) {
    auto table_view = new QTreeView();

    table_view->setAlternatingRowColors(true);
    table_view->setStyleSheet(
        "QTreeView {background-color: #D2DCDF; alternate-background-color: #f6fafb; show-decoration-selected: 1;}"
        "QHeaderView::section {background-color: #D2DCDF}");
    table_view->setContextMenuPolicy(Qt::ActionsContextMenu);
    // table_view->setSelectionMode( QAbstractItemView::SelectionMode::ExtendedSelection );

    return table_view;
}

int initConversionTemplateView(ExtensionManager *manager) {
    auto view = (QAbstractItemView *)manager->getLastVersionExtensionObject("widget", "conv_template_list");

    view->setModel(new ConvTemplateTableModel(manager));

    auto io_units = manager->getLastVersionExtensionUnitsByType("io");
    for (auto i : io_units) {
        if (i && i->ptr && (((IO_ifs *)i->ptr)->filename_pattern_ == "*.base")) {
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
    return 0;
}