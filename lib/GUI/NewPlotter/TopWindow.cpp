//
// Created by SVK on 27.03.2022.
//
#include "Receiver/Receiver.h"
//
#include <QBoxLayout>
#include <QCoreApplication>
#include <QDebug>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QInputDialog>
#include <QMimeData>
#include <QTreeView>
///
#include "GUI/CustomQActions.h"
#include "GUI/WidgetWrappers.h"
#include "TopWindow.h"
#include "common/StringProcessingTools.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/Parameter_ifs.h"
#include "convtemplate/PrmBuffer_ifs.h"
#include "device/DeviceManager.h"

//
#include "qformscreen.h"

TopWindow::TopWindow(QWidget *parent) {
    setAcceptDrops(true);
    setUnifiedTitleAndToolBarOnMac(true);
}

void TopWindow::init(ExtensionManager *manager) {
    manager_ = manager;
    device_view_wrapper_ =
        (DeviceViewWrapper_ifs *)manager->getLastVersionExtensionObject("widget_wrapper", "device_view_wrapper");
    device_manager_ = (DeviceManager *)manager->getLastVersionExtensionObject("device_manager", "device_manager");
}

void TopWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (block_) {
        e->ignore();
        return;
    }
    if (e->mimeData()->hasFormat("text/module")) {
        auto list = split(e->mimeData()->data("text/device").data(), '\n');
        if (list.size() == 1) e->acceptProposedAction();
    }
}

void TopWindow::dropEvent(QDropEvent *e) {
    if (e->mimeData()->hasFormat("text/device")) {
        // if (block_) return;

        auto list = split(e->mimeData()->data("text/device").data(), '\n');

        if (list.size() > 1) return;
        const auto &i = list.front();

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
            auto pre_plotter = new ParametersToPlot(this, device, manager_);
            connect(pre_plotter, &ParametersToPlot::toRemove, this, &TopWindow::onRemoveDocWidget);

            dock->setAttribute(Qt::WA_DeleteOnClose);
            dock->setWidget(pre_plotter);
            addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dock, Qt::Orientation::Horizontal);
            device_view_wrapper_->setActive(device->getSource(), "");
            block_ = true;
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

void TopWindow::addPlotter(const std::string &name, QFormScreen *form_screen) {
    auto dock = new QDockWidget(name.data(), this);
    dock->setAttribute(Qt::WA_DeleteOnClose);
    dock->setWidget(form_screen);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dock, Qt::Orientation::Horizontal);
}

/*
 *
 *
 *
 */

