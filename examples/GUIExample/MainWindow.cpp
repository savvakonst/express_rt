//#include "chip.h"
#define _USE_MATH_DEFINES

#include "MainWindow.h"

#include <qgraphicsscene.h>
#include <winsock2.h>

#include <QFrame>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QTimer>

#include "Device/Device.h"
#include "Ping/ksdconnected.h"
#include "ReaderExample/ReaderExample.h"
#include "Receiver/Receiver.h"
#include "common/extension.h"

bool initWSA();

class DeviceBuildingContext : public DeviceBuildingContext_ifs {
   public:
    ~DeviceBuildingContext() {}

    bool addUint(ExtensionUnit *uint) {
        if (std::string(uint->type) == "module") map_[uint->name] = (moduleConstructor_f)uint->ptr;
        return true;
    }

    moduleConstructor_f getConstructor(const std::string &module_id) override {
        auto e = map_.find(module_id);
        if (e == map_.end()) {
            return nullptr;
        } else {
            return e->second;
        }
    }

    Module_ifs *createModule(const std::string &module_id, const void *ptr, size_t size,
                             DeviceBuildingContext_ifs *context) override {
        moduleConstructor_f constructor = context->getConstructor(module_id);
        if (constructor) return constructor(ptr, size, context);
        return nullptr;
    }

   private:
    std::map<std::string, moduleConstructor_f> map_;
};
InitExtension(ExtensionInfo *) initModules(void);

#include <iostream>

/*
 *
 *
 *
 */

MainWindow::MainWindow(QWidget *parent, double signal_frequency)
    : signal_frequency_(signal_frequency / (int64_t(1) << 32)) {
    sample_frequency_log2_ = 15;

    initScene();

    setWindowTitle(tr("plot example"));
    setCentralWidget(view_);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::redraw);

    redraw();
    timer->start(5);
}

MainWindow::~MainWindow() {}

void MainWindow::initScene() {
    initWSA();

    ExtensionInfo *extension_info = initModules();
    auto uints = extension_info->units;

    DeviceBuildingContext building_context;

    while (uints->name) {
        building_context.addUint(uints++);
    }

    std::string error_msg;
    auto devices = devicePing(error_msg);

    if (error_msg.size()) {
        std::cout << error_msg;
        return;
    }
    auto v = devices.front()->getTask();

    Device device = Device(v.data(), v.size(), &building_context);

    if (device.hasTransceiver()) {
        ModuleStream_ifs *m_stream = device.getTopModule()->createModuleStream();
        Receiver *receiver = new Receiver(m_stream, device.getSrcAddress());

        auto prm_buff = device.getTopModule()->getPrmBufferMap();

        for (auto i : prm_buff) {
            reader_ = i.second;
            break;
        }
        receiver->start();
    }

    // reader_ = createNewPrmBufferSimulator(sample_frequency_log2_);

    buffer_1_ = new Reader_ifs::Point[buffer_size_];
    buffer_2_ = new Reader_ifs::Point[buffer_size_];

    view_1_ = getReaderExample(reader_);
    view_2_ = getReaderExample(reader_);

    this->scene_ = new GraphicsScene(this);
    view_ = new QGraphicsView;
    scene_->setSceneRect(0, 0, buffer_size_, hight_);

    view_->setScene(scene_);
    view_->show();
}

void MainWindow::draw(QPainter &paint, Reader_ifs::Chunk *c_ptr, size_t offset) {
    paint.setPen(QColor(0, 100, 100));
    while (c_ptr) {
        Reader_ifs::Point *pb = c_ptr->first_point_;

        for (size_t i = 0; i < c_ptr->number_of_points_; i++)
            paint.drawLine((int)pb[i].pos, pb[i].max * scale_h_ + offset, (int)pb[i].pos,
                           pb[i].min * scale_h_ + offset);
        c_ptr = c_ptr->next_;
    }
    paint.setPen(QColor(100, 100, 0));
    paint.drawLine(0, (int)offset, (int)buffer_size_, (int)offset);
}

void MainWindow::redraw() {
    auto borders_1 = view_1_->getAvailableBorders();
    auto borders_2 = Borders{borders_1.begin + time_offset_, time_interval_};

    auto chunk_1 = view_1_->getPoints(borders_1, buffer_1_, buffer_size_);
    auto chunk_2 = view_2_->getPoints(borders_2, buffer_2_, buffer_size_);

    QPixmap pixmap((int)buffer_size_, (int)hight_);
    pixmap.fill(QColor(250, 250, 230));
    QPainter paint(&pixmap);

    auto offset = scale_h_ + scale_h_ / 2;

    draw(paint, chunk_1.get(), offset);
    draw(paint, chunk_2.get(), offset + half_h_);

    scene_->setBackgroundBrush(pixmap);
}

void GraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *event) {
    QGraphicsScene::wheelEvent(event);

    MainWindow *p = ((MainWindow *)parent());
    int64_t s = 100, ds = 5;
    auto delta = event->delta();

    if (delta > 0) p->time_interval_.time = (p->time_interval_.time * s) / (s + ds);
    if (delta < 0) p->time_interval_.time = (p->time_interval_.time * (s + ds)) / s;

    // std::cout << "ls_ineger: " << p->time_interval_.ls_ineger << ", ms_fractional: " <<
    // p->time_interval_.ms_fractional
    //          << "\n";
}

void GraphicsScene::keyPressEvent(QKeyEvent *event) {
    QGraphicsScene::keyPressEvent(event);
    MainWindow *p = ((MainWindow *)parent());
    if (Qt::Key::Key_Minus == event->key()) {
        if (p->time_interval_ > RelativeTime{0, 1}) {
            p->time_interval_ = p->time_interval_ - RelativeTime{0, 1};
        }
    } else if (Qt::Key::Key_Equal == event->key()) {
        p->time_interval_ = p->time_interval_ + RelativeTime{0, 1};
    } else if (Qt::Key::Key_Left == event->key()) {
        p->time_offset_ = p->time_offset_ - RelativeTime{0, 1};
    } else if (Qt::Key::Key_Right == event->key()) {
        p->time_offset_ = p->time_offset_ + RelativeTime{0, 1};
    }

    p->redraw();
}
