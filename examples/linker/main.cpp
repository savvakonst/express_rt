//
// Created by SVK on 31.10.2021.
//
#include <clocale>
#include <filesystem>

#include "common/ExtensionManager.h"

int main(void) {
    std::setlocale(LC_ALL, "en_US.UTF-8");

    ExtensionManager manager;

    return 0;
}