
#include <QApplication>
#include <QTranslator>
#include <fstream>
//
#include "baseio/DefaultBaseIO.h"
#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QTranslator my_translator;
    my_translator.load("dictionary_ru");
    app.installTranslator(&my_translator);

    ExtensionManager manager;
    auto ctm = (ConversionTemplateManager *)manager
                   .getLastVersionExtensionUint("cnv_template_manager", "cnv_template_manager")
                   ->ptr;
    {
        auto e_unit = manager.getLastVersionExtensionUint("io", "base_io");
        if (e_unit == nullptr) {
            DEBUG_CERR("cant find \"base_io\" unit with \"io\" type\n");
            return 1;
        }
        auto *base_io = (IO_ifs *)(e_unit->ptr);
        if (base_io->readDocument(&manager, "analog.base"))
            std::cerr << "error (base_io ): " << base_io->getErrorMessage();
    }
    std::cout << "---------------------------------------------------------------------\n";

    {
        auto conv_template = new ConversionTemplate(&manager);
        conv_template->setName("first");
        conv_template->addInfo("source", Value("C:/another/virtual/path"));
        conv_template->addInfo("company", Value("АО ничего"));
        ctm->addConversionTemplate(conv_template);
    }
    {
        auto conv_template = new ConversionTemplate(&manager);
        conv_template->setName("second");
        conv_template->addInfo("source", Value("/virtual"));
        conv_template->addInfo("company", Value("АО Элистех"));
        ctm->addConversionTemplate(conv_template);
    }

    MainWindow main_win(&manager);
    main_win.show();
    return QApplication::exec();
}
