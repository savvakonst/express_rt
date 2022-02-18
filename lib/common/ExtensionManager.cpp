
//
// Created by SVK on 31.10.2021.
//
#include <windows.h>

#include <clocale>
///
#include <process.h>
#include <psapi.h>

#include <filesystem>
#include <iostream>

#include "common/ExtensionManager.h"

namespace fs = std::filesystem;

std::string getExecutePath() {
    HANDLE process_handle = NULL;
    TCHAR file_name[MAX_PATH];
    std::string ret;
    process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, _getpid());
    if (process_handle != NULL) {
        if (GetModuleFileNameEx(process_handle, NULL, file_name, MAX_PATH) == 0) {
            DEBUG_CERR("Failed to get module file_name." << std::endl);

        } else {
            DEBUG_COUT("Module file_name is: " << std::filesystem::path(std::string(file_name)).parent_path().string()
                                               << std::endl);

            ret = std::filesystem::path(std::string(file_name)).parent_path().string();
        }
        CloseHandle(process_handle);
    } else {
        DEBUG_CERR("Failed to open process." << std::endl);
    }

    return ret;
}

std::list<std::string> searchForSharedLibraries(const std::string &path) {
    std::string ext(".dll");
    std::list<std::string> ret;

    if (path.empty()) return ret;

    for (auto &p : fs::recursive_directory_iterator(path)) {
        if (p.path().extension() == ext) {
            ret.push_back(p.path().stem().string());
        }
    }
    return ret;
}

HINSTANCE tryToLinkSharedLibrary(ExtensionManager *em, const std::string &path, const std::string &module_name) {
    ExtensionInfo *extension_info = nullptr;

    bool f_run_time_link_success = FALSE;

    const std::string library_name = path + "\\" + module_name + ".dll";
    const std::string init_function_name = "init" + module_name;

    HINSTANCE hinstance = LoadLibrary(TEXT(library_name.c_str()));

    // If the handle is valid, try to get the function address.
    if (hinstance != nullptr) {
        DEBUG_COUT("library loaded \n");
        ExtensionInitFunction initFunction =
            (ExtensionInitFunction)GetProcAddress(hinstance, init_function_name.c_str());

        // If the function address is valid, call the function.
        if (nullptr != initFunction) {
            DEBUG_COUT("getting initFunction success\n");
            f_run_time_link_success = true;
            extension_info = initFunction();
        }
        // Free the DLL module.
    }

    if (extension_info) {
        em->insertExtensionInfo(extension_info);
    } else {
        bool f_free_result = FreeLibrary(hinstance);
        hinstance = nullptr;
    }
    return hinstance;
}

ExtensionManager::ExtensionManager(bool init) {
    std::list<HINSTANCE> *hinstance_list = new std::list<HINSTANCE>;
    resource_ = (extensionResource_t)hinstance_list;

    std::string path = getExecutePath();
    auto shared_libs_list = searchForSharedLibraries(path);

    DEBUG_COUT("----------------------------------------------------------\n");

    for (auto i : shared_libs_list) {
        DEBUG_COUT("--------------------------\n" << i << "\n");
        HINSTANCE hinstance = tryToLinkSharedLibrary(this, path, i);
        if (hinstance != nullptr) hinstance_list->push_back(hinstance);
        DEBUG_COUT("\n");
    }

    if (init) {
        this->init();
    }
}

void ExtensionManager::init() {
    DEBUG_COUT("\n\n------------------running init modules---------------------\n");
    auto init_set = getLastVersionExtensionUnitsByType("init");
    for (auto i : init_set) ((initUnit_t)i->ptr)(this);
}

ExtensionManager::~ExtensionManager() {
    std::list<HINSTANCE> *hinstance_list = (std::list<HINSTANCE> *)resource_;
    bool f_free_result;

    DEBUG_COUT("\n\n------------------free-libraries---------------------\n");
    for (auto hinstance : *hinstance_list)
        if (hinstance != nullptr) {
            // Free the DLL module.
            f_free_result = FreeLibrary(hinstance);
            DEBUG_COUT("Free the DLL module: " << f_free_result << "\n");
        }
    delete hinstance_list;
    resource_ = nullptr;
}
