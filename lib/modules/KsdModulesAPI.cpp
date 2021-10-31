//
// Created by SVK on 31.10.2021.
//

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
#include "Module_ID02.h"
#include "Module_M01_.h"
#include "common/Extension.h"

static ExtensionUint g_modules_extension_uint[] = {
    {"M01_", "module", "M01_ ksd module", (void*)createModule<Module_M01_>},
    {"A01_", "module", "A01_ ksd module", (void*)createModule<Module_A01_>},
    {"A02_", "module", "A02_ ksd module", (void*)createModule<Module_A02_>},
    {"A03_", "module", "A03_ ksd module", (void*)createModule<Module_A03_>},
    {"A04_", "module", "A04_ ksd module", (void*)createModule<Module_A04_>},
    {"A05_", "module", "A05_ ksd module", (void*)createModule<Module_A05_>},
    {"A06_", "module", "A06_ ksd module", (void*)createModule<Module_A06_>},
    {"A07_", "module", "A07_ ksd module", (void*)createModule<Module_A07_>},
    {"A08_", "module", "A08_ ksd module", (void*)createModule<Module_A08_>},
    {"A09_", "module", "A09_ ksd module", (void*)createModule<Module_A09_>},
    {"A10_", "module", "A10_ ksd module", (void*)createModule<Module_A10_>},
    {"A11_", "module", "A11_ ksd module", (void*)createModule<Module_A11_>},
    {"D01_", "module", "D01_ ksd module", (void*)createModule<Module_D01_>},
    {"D02_", "module", "D02_ ksd module", (void*)createModule<Module_D02_>},
    {"ID02", "module", "ID02 ksd module", (void*)createModule<Module_ID02>},
    {"C06_", "module", "C06_ ksd module", (void*)createModule<Module_C06_>},
    {"C06M", "module", "C06M ksd module", (void*)createModule<Module_C06M>},
    {"C03_", "module", "C03_ ksd module", (void*)createModule<Module_C03_>},
    {"C04_", "module", "C04_ ksd module", (void*)createModule<Module_C04_>},
    nullptr};

static ExtensionInfo g_modules_extension_info = {"Modules_extensiln", 0x01, g_modules_extension_uint};

InitExtension(ExtensionInfo*) init_Modules(void) { return &g_modules_extension_info; }