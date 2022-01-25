
#include "common/DataSchema_ifs.h"

std::string toString(DataSchema_ifs *arg, const std::string &intend) {
    std::string field_name = intend + arg->name_;
    auto dims = arg->getDims();
    for (auto &i : dims) field_name += "[" + std::to_string(i) + "]";

    std::string type = ":\n";
    auto map_list = arg->getMapList();

    if (arg->isMap())
        for (auto i : map_list) type += toString(i, intend + "  ");
    else {
        type = ": " + arg->getType() + "\n";
    }
    field_name += type;
    return field_name;
}
