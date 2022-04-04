

#include <sstream>
//
#include <QDebug>
#include <QHeaderView>
#include <QMimeData>
#include <QTableView>
//
#include "GUI/TreeEditor.h"
#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "convtemplate/Parameter_ifs.h"
#include "device/Device.h"
//
#include "ParameterTable.h"

/*
 *
 *
 *
 *
 */

ParameterTableModel::ParameterTableModel(ExtensionManager *manager) {
    auto unit = manager->getLastVersionExtensionUnit("data_schema", "common");
    if (unit && unit->ptr) {
        schema_ = (DataSchema_ifs *)unit->ptr;
        schema_->init(manager);
        list_of_entries_ = schema_->getMapList();
    } else {
        std::cerr << "can't find data_schema;\n";
    }

    device_view_wrapper_ =
        (DeviceViewWrapper_ifs *)manager->getLastVersionExtensionObject("widget_wrapper", "device_view_wrapper");
    if (device_view_wrapper_ == nullptr) std::cerr << "can't find device_view_wrapper;\n";

    parent_view_ = (QAbstractItemView *)manager->getLastVersionExtensionObject("widget", "conv_template_list");
    if (parent_view_ == nullptr) std::cerr << "can't find conv_template_list;\n";

    cnv_manager_ = (ConversionTemplateManager *)manager->getLastVersionExtensionObject("cnv_template_manager",
                                                                                       "cnv_template_manager");
    if (cnv_manager_ == nullptr) std::cerr << "can't find cnv_template_manager;\n";

    child_view_ = (TreeEditor *)manager->getLastVersionExtensionObject("tree_editor", "tree_editor");
    if (child_view_ == nullptr) std::cerr << "can't find cnv_template_manager;\n";

    class Delegate : public Signal_ifs {
       public:
        explicit Delegate(ParameterTableModel *parent) : parent_(parent) {}
        void emit_() override { parent_->layoutChanged(); }

       private:
        ParameterTableModel *parent_;
    };

    cnv_manager_->addSignal(new Delegate(this));

    connect(parent_view_, &QAbstractItemView::activated, this, &ParameterTableModel::receiveRow);
}

int ParameterTableModel::rowCount(const QModelIndex &parent) const {
    auto conv_temp = getCurrentConversionTemplate();
    auto ret = conv_temp ? (int)conv_temp->getAllParameters().size() : 0;
    return ret;
}

int ParameterTableModel::columnCount(const QModelIndex &parent) const { return (int)list_of_entries_.size(); }

QVariant ParameterTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) return QString(list_of_entries_[section]->description_.data());
    return {};
}

QVariant ParameterTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::TextColorRole && index.column() == 0) {
        auto device = device_view_wrapper_->getActiveDevice();
        if (device == nullptr) return {};

        auto prm = getParameter(index);
        auto path = firstCharPos(prm->getProperty("common/path")->getValue().asString(), '/');

        return device->isChannelAvailable(path.second) ? QColor("red") : QVariant();

    } else if (role == Qt::DisplayRole) {
        auto prm = getParameter(index);
        auto name = list_of_entries_[index.column()]->name_;
        auto h_data = prm->getProperty("common/" + name);
        return h_data->getValue().asString().data();
    }
    return {};
}

Qt::ItemFlags ParameterTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.isValid()) return flags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    else
        return flags | Qt::ItemIsDropEnabled;
}

QModelIndex ParameterTableModel::getIndex(const std::string &name) const {
    auto conv_template = getCurrentConversionTemplate();
    auto &prm_map = conv_template->getAllParameters();

    auto prm = conv_template->getParameter(name);

    if (prm == nullptr) return {};

    auto cnt = 0;
    for (const auto &i : prm_map) {
        if (i.second == prm) break;
        cnt++;
    }

    return {};
}

Parameter_ifs *ParameterTableModel::getParameter(const QModelIndex &index) const {
    if (!index.isValid()) return nullptr;

    auto conv_template = getCurrentConversionTemplate();

    auto &prm_map = conv_template->getAllParameters();

    auto it = prm_map.begin();
    std::advance(it, index.row());

    return (it == prm_map.end()) ? nullptr : it->second;
}

std::vector<Parameter_ifs *> ParameterTableModel::getParameters(const QList<QModelIndex> &indexes) const {
    std::vector<Parameter_ifs *> ret;

    auto conv_template = getCurrentConversionTemplate();
    auto &prm_map = conv_template->getAllParameters();

    for (auto i : indexes) {
        auto it = prm_map.begin();
        std::advance(it, i.row());
        if (it == prm_map.end()) break;
        ret.push_back(it->second);
    }
    return ret;
}

ConversionTemplate *ParameterTableModel::getCurrentConversionTemplate() const {
    auto index = parent_view_->currentIndex();

    if (!index.isValid()) return nullptr;

    while (index.parent().isValid()) index = index.parent();

    return cnv_manager_->getConversionTemplateByIndex((size_t)index.row());
}

void ParameterTableModel::receiveRow(const QModelIndex &index) { layoutChanged(); }

void ParameterTableModel::selectParameter(const QModelIndex &index) {
    auto prm = getParameter(index);
    if (prm) child_view_->setupProperties(prm);
}

QStringList ParameterTableModel::mimeTypes() const { return {QAbstractItemModel::mimeTypes().at(0), "text/parameter"}; }

