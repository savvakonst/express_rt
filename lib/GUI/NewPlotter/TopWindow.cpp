//
// Created by SVK on 27.03.2022.
//

#include <QBoxLayout>
#include <QCoreApplication>
#include <QDebug>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QTreeView>
///
#include "GUI/CustomQActions.h"
#include "TopWindow.h"
#include "common/StringProcessingTools.h"
#include "device/DeviceManager.h"
#include "qformscreen.h"

TopWindow::TopWindow(QWidget *parent) {
    setAcceptDrops(true);
    setUnifiedTitleAndToolBarOnMac(true);
}

void TopWindow::init(ExtensionManager *manager) {
    manager_ = manager;
    device_manager_ = (DeviceManager *)manager->getLastVersionExtensionObject("device_manager", "device_manager");
}

void TopWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasFormat("text/module")) {
        auto list = split(e->mimeData()->data("text/device").data(), '\n');
        if (list.size() == 1) e->acceptProposedAction();
    }
}

void TopWindow::dropEvent(QDropEvent *e) {
    if (e->mimeData()->hasFormat("text/device")) {
        auto list = split(e->mimeData()->data("text/device").data(), '\n');

        for (const auto &i : list) {
            auto dock_widgets = findChildren<QDockWidget *>();
            auto title = QString::fromStdString(i);

            bool exists = false;
            for (auto dock : dock_widgets)
                if (dock->windowTitle() == title) {
                    exists = true;
                    break;
                }

            if (!exists) {
                auto dock = new QDockWidget(i.data(), this);
                auto device = device_manager_->getDeviceByPath(i);
                auto pre_plotter = new PrePlotter(device, manager_);
                connect(pre_plotter, &PrePlotter::toRemove, this, &TopWindow::onRemoveDocWidget);

                dock->setAttribute(Qt::WA_DeleteOnClose);
                dock->setWidget(pre_plotter);
                addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dock, Qt::Orientation::Horizontal);
            }
        }
    }
}

void TopWindow::onRemoveDocWidget(QWidget *widget) {
    auto dock_widgets = findChildren<QDockWidget *>();
    qDebug() << dock_widgets.size();
    for (auto i : dock_widgets)
        if (i->widget() == widget) {
            i->close();
            return;
        }
}

/*
 *
 *
 *
 */

ModuleStream_ifs *generateStream(ExtensionManager *manager, const std::string &type, Device *device,
                                 const std::list<Parameter_ifs *> &parameters) {
    ModuleStream_ifs *top_m_stream = device->createModuleStream();

    for (auto prm : parameters) {
        auto constructor = (prmBufferConstructor_f)manager->getLastVersionExtensionObject(type, prm->getType());
        if (constructor) {
            auto prm_buffer = constructor(prm, manager);

            auto full_path = prm->getProperty("common/path")->getValue().asString();

            auto path = lastCharPos(full_path, '/');
            auto sub_modules = ::getSubmodules(device, path.first);
            if (sub_modules.size() == 1) {
                auto m = sub_modules.front()->getModuleStream();
                if (m) top_m_stream->addPrmBuffer(path.second, prm_buffer);
            }
        }
    }

    return top_m_stream;
}

/*
 *
 *
 *
 *
 */
PrePlotter::PrePlotter(Device *device, ExtensionManager *manager, QWidget *parent) : QWidget(parent) {
    auto table = new QTreeView();
    // model_ = new ParameterBufferTableModel(device, manager);

    table->setParent(this);
    model_->setParent(this);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(table);

    QIcon run_icon;
    run_icon.addPixmap(getPixmap("/png/common/play.png"), QIcon::Normal, QIcon::Off);
    run_icon.addPixmap(getPixmap("/png/common/stop.png"), QIcon::Normal, QIcon::On);

    auto *action_run = new QAction(run_icon, QObject::tr("Ru&n"), this);
    connect(action_run, &QAction::triggered, this, &PrePlotter::runAndClose);

    table->setDragDropMode(QAbstractItemView::DragDrop);
    table->setDragEnabled(true);
    table->setDropIndicatorShown(true);
    table->setAcceptDrops(true);
    table->setModel(model_);

    auto p_layout = reinterpret_cast<QHBoxLayout *>(this->layout());
    auto toolbar = new QToolBar(this);
    toolbar->addAction(action_run);
    p_layout->insertWidget(0, toolbar);
}

PrePlotter::~PrePlotter() = default;

void PrePlotter::runAndClose() { emit toRemove(this); }
void PrePlotter::createPlotter(QFormScreen *form_screen) { auto vector = model_->getParameters().toStdList(); }

/*
 *
 *
 *
 */

#include "GUI/WidgetWrappers.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "convtemplate/Parameter_ifs.h"
#include "device/Device.h"

ParameterBufferTableModel::ParameterBufferTableModel(Device *device, ExtensionManager *manager) : device_(device) {
    parameter_view_wrapper_ =
        (ParameterViewWrapper_ifs *)manager->getLastVersionExtensionObject("widget_wrapper", "parameter_view_wrapper");

    auto schema = (DataSchema_ifs *)manager->getLastVersionExtensionObject("data_schema", "common");
    if (schema) list_of_entries_ = schema->getMapList();
    else
        std::cerr << "can't find data_schema;\n";
}

QVariant ParameterBufferTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) return QString(list_of_entries_[section]->description_.data());
    return {};
}

QVariant ParameterBufferTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        auto prm = parameters_[index.row()];
        auto name = list_of_entries_[index.column()]->name_;
        auto h_data = prm->getProperty("common/" + name);
        return h_data->getValue().asString().data();
    }
    return {};
}

Qt::ItemFlags ParameterBufferTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.isValid()) return flags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    else
        return flags | Qt::ItemIsDropEnabled;
}

bool ParameterBufferTableModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                                const QModelIndex &parent) const {
    qDebug() << "Qt::DropActions ParameterTableModel::supportedDropActions()";

    if (data->hasFormat("text/parameter")) {
        auto list = split(data->data("text/parameter").data(), '\n');
        if (list.size() > 1) return true;

        auto name = list.front();

        auto cnv_template = parameter_view_wrapper_->currentConversionTemplate();

        if (cnv_template) {
            auto parameter = (Parameter_ifs *)cnv_template->getParameter(name);
            if (parameter && !parameters_.contains(parameter)) {
                auto path = firstCharPos(parameter->getProperty("common/path")->getValue().asString(), '/');

                if (device_->isChannelAvailable(path.second)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool ParameterBufferTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                             const QModelIndex &parent) {
    auto ret = false;
    if (data->hasFormat("text/parameter")) {
        auto list = split(data->data("text/parameter").data(), '\n');
        auto cnv_template = parameter_view_wrapper_->currentConversionTemplate();

        for (const auto &name : list) {
            if (cnv_template) {
                auto parameter = (Parameter_ifs *)cnv_template->getParameter(name);

                if (parameter && !parameters_.contains(parameter)) {
                    auto path = firstCharPos(parameter->getProperty("common/path")->getValue().asString(), '/');

                    if (device_->isChannelAvailable(path.second)) {
                        parameters_.push_back(parameter);
                        ret = true;
                    }
                }
            }
        }
    }
    if (ret) layoutChanged();
    return ret;
}