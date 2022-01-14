//
// Created by SVK on 31.10.2021.
//

#include <algorithm>
#include <iostream>

#include "Module_A01_.h"
#include "Module_A02_.h"
#include "Module_A03_.h"
#include "Module_A04_.h"
#include "Module_A05_.h"
#include "Module_A06_.h"
#include "Module_A07_.h"
#include "Module_A08_.h"
#include "Module_A09_.h"
#include "Module_A10_.h"
#include "Module_A11_.h"
#include "Module_C03_.h"
#include "Module_C04_.h"
#include "Module_C06M.h"
#include "Module_C06_.h"
#include "Module_D01_.h"
#include "Module_D02_.h"
#include "Module_DCU_.h"
#include "Module_ID02.h"
#include "Module_M01_.h"
#include "common/Extension.h"

/*
#define createDCU_Module(NAME)                                              \
    ([](const void *ptr, size_t size, DeviceBuildingContext_ifs *context) { \
        return new Module_DCU_((NAME), ptr, size, context);                 \
    })
*/

#define M_createDCU_Module(NAME)                                                                         \
    Module_ifs *create##NAME##Module(const void *ptr, size_t size, DeviceBuildingContext_ifs *context) { \
        return new Module_DCU_(#NAME, ptr, size, context);                                               \
    }

M_createDCU_Module(CH04);
M_createDCU_Module(CH06);
M_createDCU_Module(CH08);
M_createDCU_Module(CH10);

static ExtensionUint g_modules_extension_uint[] = {
    {"CH04", "module", "CH04 ksd module", (void *)&createCH04Module, 0x00},
    {"CH06", "module", "CH06 ksd module", (void *)&createCH06Module, 0x00},
    {"CH08", "module", "CH08 ksd module", (void *)&createCH08Module, 0x00},
    {"CH10", "module", "CH10 ksd module", (void *)createCH10Module, 0x00},
    {"M01_", "module", "M01_ ksd module", (void *)createModule<Module_M01_>, 0x00},
    {"A01_", "module", "A01_ ksd module", (void *)createModule<Module_A01_>, 0x00},
    {"A02_", "module", "A02_ ksd module", (void *)createModule<Module_A02_>, 0x00},
    {"A03_", "module", "A03_ ksd module", (void *)createModule<Module_A03_>, 0x00},
    {"A04_", "module", "A04_ ksd module", (void *)createModule<Module_A04_>, 0x00},
    {"A05_", "module", "A05_ ksd module", (void *)createModule<Module_A05_>, 0x00},
    {"A06_", "module", "A06_ ksd module", (void *)createModule<Module_A06_>, 0x00},
    {"A07_", "module", "A07_ ksd module", (void *)createModule<Module_A07_>, 0x00},
    {"A08_", "module", "A08_ ksd module", (void *)createModule<Module_A08_>, 0x00},
    {"A09_", "module", "A09_ ksd module", (void *)createModule<Module_A09_>, 0x00},
    {"A10_", "module", "A10_ ksd module", (void *)createModule<Module_A10_>, 0x00},
    {"A11_", "module", "A11_ ksd module", (void *)createModule<Module_A11_>, 0x00},
    {"D01_", "module", "D01_ ksd module", (void *)createModule<Module_D01_>, 0x00},
    {"D02_", "module", "D02_ ksd module", (void *)createModule<Module_D02_>, 0x00},
    {"ID02", "module", "ID02 ksd module", (void *)createModule<Module_ID02>, 0x00},
    {"C06_", "module", "C06_ ksd module", (void *)createModule<Module_C06_>, 0x00},
    {"C06M", "module", "C06M ksd module", (void *)createModule<Module_C06M>, 0x00},
    {"C03_", "module", "C03_ ksd module", (void *)createModule<Module_C03_>, 0x00},
    {"C04_", "module", "C04_ ksd module", (void *)createModule<Module_C04_>, 0x00},
    {nullptr, nullptr, nullptr, nullptr, 0}};

static ExtensionInfo g_modules_extension_info = {"Modules_extension", 0x01, g_modules_extension_uint};

InitExtension(ExtensionInfo *) initModules(void) { return &g_modules_extension_info; }