ModuleStream_ifs *generateStream(ExtensionManager *manager, const std::string &type, Device_ifs *device,
                                 const std::list<Parameter_ifs *> &parameters) {
    ModuleStream_ifs *top_m_stream = device->createModuleStream();

    for (auto prm : parameters) {
        auto constructor = (prmBufferConstructor_f)manager->getLastVersionExtensionObject(type, prm->getType());
        if (constructor) {
            auto full_path = prm->getProperty("common/path")->getValue().asString();

            auto path = lastCharPos(full_path, '/');
            auto sub_modules = ::getSubmodules(device, path.first);
            if (sub_modules.size() == 1) {
                auto module = sub_modules.front()->getModuleStream();

                if (module) {
                    auto prm_buffer = constructor(module->getModule(), prm, manager);
                    top_m_stream->addPrmBuffer(path.second, prm_buffer);
                }
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

const RelativeTime t_{30, 0};
class Timer : public QTimer {
   public:
    Timer(QFormScreen *q_form_screen, ModuleStream_ifs *redused_module_stream)
        : q_form_screen_(q_form_screen), redused_module_stream_(redused_module_stream) {
        QObject::connect(this, &Timer::timeout, this, &Timer::onTimer);
        setInterval(static_cast<int>(125));
        start(static_cast<int>(0));
    }

   public slots:
    void onTimer() { q_form_screen_->onRefresh(redused_module_stream_->getTime()); };

   private:
    ModuleStream_ifs *redused_module_stream_;
    QFormScreen *q_form_screen_;
};

class OnlinePlotterContext : public PlotterContext_ifs {
   public:
    explicit OnlinePlotterContext(Receiver *receiver, ModuleStream_ifs *module_stream)
        : receiver_(receiver), module_stream_(module_stream) {}
    ~OnlinePlotterContext() override {
        qDebug() << "~OnlinePlotterContext()";
        if (timer_) timer_->stop();
        delete timer_;
        delete receiver_;
        delete module_stream_;
    };

    void setPlotter(QFormScreen *q_form_screen) {
        timer_ = new Timer(q_form_screen, module_stream_->reduce());
        start();
    }

    bool start() override {
        receiver_->start();
        return true;
    }

    bool stop() override {
        receiver_->stop();
        return true;
    }

   private:
    Timer *timer_ = nullptr;
    ModuleStream_ifs *module_stream_;
    Receiver *receiver_;
};

/*
 *
 *
 *
 *
 */

ParametersToPlot::ParametersToPlot(TopWindow *plotter_main_window, Device_ifs *device, ExtensionManager *manager,
                                   QWidget *parent)
    : plotter_main_window_(plotter_main_window), device_(device), manager_(manager), QWidget(parent) {
    tree_view_ = new QTreeView();
    model_ = new ParameterBufferModel(device, manager);

    tree_view_->setParent(this);
    model_->setParent(this);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(tree_view_);

    QIcon run_icon;
    run_icon.addPixmap(getPixmap("/png/common/play.png"), QIcon::Normal, QIcon::Off);
    run_icon.addPixmap(getPixmap("/png/common/stop.png"), QIcon::Normal, QIcon::On);

    auto *action_run = new QAction(run_icon, QObject::tr("Ru&n"), this);
    connect(action_run, &QAction::triggered, this, &ParametersToPlot::runAndClose);

    tree_view_->setDragDropMode(QAbstractItemView::DragDrop);
    tree_view_->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
    tree_view_->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree_view_->setDragEnabled(true);
    tree_view_->setDropIndicatorShown(true);
    tree_view_->setAcceptDrops(true);
    tree_view_->setModel(model_);

    auto group_action = new FuncProxyQAction(
        [&]() {
            auto group_name = QInputDialog::getText(tree_view_, "Имя группы", "group");

            auto index_list = tree_view_->selectionModel()->selectedRows();
            // for (auto i : index_list)
            //     if (i.row() == 0)
            //         model_->
            //
            //             return true;

            // NameDialog(tree_view_).show();
            //  name_dialog.show();
            return true;
        },
        tree_view_);

    group_action->setShortcutContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
    group_action->setText("&Создать группу");
    tree_view_->addAction(group_action);

    auto p_layout = reinterpret_cast<QHBoxLayout *>(this->layout());
    auto toolbar = new QToolBar(this);
    toolbar->addAction(action_run);
    p_layout->insertWidget(0, toolbar);
}

ParametersToPlot::~ParametersToPlot() {
    qDebug() << "ParametersToPlot::~ParametersToPlot()";
    plotter_main_window_->block_ = false;
};

void ParametersToPlot::runAndClose() {
    auto list = model_->getParameters().toStdList();
    // auto module_stream = generateStream(manager_, "dynamic_prm_buffer", device_, list);

    ModuleStream_ifs *top_m_stream = device_->createModuleStream();

    auto receiver = new Receiver(top_m_stream->reduce(), device_->getSrcAddress());
    auto plotter_context = new OnlinePlotterContext(receiver, top_m_stream);

    auto form_screen = new QFormScreen(manager_, plotter_context);

    for (auto prm : list) {
        auto constructor =
            (prmBufferConstructor_f)manager_->getLastVersionExtensionObject("dynamic_prm_buffer", prm->getType());
        if (constructor) {
            auto full_path = prm->getProperty("common/path")->getValue().asString();

            auto path = lastCharPos(full_path, '/');
            auto sub_modules = ::getSubmodules(device_, path.first);
            if (sub_modules.size() == 1) {
                auto module_stream = sub_modules.front()->getModuleStream();

                if (module_stream) {
                    auto prm_buffer = constructor(module_stream->getModule(), prm, manager_);
                    module_stream->addPrmBuffer(path.second, prm_buffer);
                    form_screen->addScale(prm_buffer->createReader());
                }
            }
        }
    }

    plotter_main_window_->addPlotter(device_->getSource() + "//", form_screen);
    plotter_context->setPlotter(form_screen);
    emit toRemove(this);
}

/*
 *
 *
 *
 */

ParameterBufferModel::ParameterBufferModel(Device_ifs *device, ExtensionManager *manager) : device_(device) {
    parameter_view_wrapper_ =
        (ParameterViewWrapper_ifs *)manager->getLastVersionExtensionObject("widget_wrapper", "parameter_view_wrapper");

    auto schema = (DataSchema_ifs *)manager->getLastVersionExtensionObject("data_schema", "common");
    if (schema) list_of_entries_ = schema->getMapList();
    else
        std::cerr << "can't find data_schema;\n";

    root_node_ = new TreeNode("");
}

QVariant ParameterBufferModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) return QString(list_of_entries_[section]->description_.data());
    return {};
}

QVariant ParameterBufferModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        auto node = getNode(index);
        auto name = list_of_entries_[index.column()]->name_;
        // auto prm = parameters_[index.row()];
        // auto h_data = prm->getProperty("common/" + name);
        // return h_data->getValue().asString().data();
        return node->getData(name);
    }
    return {};
}

