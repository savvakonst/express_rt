//
// Created by SVK on 31.10.2021.
//

#include <windows.h>

#include <iostream>
#include <string>
#include <vector>

#include "Device/Module_ifs.h"
#include "common/BaseClass_ifs.h"
#include "common/Extension.h"


class ExtensionManager {
public:
    std::vector<std::string> getAvalibleExtensionTypes(std::string type) { return {}; }

    std::vector<std::string> getExtensionsByType(std::string type) { return {}; }

    ExtensionUint *getLastVersionExtensionUint(std::string name, std::string type) { return nullptr; }

    Module_ifs *getModuleExtensionUint() { return nullptr; }

    Module_ifs *getParameterExtensionUint() { return nullptr; }

};

int main(void) {
    const std::string module_name = "Modules";

    ExtensionInfo *extension_info = nullptr;
    ExtensionInitFunction initFunction = nullptr;
    HINSTANCE hinst_lib;

    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

    const std::string library_name = module_name + ".dll";
    const std::string init_function_name = "init" + module_name;

    // ExtensionInitFunction

    // Get a handle to the DLL module.
    hinst_lib = LoadLibrary(TEXT(library_name.c_str()));

    // If the handle is valid, try to get the function address.

    if (hinst_lib != nullptr) {
        initFunction = (ExtensionInitFunction) GetProcAddress(hinst_lib, init_function_name.c_str());

        // If the function address is valid, call the function.
        if (nullptr != initFunction) {
            std::cout << "getting initFunction success\n";
            fRunTimeLinkSuccess = TRUE;
            extension_info = initFunction();
        }
        // Free the DLL module.
    }

    if (extension_info) {
        std::cout << "run initFunction success\n";
        for (ExtensionUint *uint = extension_info->uints; uint->name != nullptr; uint++) {
            std::cout << "name: " << uint->name << ", type: " << uint->type << ", description: " << uint->description
                      << "\n";
        }
    }

    if (hinst_lib != nullptr)  // Free the DLL module.
        fFreeResult = FreeLibrary(hinst_lib);

    return 0;
}