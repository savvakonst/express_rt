
#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "device/DeviceManager.h"

static ExtensionUnit *g_common_extension_uint;
static ExtensionInfo g_common_extension_info;

InitExtension(ExtensionInfo *) initCommonLib(void) {
    g_common_extension_uint = new ExtensionUnit[]{
        {"device_manager", "device_manager", "pass", (void *)new DeviceManager(), 0x00},
        {"cnv_template_manager", "cnv_template_manager", "pass", (void *)new ConversionTemplateManager(), 0x00},
        {nullptr, nullptr, nullptr, nullptr, 0x00}};

    g_common_extension_info = {"common_extension", 0x01, g_common_extension_uint};

    return &g_common_extension_info;
}