
#include <QApplication>
#include <fstream>
//
#include "baseio/DefaultBaseIO.h"
#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    auto ctm = ConversionTemplateManager();
    ExtensionManager manager(false);
    manager.insertExtensionUnit(
        new ExtensionUnit{"cnv_template_manager", "cnv_template_manager", "pass", (void *)&ctm, 0x00});
    manager.init();

    {
        std::string contents;
        {
            std::fstream in("analog.base", std::ios_base::in | std::ios::binary);
            if (!in) return 1;

            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
        }

        auto e_unit = manager.getLastVersionExtensionUint("io", "base_io");
        if (e_unit == nullptr) {
            DEBUG_CERR("cant find \"base_io\" unit with \"io\" type\n");
            return 1;
        }
        auto *base_io = (DefaultBaseIO *)(e_unit->ptr);

        auto conv_template = base_io->parseDocument(&manager, contents);
        if (conv_template) ctm.addConversionTemplate(conv_template);
        
        if (base_io->getErrorMessage().size()) std::cerr << base_io->getErrorMessage();
    }
    std::cout << "---------------------------------------------------------------------\n";

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

    MainWindow main_win(&manager);
    main_win.show();
    return QApplication::exec();
}