Qt::ItemFlags ParameterBufferModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (index.isValid()) flags |= Qt::ItemNeverHasChildren;

    if (index.isValid()) return flags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    else
        return flags | Qt::ItemIsDropEnabled;
}

bool ParameterBufferModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                           const QModelIndex &parent) const {
    if (data->hasFormat("text/parameter")) {
        auto list = split(data->data("text/parameter").data(), '\n');
        if (list.size() > 1) return true;

        auto name = list.front();

        auto cnv_template = parameter_view_wrapper_->currentConversionTemplate();

        if (cnv_template) {
            auto parameter = (Parameter_ifs *)cnv_template->getParameter(name);
            if (parameter && !root_node_->contains(parameter)) {
                auto path = firstCharPos(parameter->getProperty("common/path")->getValue().asString(), '/');
                if (device_->isChannelAvailable(path.second)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool ParameterBufferModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                        const QModelIndex &parent) {
    auto ret = false;
    if (data->hasFormat("text/parameter")) {
        auto list = split(data->data("text/parameter").data(), '\n');
        auto cnv_template = parameter_view_wrapper_->currentConversionTemplate();

        for (const auto &name : list) {
            if (cnv_template) {
                auto parameter = (Parameter_ifs *)cnv_template->getParameter(name);

                if (parameter && !root_node_->contains(parameter)) {
                    auto path = firstCharPos(parameter->getProperty("common/path")->getValue().asString(), '/');

                    if (device_->isChannelAvailable(path.second)) {
                        root_node_->addSubNode(parameter);

                        ret = true;
                    }
                }
            }
        }
    }
    if (ret) layoutChanged();
    return ret;
}
bool ParameterBufferModel::moveRows(const QModelIndex &source_parent, int source_row, int count,
                                    const QModelIndex &destination_parent, int destination_child) {
    qDebug() << "ParameterBufferModel::moveRows: " << source_parent << ", " << source_row;
    return true;
}

QString ParameterBufferModel::TreeNode::getData(const std::string &name) {
    if (isParameter()) {
        auto h_data = parameter_->getProperty("common/" + name);
        return h_data->getValue().asString().data();
    } else if (name != "name")
        return "";
    else
        return data_;
}
