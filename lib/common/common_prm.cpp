#include "convtemplate/Parameter_ifs.h"

// use BOM (Byte Order Mark)
/*
"([^"]*)"
{DataType::string_val,u8"$1",u8"$1 description",u8"$1 help"},
*/

std::vector<FieldInfo*> g_common_parameter_fields = {
    new FieldInfo{DataType::str_v, u8"common.name", u8"name descripton", u8"name help"},
    new FieldInfo{DataType::str_v, u8"common.identifier", u8"identifier descripton ", u8"identifier help"},
    new FieldInfo{DataType::str_v, u8"common.dimension", u8"dimension description", u8"dimension help"},
    new FieldInfo{DataType::str_v, u8"common.category", u8"category description", u8"category help"},
    new FieldInfo{DataType::str_v, u8"common.system", u8"system description", u8"system help"},
    new FieldInfo{DataType::str_v, u8"common.department", u8"department description", u8"department help"},
    new FieldInfo{DataType::str_v, u8"common.description", u8"description description", u8"description help"},
    new FieldInfo{DataType::str_v, u8"common.path", u8"физический адрес параметра", u8"path description"}};

std::map<std::string, std::vector<FieldInfo>> g_map_of_all_parameters;
