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
class Device;
class DataSchema_ifs;
class ParameterViewWrapper_ifs;
class ParameterBufferTableModel;

class ParametersToPlot : public QWidget {
    Q_OBJECT
   public:
    explicit ParametersToPlot(TopWindow *plotter_main_window, Device *device, ExtensionManager *manager,
                              QWidget *parent = nullptr);
    ~ParametersToPlot() override;
   public slots:

    void runAndClose();

   private:
    TopWindow *plotter_main_window_ = nullptr;
    ParameterBufferTableModel *model_ = nullptr;
    ExtensionManager *manager_ = nullptr;
    Device *device_ = nullptr;

    QTimer *timer_ = nullptr;

   signals:
    void toRemove(QWidget *widget);
};

class ParameterBufferTableModel : public QAbstractTableModel {
    Q_OBJECT
   public:
    explicit ParameterBufferTableModel(Device *device, ExtensionManager *manager);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override { return parameters_.size(); }

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override { return (int)list_of_entries_.size(); }

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    //////////////
    [[nodiscard]] QStringList mimeTypes() const override {
        return {QAbstractItemModel::mimeTypes().at(0), "text/parameter"};
    }

    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                         const QModelIndex &parent) const override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                      const QModelIndex &parent) override;

    const QList<Parameter_ifs *> &getParameters() { return parameters_; }

   protected:
    std::vector<DataSchema_ifs *> list_of_entries_;

    QList<Parameter_ifs *> parameters_;

    Device *device_ = nullptr;
    DeviceViewWrapper_ifs *device_view_wrapper_ = nullptr;
    ParameterViewWrapper_ifs *parameter_view_wrapper_ = nullptr;
};

#endif  // EXRT_TOPWINDOW_H
