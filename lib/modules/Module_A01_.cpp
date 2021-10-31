

#include "Module_A01_.h"

Module_A01_::Module_A01_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {32, TaskMapper({{"frequency", u8},
                                        {"reserved", {2, u8}},
                                        {"flags", u8},
                                        {"rangeH", i32},
                                        {"rangeL", i32},
                                        {"ureserved", u32}})}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A01_::Module_A01_(const void *ptr, size_t size, DeviceBuildingContext_ifs *context) : Module_A01_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task *)ptr);
}
