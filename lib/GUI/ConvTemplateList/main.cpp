
#include <QApplication>
//
#include "common/ExtensionManager.h"
//

#ifndef CONV_TEMPLATE_LIST_LIB_NAME
#    error "CONV_TEMPLATE_LIST_LIB_NAME undefined"
#endif

QWidget *newParameterTreeView(QWidget *parent);
QWidget *newConversionTemplateView(QWidget *parent);
//
int initParameterTreeView(ExtensionManager *manager);
int initConversionTemplateView(ExtensionManager *manager);
//
static int initWidgets(ExtensionManager *manager);
static ExtensionUnit *g_conv_template_list_extension_uint = nullptr;
static ExtensionInfo g_conv_template_list_extension_info;

#define CONV_TEMPLATE_LIST_VER 0x00
#define PARAMETER_LIST_VER 0x00

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, CONV_TEMPLATE_LIST_LIB_NAME)(void) {
    if (QCoreApplication::instance() == nullptr) return nullptr;

    g_conv_template_list_extension_uint = new ExtensionUnit[]{
        {"conv_template_list", "widget",
         "returns widget instance, which provides list of available conversion templates",
         (newConversionTemplateView(nullptr)), CONV_TEMPLATE_LIST_VER},
        {"parameter_list", "widget", "returns widget instance, which provides list of available parameters",
         (newParameterTreeView(nullptr)), PARAMETER_LIST_VER},
        {"conv_template_list", "init", "checks the version and, if it's the latest, initializes the widget.",
         (void *)&initWidgets, 0x00},
        {nullptr, nullptr, nullptr, nullptr, 0}};

    g_conv_template_list_extension_info = {"conv_template_list_extension", 0x01, g_conv_template_list_extension_uint};

    return &g_conv_template_list_extension_info;
}

static int initWidgets(ExtensionManager *manager) {
    //
    auto p_unit = manager->getLastVersionExtensionUint("widget", "conv_template_list");
    if ((p_unit == nullptr) || (p_unit->version != CONV_TEMPLATE_LIST_VER)) {
        DEBUG_CERR("cant init (name: " << p_unit->name << ", type: " << p_unit->type << ", ver.:" << p_unit->version
                                       << ") unit, since there is a newer unit.\n");
    } else
        initConversionTemplateView(manager);

    //
    p_unit = manager->getLastVersionExtensionUint("widget", "parameter_list");
    if ((p_unit == nullptr) || (p_unit->version != CONV_TEMPLATE_LIST_VER)) {
        DEBUG_CERR("cant init (name: " << p_unit->name << ", type: " << p_unit->type << ", ver.:" << p_unit->version
                                       << ") unit, since there is a newer unit.\n");
    } else
        initParameterTreeView(manager);

    return 0;
}