QMimeData *ParameterTableModel::mimeData(const QModelIndexList &indexes) const {
    auto mime_data = new QMimeData();
    std::stringstream parameters_s_stream;

    for (auto &i : indexes) {
        if (i.column() != 0) continue;

        auto parameters = getParameter(i);

        auto name = parameters->getProperty("common/name")->getValue().asString();
        parameters_s_stream << name << "\n";
    }

    mime_data->setData("text/parameter", QByteArray::fromStdString(parameters_s_stream.str()));
    return mime_data;
}

Qt::DropActions ParameterTableModel::supportedDropActions() const { return QAbstractItemModel::supportedDropActions(); }

bool ParameterTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                       const QModelIndex &parent) {
    // qDebug() << data->data("module").data();
    return QAbstractTableModel::dropMimeData(data, action, row, column, parent);
}

bool ParameterTableModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                          const QModelIndex &parent) const {
    return QAbstractItemModel::canDropMimeData(data, action, row, column, parent);
}

/*
 *
 */

/*
 *
 *
 *
 */

#include <QBoxLayout>
#include <QCoreApplication>
#include <QDockWidget>
#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>

#include "GUI/CustomQActions.h"

class ParameterViewWrapper : public ParameterViewWrapper_ifs {
   public:
    ParameterViewWrapper() : widget_(new ParameterTreeView()) {
        widget_->setAlternatingRowColors(true);
        widget_->setStyleSheet(
            "QWidget {background-color: #D2DCDF; alternate-background-color: #f6fafb; show-decoration-selected: 1;}"
            "QHeaderView::section {background-color: #D2DCDF}");

        widget_->setContextMenuPolicy(Qt::ActionsContextMenu);
        widget_->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

        // widget_->setSelectionMode(QAbstractItemView::SingleSelection);
        widget_->setDragDropMode(QAbstractItemView::DragDrop);
        widget_->setDragEnabled(true);
        widget_->setDropIndicatorShown(true);
        widget_->setAcceptDrops(true);

        top_widget_ = new QWidget();
        auto layout = new QVBoxLayout(top_widget_);
        layout->addWidget(widget_);

        auto p_layout = reinterpret_cast<QHBoxLayout *>(top_widget_->layout());
        auto toolbar = new QToolBar();

        /// action_run_->setCheckable(true);

        toolbar->addAction(action_run_);
        p_layout->insertWidget(0, toolbar);
    }

    void init(ExtensionManager *manager) {
        model_ = new ParameterTableModel(manager);

        main_window_ = (QMainWindow *)manager->getLastVersionExtensionObject("main_window", "main_window");

        QObject::connect(widget_, &ParameterTreeView::currentChangedSignal, model_,
                         &ParameterTableModel::selectParameter);
        widget_->setModel(model_);
    }

    status addSignal(Signal_ifs *signal) override {
        signals_.push_back(signal);
        return status::succes;
    }

    QWidget *getWidget() override { return top_widget_; }

    ConversionTemplate *currentConversionTemplate() override { return model_->getCurrentConversionTemplate(); }

    bool setActive(size_t row_index) override {
        auto cnt = rowCount();
        if (cnt < row_index) {
            auto index = widget_->model()->index(int(row_index), 0);
            widget_->selectionModel()->setCurrentIndex(index, {});
        }
        return true;
    }

    bool setActive(const std::string &name) override {
        auto index = model_->getIndex(name);
        widget_->selectionModel()->setCurrentIndex(index, {});
        return true;
    }

    bool removeFromActive() override {
        widget_->setCurrentIndex(QModelIndex());
        return true;
    }

    bool addToSelected(size_t row_index) override {
        auto cnt = rowCount();
        if (cnt < row_index) {
            auto index = widget_->model()->index(int(row_index), 0);
            widget_->selectionModel()->select(index, QItemSelectionModel::Rows);
        }
        return true;
    }

    bool addToSelected(const std::string &name) override {
        auto index = model_->getIndex(name);
        widget_->selectionModel()->select(index, QItemSelectionModel::Rows);
        return true;
    }

    bool removeFromSelected(size_t row_index) override {
        // TODO: remove code repeat
        auto cnt = rowCount();
        if (cnt < row_index) {
            auto index = widget_->model()->index(int(row_index), 0);
            widget_->selectionModel()->select(index, QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
        }
        return true;
    }

    Parameter_ifs *getActive() override {
        auto current_index = widget_->selectionModel()->currentIndex();
        return model_->getParameter(current_index);
    }

    std::vector<Parameter_ifs *> getSelected() override {
        auto current_index = widget_->selectionModel()->selectedRows();
        return model_->getParameters(current_index);
    }

   private:
    size_t rowCount() { return size_t(widget_->model()->rowCount()); }
    std::list<Signal_ifs *> signals_;

   protected:
    void emit_() {
        for (auto i : signals_) {
            i->emit_();
        }
    }

    DeviceViewWrapper_ifs *device_view_wrapper_ = nullptr;
    ParameterTableModel *model_ = nullptr;
    ParameterTreeView *widget_;
    QMainWindow *main_window_ = nullptr;

    QAction *action_run_ = nullptr;
    QWidget *top_widget_ = nullptr;
};

void ParameterTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    emit currentChangedSignal(current);
}

WidgetWrapper_ifs *newParameterViewWrapper() { return new ParameterViewWrapper(); }

int initParameterTreeView(ExtensionManager *manager) {
    auto view_wrapper =
        (ParameterViewWrapper *)manager->getLastVersionExtensionObject("widget_wrapper", "parameter_view_wrapper");

    view_wrapper->init(manager);
    return 0;
}
