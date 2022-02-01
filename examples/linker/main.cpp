//
// Created by SVK on 31.10.2021.
//
#include <clocale>
#include <filesystem>
#include <fstream>

#include "baseio/DefaultBaseIO.h"
#include "common/ExtensionManager.h"

int main(void) {
    std::setlocale(LC_ALL, "en_US.UTF-8");

    ExtensionManager manager;

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

    base_io->parseDocument(&manager, contents);

    std::cout << base_io->getErrorMessage();

    return 0;
}