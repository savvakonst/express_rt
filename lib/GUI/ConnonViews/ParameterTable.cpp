

#include <QHeaderView>
#include <QTableView>

#include "QDebug"
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
    }  // else if (role == Qt::CheckStateRole) {
       //    if (index.column() == 0) return Qt::Unchecked;
    //}
    return {};
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

    auto it = prm_map.begin();
    for (auto i : indexes) {
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

#include "CustomQActions.h"
#include "qformscreen.h"

QPixmap getPixmap(QString str) {
    auto pixmap_path = QCoreApplication::applicationDirPath() + str;
    return QPixmap(pixmap_path);
}

class ParameterViewWrapper : public ParameterViewWrapper_ifs {
   public:
    ParameterViewWrapper() : widget_(new ParameterTreeView()) {
        widget_->setAlternatingRowColors(true);
        widget_->setStyleSheet(
            "QTreeView {background-color: #D2DCDF; alternate-background-color: #f6fafb; show-decoration-selected: 1;}"
            "QHeaderView::section {background-color: #D2DCDF}");

        top_widget_ = new QWidget();
        auto layout = new QVBoxLayout(top_widget_);

        layout->addWidget(widget_);

        auto p_layout = reinterpret_cast<QHBoxLayout *>(top_widget_->layout());
        auto toolbar = new QToolBar();

        QIcon run_icon;
        run_icon.addPixmap(getPixmap("/png/common/play.png"), QIcon::Normal, QIcon::Off);
        run_icon.addPixmap(getPixmap("/png/common/stop.png"), QIcon::Normal, QIcon::On);

        action_run_ = new FuncProxyQAction(
            run_icon, QObject::tr("Ru&n"),
            [&]() {
                if (main_window_ == nullptr) return false;

                main_window_->setCentralWidget(dock);

                QList<int> dock_widths;
                QList<QDockWidget *> dock_widgets;
                qDebug() << "-------------------";

                int last_width = 400;
                for (auto i : main_window_->findChildren<QDockWidget *>())
                    if (i->objectName() != "&plotter") {
                        dock_widths.push_back(i->width());
                        dock_widgets.push_back(i);
                        qDebug() << "\t" << i->objectName() << ": " << i->width();
                    } else {
                        last_width = i->width();
                    }

                auto dock = new QDockWidget(QObject::tr("&plotter"), main_window_);
                dock->setObjectName(QObject::tr("&plotter"));

                QWidget *form_screen = !action_run_->isChecked() ? new QWidget : new QFormScreen();

                dock->setWidget(form_screen);

                // delete main_window_->takeCentralWidget();

                main_window_->setCentralWidget(dock);
                // main_window_->resizeDocks(dock_widgets, dock_widths, Qt::Horizontal);
                main_window_->resizeDocks({dock}, {last_width}, Qt::Horizontal);

                // main_window_->addDockWidget(Qt::RightDockWidgetArea, dock);
                return true;
            },
            widget_);

        action_run_->setCheckable(true);

        toolbar->addAction(action_run_);
        p_layout->insertWidget(0, toolbar);
    }

    void init(ExtensionManager *manager) {
        auto model = new ParameterTableModel(manager);

        main_window_ = (QMainWindow *)manager->getLastVersionExtensionObject("main_window", "main_window");

        QObject::connect(widget_, &ParameterTreeView::currentChangedSignal, model,
                         &ParameterTableModel::selectParameter);
        widget_->setModel(model);
    }

    status addSignal(Signal_ifs *signal) override {
        signals_.push_back(signal);
        return status::succes;
    }

    QWidget *getWidget() override { return top_widget_; }

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
