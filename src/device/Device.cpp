

#include "Device/Device.h"

Device::Device(/* args */) {}

Device::~Device() {}

exo_container<const Module_ifs*> Device::getAllModules() {}

const Module_ifs* Device::getModuleFromPath(std::string name) {}

exo_container<const Module_ifs*> Device::getLineFromPath(std::string path) {}

status Device::checkValExistence(std::string path) {}

status Device::isTranciverEnabled() {}

std::string Device::getSrcAddress() {}
