
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

void MainWindow::newLetter() {
    text_edit_->clear();

    QTextCursor cursor(text_edit_->textCursor());
    cursor.movePosition(QTextCursor::Start);
    QTextFrame *topFrame = cursor.currentFrame();
    QTextFrameFormat topFrameFormat = topFrame->frameFormat();
    topFrameFormat.setPadding(16);
    topFrame->setFrameFormat(topFrameFormat);

    QTextCharFormat textFormat;
    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);
    QTextCharFormat italicFormat;
    italicFormat.setFontItalic(true);

    QTextTableFormat tableFormat;
    tableFormat.setBorder(1);
    tableFormat.setCellPadding(16);
    tableFormat.setAlignment(Qt::AlignRight);
    cursor.insertTable(1, 1, tableFormat);
    cursor.insertText("The Firm", boldFormat);
    cursor.insertBlock();
    cursor.insertText("321 City Street", textFormat);
    cursor.insertBlock();
    cursor.insertText("Industry Park");
    cursor.insertBlock();
    cursor.insertText("Some Country");
    cursor.setPosition(topFrame->lastPosition());
    cursor.insertText(QDate::currentDate().toString("d MMMM yyyy"), textFormat);
    cursor.insertBlock();
    cursor.insertBlock();
    cursor.insertText("Dear ", textFormat);
    cursor.insertText("NAME", italicFormat);
    cursor.insertText(",", textFormat);
    for (int i = 0; i < 3; ++i) cursor.insertBlock();
    cursor.insertText(tr("Yours sincerely,"), textFormat);
    for (int i = 0; i < 3; ++i) cursor.insertBlock();
    cursor.insertText("The Boss", textFormat);
    cursor.insertBlock();
    cursor.insertText("ADDRESS", italicFormat);
}

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

#include "common/ExtensionManager.h"
#include "iostream"
void MainWindow::createDockWindows() {
    QDockWidget *dock = nullptr;

    /*
     *
     */

    dock = new QDockWidget(tr("list of conversion templates"), this);

    table_view_ = (QTableView *)ctm_->getLastVersionExtensionUint("widget", "conv_template_list")->ptr;
    /*
        table_view_ = new QTableView(dock);

        QHeaderView *vertical_header = table_view_->verticalHeader();

        vertical_header->setDefaultSectionSize(vertical_header->minimumSectionSize());  // 20 px height
        vertical_header->sectionResizeMode(QHeaderView::Fixed);
        vertical_header->hide();

        table_view_->setSelectionBehavior(QAbstractItemView::SelectRows);
        table_view_->setAlternatingRowColors(true);

        // table_view_->setStyleSheet(
        //     "QTableView::item:alternate { background-color: #f6fafb; } QTableView::item { background-color: #D2DCDF;
       }"); table_view_->setStyleSheet( "QHeaderView::section {background-color: #D2DCDF; alternate-background-color:
       #f6fafb;};"); table_view_->setModel(new ConvTempateTableModel(ctm_));
    */

    dock->setWidget(table_view_);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    /*
     *
     */
}
//! [9]
