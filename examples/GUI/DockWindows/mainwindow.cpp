
#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#    include <QtPrintSupport/qtprintsupportglobal.h>
#    if QT_CONFIG(printdialog)
#        include <QtPrintSupport>
#    endif
#endif

#include "mainwindow.h"

MainWindow::MainWindow(ExtensionManager *ctm) : text_edit_(new QTextEdit), ctm_(ctm) {
    setCentralWidget(text_edit_);

    createActions();
    createStatusBar();
    createDockWindows();

    setWindowTitle(tr("Dock Widgets"));

    newLetter();
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::newLetter() { text_edit_->clear(); }

void MainWindow::print() {
#if QT_CONFIG(printdialog)
    QTextDocument *document = text_edit_->document();
    QPrinter printer;

    QPrintDialog dlg(&printer, this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    document->print(&printer);
    statusBar()->showMessage(tr("Ready"), 2000);
#endif
}

void MainWindow::save() {
    QMimeDatabase mimeDatabase;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Choose a file name"), ".",
                                                    mimeDatabase.mimeTypeForName("text/html").filterString());
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(
            this, tr("Dock Widgets"),
            tr("Cannot write file %1:\n%2.").arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << text_edit_->toHtml();
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
}

void MainWindow::undo() {
    QTextDocument *document = text_edit_->document();
    document->undo();
}

void MainWindow::insertCustomer(const QString &customer) {
    if (customer.isEmpty()) return;
    QStringList customerList = customer.split(", ");
    QTextDocument *document = text_edit_->document();
    QTextCursor cursor = document->find("NAME");
    if (!cursor.isNull()) {
        cursor.beginEditBlock();
        cursor.insertText(customerList.at(0));
        QTextCursor oldcursor = cursor;
        cursor = document->find("ADDRESS");
        if (!cursor.isNull()) {
            for (int i = 1; i < customerList.size(); ++i) {
                cursor.insertBlock();
                cursor.insertText(customerList.at(i));
            }
            cursor.endEditBlock();
        } else
            oldcursor.endEditBlock();
    }
}

void MainWindow::addParagraph(const QString &paragraph) {
    if (paragraph.isEmpty()) return;
    QTextDocument *document = text_edit_->document();
    QTextCursor cursor = document->find(tr("Yours sincerely,"));
    if (cursor.isNull()) return;
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, 2);
    cursor.insertBlock();
    cursor.insertText(paragraph);
    cursor.insertBlock();
    cursor.endEditBlock();
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Dock Widgets"),
                       tr("The <b>Dock Widgets</b> example demonstrates how to "
                          "use Qt's dock widgets. You can enter your own text, "
                          "click a customer to add a customer name and "
                          "address, and click standard paragraphs to add them."));
}

void MainWindow::createActions() {}

void MainWindow::createStatusBar() {}

#include "GUI/TreeEditor.h"
#include "common/ExtensionManager.h"
#include "iostream"
void MainWindow::createDockWindows() {
    QDockWidget *dock = nullptr;

    /*
     *
     */

    dock = new QDockWidget(tr("parameter properties"), this);

    auto constructor = (newTreeEditor_t)ctm_->getLastVersionExtensionUint("tree_editor", "tree_editor")->ptr;
    auto ds = (DataSchema_ifs *)ctm_->getLastVersionExtensionUint("data_schema", "ethernet_udp")->ptr;

    TreeEditor *top = constructor(ctm_, nullptr);
    top->setupProperties(ds);

    dock->setWidget(top);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    this->resize(750, 600 * 2);
    top->setMinimumHeight(600);
    top->setMinimumWidth(400);
    top->setColumnWidth(0, 250);
    // dock->swtMinimumSizeHint(250, 900);
    /*
     *
     */

    auto units = ctm_->getLastVersionExtensionUintsByType("widget");
    for (auto i : units) {
        dock = new QDockWidget(tr(i->name), this);
        auto widget = (QWidget *)i->ptr;  // ctm_->getLastVersionExtensionUint("widget", "conv_template_list")->ptr;
        dock->setWidget(widget);
        addDockWidget(Qt::BottomDockWidgetArea, dock);
    }
    /*
     *
     */
}
//! [9]
