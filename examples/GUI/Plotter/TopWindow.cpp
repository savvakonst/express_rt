//#include "chip.h"
#define _USE_MATH_DEFINES
#include "Receiver/Receiver.h"
//
#include <qgraphicsscene.h>

#include <QFrame>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QTimer>

#include "Ping/ksdconnected.h"
#include "ReaderExample/ReaderExample.h"
#include "TopWindow.h"
#include "common/Extension.h"
#include "device/Device_ifs.h"

InitExtension(ExtensionInfo *) initModules(void);

#include <iostream>

#include "common/ExtensionManager.h"
/*
 *
 *
 *
 */

TopWindow::TopWindow(QWidget *parent, double signal_frequency)
    : signal_frequency_(signal_frequency / (int64_t(1) << 32)) {
    sample_frequency_log2_ = 15;

    initScene();

    setWindowTitle(tr("plot example"));
    setCentralWidget(view_);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TopWindow::redraw);

    redraw();
    timer->start(5);
}

TopWindow::~TopWindow() { delete manager_; }

void TopWindow::initScene() {
    manager_ = new ExtensionManager;

    std::string error_msg;

    EthernetAddress addr;
    addr.ip = inet_addr("192.168.001.176");
    addr.port = 0;

    auto devices = devicePing(addr, error_msg);

    if (!error_msg.empty()) {
        std::cout << error_msg;
        return;
    }
    auto v = devices.front()->getTask();

    Device_ifs device = Device_ifs(v.data(), v.size(), manager_);

    if (device.hasTransceiver()) {
        ModuleStream_ifs *m_stream = device.getTopModule()->createModuleStream();
        auto *receiver = new Receiver(m_stream, device.getSrcAddress());

        auto prm_buff = device.getTopModule()->getPrmBufferMap();

        for (const auto &i : prm_buff) {
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

void TopWindow::draw(QPainter &paint, Reader_ifs::Chunk *c_ptr, size_t offset) const {
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

void TopWindow::redraw() {
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

    TopWindow *p = ((TopWindow *)parent());
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
    TopWindow *p = ((TopWindow *)parent());
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
