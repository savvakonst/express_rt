
#include <QApplication>

#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    auto ctm = ConversionTemplateManager();
    ExtensionManager manager;
    manager.insertExtensionUnit(
        new ExtensionUnit{"cnv_template_manager", "cnv_template_manager", "pass", (void *)&ctm, 0x00});

    {
        auto conv_template = new ConversionTemplate(&manager);
        conv_template->setName("first");
        conv_template->addInfo("source", Value("C:/another/virtual/path"));
        conv_template->addInfo("company", Value("АО ничего"));
        ctm.addConversionTemplate(conv_template);
    }
    {
        auto conv_template = new ConversionTemplate(&manager);
        conv_template->setName("second");
        conv_template->addInfo("source", Value("/virtual"));
        conv_template->addInfo("company", Value("АО Элистех"));
        ctm.addConversionTemplate(conv_template);
    }

    QApplication app(argc, argv);

    MainWindow main_win(&manager);
    main_win.show();
    return QApplication::exec();
}
